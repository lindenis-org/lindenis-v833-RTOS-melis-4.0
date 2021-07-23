#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <rtthread.h>

#include <init.h>

#include <blkpart.h>

#include <sdmmc/hal_sdhost.h>
#include <sdmmc/card.h>
#include <sdmmc/sys/sys_debug.h>

#include <typedef.h>
#include <kapi.h>

#ifdef CONFIG_SUPPORT_SDMMC_CACHE
#include "sdmmc_cache.h"
#endif

#define DETECT_BY_GPIO

#ifdef CONFIG_SDC_DMA_BUF_SIZE
#define SDXC_MAX_TRANS_LEN              (CONFIG_SDC_DMA_BUF_SIZE * 1024)
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))
#endif

#ifndef MIN
#define MIN(a, b) (a > b ? b : a)
#endif

void card_detect(uint32_t present);

int32_t mmc_test_init(uint32_t host_id, SDC_InitTypeDef *sdc_param, uint32_t scan);
struct mmc_card *mmc_card_open(uint8_t card_id);
int32_t mmc_card_close(uint8_t card_id);
int32_t mmc_block_read(struct mmc_card *card, uint8_t *buf, uint64_t sblk, uint32_t nblk);
int32_t mmc_block_write(struct mmc_card *card, const uint8_t *buf, uint64_t sblk, uint32_t nblk);

static rt_err_t sunxi_sdmmc_init(rt_device_t dev)
{
    int ret = -1;
    int host_id = 0;
    dev->flag |= RT_DEVICE_FLAG_ACTIVATED;

    SDC_InitTypeDef sdc_param = {0};

    sdc_param.debug_mask = (ROM_INF_MASK | \
                            ROM_WRN_MASK | ROM_ERR_MASK | ROM_ANY_MASK);

#ifndef DETECT_BY_GPIO
    sdc_param.cd_mode = CARD_ALWAYS_PRESENT;
#else
    sdc_param.cd_mode = CARD_DETECT_BY_GPIO_IRQ;
#endif

    sdc_param.cd_cb = &card_detect;
    sdc_param.dma_use = 1;

    if (mmc_test_init(host_id, &sdc_param, 1))
    {
        dev->flag &= ~RT_DEVICE_FLAG_ACTIVATED;
        printf("init sdmmc failed!\n");
        return ret;
    }

    dev->user_data = (void *)host_id;
    return 0;
}

static rt_size_t sunxi_sdmmc_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    int err = -1;
    ssize_t ret, sz = 0;
    struct rt_device_blk_geometry geometry;
    unsigned long long total_bytes;
    char *sector_buf = NULL;
    uint8_t *data = buffer;

    struct mmc_card *card = mmc_card_open((int)dev->user_data);
    if (card == NULL)
    {
        printf("mmc open fail\n");
        return -EIO;
    }

    if (size == 0)
    {
        return 0;
    }

    memset(&geometry, 0, sizeof(struct rt_device_blk_geometry));
    err = dev->control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
    if (err)
    {
        return -EIO;
    }
    total_bytes = geometry.bytes_per_sector * geometry.sector_count;

    if (pos >= total_bytes)
    {
        printf("read offset %lu over part size %llu\n", pos, total_bytes);
        return 0;
    }

    if (pos + size > total_bytes)
    {
        printf("over limit: offset %lu + size %lu over %llu\n",
               pos, size, total_bytes);
    }

    size = MIN(total_bytes - pos, size);
    pr_debug("off 0x%lx size %lu\n", pos, size);

    if (pos % geometry.bytes_per_sector || size % geometry.bytes_per_sector)
    {
        sector_buf = malloc(geometry.bytes_per_sector);
        if (!sector_buf)
        {
            return -ENOMEM;
        }
        memset(sector_buf, 0, geometry.bytes_per_sector);
    }

    /**
     * Step 1:
     * read the beginning data that not align to block size
     */
    if (pos % geometry.bytes_per_sector)
    {
        uint32_t addr, poff, len;

        addr = ALIGN_DOWN(pos, geometry.bytes_per_sector);
        poff = pos - addr;
        len = MIN(geometry.bytes_per_sector - poff, size);

        pr_debug("offset %lu not align %u, fix them before align read\n",
                 pos, geometry.bytes_per_sector);
        pr_debug("step1: read page data from addr 0x%x\n", addr);
        ret = mmc_block_read(card, sector_buf, addr / 512, 1);
        if (ret != 0)
        {
            goto err;
        }

        pr_debug("step2: copy page data to buf with page offset 0x%x and len %u\n",
                 poff, len);
        memcpy(data, sector_buf + poff, len);

        pos += len;
        buffer += len;
        sz += len;
        size -= len;
    }

    /**
     * Step 2:
     * read data that align to block size
     */
    while (size >= geometry.bytes_per_sector)
    {
        if (size < SDXC_MAX_TRANS_LEN)
        {
            ret = mmc_block_read(card, data, pos / 512, size / geometry.bytes_per_sector);
            if (ret)
            {
                goto err;
            }
            pos += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            data += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            sz += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            size -= geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
        }
        else
        {
            while (size > SDXC_MAX_TRANS_LEN)
            {
                ret = mmc_block_read(card, data, pos / 512, SDXC_MAX_TRANS_LEN / 512);
                if (ret)
                {
                    goto err;
                }
                size -= SDXC_MAX_TRANS_LEN;
                data += SDXC_MAX_TRANS_LEN;
                pos += SDXC_MAX_TRANS_LEN;
                sz += SDXC_MAX_TRANS_LEN;
            }
        }
    }

    /**
     * Step 3:
     * read the last data that not align to block size
     */
    if (size)
    {
        pr_debug("last size %u not align %u, read them\n", size, geometry.bytes_per_sector);

        pr_debug("step1: read page data from addr 0x%lx\n", pos);
        ret = mmc_block_read(card, sector_buf, pos / 512, 1);
        if (ret != 0)
        {
            goto err;
        }

        pr_debug("step2: copy page data to buf with page with len %u\n", size);
        memcpy(data, sector_buf, size);
        sz += size;
    }

    ret = 0;
    goto out;

err:
    pr_err("read failed - %d\n", (int)ret);
out:
    if (sector_buf)
    {
        free(sector_buf);
    }
    mmc_card_close((int)dev->user_data);

    return ret ? ret : sz;
}

static rt_err_t sunxi_sdmmc_open(rt_device_t dev, rt_uint16_t oflag)
{
    return 0;
}

static rt_err_t sunxi_sdmmc_close(rt_device_t dev)
{
    return 0;
}

static rt_size_t sunxi_sdmmc_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    int err = -1;
    ssize_t ret, sz = 0;
    struct rt_device_blk_geometry geometry;
    unsigned long long total_bytes;
    char *sector_buf = NULL;
    uint8_t *data = (uint8_t *)buffer;

    struct mmc_card *card = mmc_card_open((int)dev->user_data);
    if (card == NULL)
    {
        printf("mmc open fail\n");
        return -EIO;
    }

    if (size == 0)
    {
        return 0;
    }

    memset(&geometry, 0, sizeof(struct rt_device_blk_geometry));
    err = dev->control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
    if (err)
    {
        return -EIO;
    }
    total_bytes = ((uint64_t)geometry.bytes_per_sector) * geometry.sector_count;

    if (pos >= total_bytes)
    {
        printf("read offset %lu over part size %llu\n", pos, total_bytes);
        return 0;
    }

    if (pos + size > total_bytes)
    {
        printf("over limit: offset %lu + size %lu over %llu\n",
               pos, size, total_bytes);
    }

    size = MIN(total_bytes - pos, size);
    pr_debug("off 0x%lx size %lu\n", pos, size);

    if (pos % geometry.bytes_per_sector || size % geometry.bytes_per_sector)
    {
        sector_buf = malloc(geometry.bytes_per_sector);
        if (!sector_buf)
        {
            return -ENOMEM;
        }
        memset(sector_buf, 0, geometry.bytes_per_sector);
    }

    /**
     * Step 1:
     * read the beginning data that not align to block size
     */
    if (pos % geometry.bytes_per_sector)
    {
        uint32_t addr, poff, len;

        addr = ALIGN_DOWN(pos, geometry.bytes_per_sector);
        poff = pos - addr;
        len = MIN(geometry.bytes_per_sector - poff, size);

        pr_debug("offset %lu not align %u, fix them before align read\n",
                 pos, geometry.bytes_per_sector);
        pr_debug("step1: read page data from addr 0x%x\n", addr);
        ret = mmc_block_write(card, sector_buf, addr / 512, 1);
        if (ret != 0)
        {
            goto err;
        }

        pr_debug("step2: copy page data to buf with page offset 0x%x and len %u\n",
                 poff, len);
        memcpy(data, sector_buf + poff, len);

        pos += len;
        buffer += len;
        sz += len;
        size -= len;
    }

    /**
     * Step 2:
     * read data that align to block size
     */
    while (size >= geometry.bytes_per_sector)
    {
        if (size < SDXC_MAX_TRANS_LEN)
        {
            ret = mmc_block_write(card, data, pos / 512, size / geometry.bytes_per_sector);
            if (ret)
            {
                goto err;
            }
            pos += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            data += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            sz += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            size -= geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
        }
        else
        {
            while (size > SDXC_MAX_TRANS_LEN)
            {
                ret = mmc_block_write(card, data, pos / 512, SDXC_MAX_TRANS_LEN / 512);
                if (ret)
                {
                    goto err;
                }
                size -= SDXC_MAX_TRANS_LEN;
                data += SDXC_MAX_TRANS_LEN;
                pos += SDXC_MAX_TRANS_LEN;
                sz += SDXC_MAX_TRANS_LEN;
            }
        }
    }

    /**
     * Step 3:
     * read the last data that not align to block size
     */
    if (size)
    {
        pr_debug("last size %u not align %u, read them\n", size, geometry.bytes_per_sector);

        pr_debug("step1: read page data from addr 0x%llx\n", pos);
        ret = mmc_block_write(card, sector_buf, pos / 512, 1);
        if (ret != 0)
        {
            goto err;
        }

        pr_debug("step2: copy page data to buf with page with len %u\n", size);
        memcpy(data, sector_buf, size);
        sz += size;
    }

    ret = 0;
    goto out;

err:
    pr_err("read failed - %d\n", (int)ret);
out:
    if (sector_buf)
    {
        free(sector_buf);
    }
    mmc_card_close((int)dev->user_data);

    return ret ? ret : sz;
}

static rt_err_t sunxi_sdmmc_control(rt_device_t dev, int cmd, void *args)
{
    int ret = 0;
    blk_dev_erase_t *erase_sector;
    struct rt_device_blk_geometry *geometry;
    int flag = 0;

    if (!dev)
    {
        return -EINVAL;
    }

    struct mmc_card *card = mmc_card_open((int)dev->user_data);
    if (!card)
    {
        return ret;
    }

    switch (cmd)
    {
        case BLOCK_DEVICE_CMD_ERASE_ALL:
            break;
        case BLOCK_DEVICE_CMD_ERASE_SECTOR:
            break;
        case BLOCK_DEVICE_CMD_GET_TOTAL_SIZE:
            *(uint64_t *)args = card->csd.capacity * 1024;
            ret = 0;
            break;
        case BLOCK_DEVICE_CMD_GET_PAGE_SIZE:
            *(uint32_t *)args = 512;
            ret = 0;
            break;
        case BLOCK_DEVICE_CMD_GET_BLOCK_SIZE:
            *(uint32_t *)args = 512;
            ret = 0;
            break;
        case RT_DEVICE_CTRL_BLK_GETGEOME:
            geometry = (struct rt_device_blk_geometry *)args;
            memset(geometry, 0, sizeof(struct rt_device_blk_geometry));
            geometry->block_size = 512;
            geometry->bytes_per_sector = 512;
            geometry->sector_count = (card->csd.capacity * 1024) / geometry->bytes_per_sector;
            ret = 0;
            break;
        default:
            break;
    }

    mmc_card_close((int)dev->user_data);
    return ret;
}

#ifdef CONFIG_MELIS_LEGACY_DRIVER_MAN
typedef struct __DEV_SDMMC
{
    __u32           card_no;
    __u32           boot;
    __u32           offset;
    __u32           used;
    char            name[32];
    char            major_name[16];
    char            minor_name[16];
    __hdle          hReg;
    __dev_blkinfo_t info;
} __dev_sdmmc_t;

static __s32 sdmmc_dev_ioctrl(__hdle hDev, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    __dev_sdmmc_t *pDev = (__dev_sdmmc_t *)hDev;

    switch (Cmd)
    {
        case DEV_CMD_GET_INFO:
        {
            if (!pBuffer)
            {
                return EPDK_FAIL;
            }

            *((__dev_blkinfo_t *)pBuffer) = pDev->info;
            return EPDK_OK;
        }
        case DEV_IOC_USR_FLUSH_CACHE:
        {
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
            flush_block_cache(1);
#endif
            return EPDK_OK;
        }
        default:
            break;
    }
    return EPDK_FAIL;
}

static __hdle sdmmc_dev_open(void *open_arg, __u32 Mode)
{
    __dev_sdmmc_t *pDev = (__dev_sdmmc_t *)open_arg;

    return pDev;
}

static __s32 sdmmc_dev_close(__hdle hDev)
{
    return 0;
}

__u32 sdmmc_dev_phy_read(void *pBuffer, __u32 blk, __u32 n, __hdle hDev)
{
    __dev_sdmmc_t *pDev = (__dev_sdmmc_t *)hDev;
    __u32 nblk = n;
    __u32 offset = 0;
    int readn = 0;
    int error;

    struct mmc_card *card = mmc_card_open(pDev->card_no);
    if (card == NULL)
    {
        printf("mmc open fail\n");
        return readn;
    }
    if (n < SDXC_MAX_TRANS_LEN / 512)
    {
        error = mmc_block_read(card, pBuffer, blk, n);
        if (error < 0)
        {
            mmc_card_close(pDev->card_no);
            return readn;
        }
        readn += n;
    }
    else
    {
        while (nblk > SDXC_MAX_TRANS_LEN / 512)
        {
            error = mmc_block_read(card, pBuffer + offset, blk, SDXC_MAX_TRANS_LEN / 512);
            if (error < 0)
            {
                mmc_card_close(pDev->card_no);
                return readn;
            }
            nblk -= SDXC_MAX_TRANS_LEN / 512;
            blk += SDXC_MAX_TRANS_LEN / 512;
            offset += SDXC_MAX_TRANS_LEN;
            readn += SDXC_MAX_TRANS_LEN / 512;
        }
        if (nblk > 0)
        {
            error = mmc_block_read(card, pBuffer + offset, blk, nblk);
            if (error < 0)
            {
                mmc_card_close(pDev->card_no);
                return readn;
            }
            readn += nblk;
        }
    }
    mmc_card_close(pDev->card_no);
    return readn;
}

__u32 sdmmc_dev_phy_write(const void *pBuffer, __u32 blk, __u32 n, __hdle hDev)
{
    __dev_sdmmc_t *pDev = (__dev_sdmmc_t *)hDev;
    __u32 nblk = n;
    __u32 offset = 0;
    int error = 0;
    int writen = 0;

    struct mmc_card *card = mmc_card_open(pDev->card_no);
    if (card == NULL)
    {
        printf("mmc open fail\n");
        return 0;
    }
    if (n < SDXC_MAX_TRANS_LEN / 512)
    {
        error = mmc_block_write(card, pBuffer, blk, n);
        if (error < 0)
        {
            mmc_card_close(pDev->card_no);
            return writen;
        }
        writen += n;
    }
    else
    {
        while (nblk > SDXC_MAX_TRANS_LEN / 512)
        {
            error = mmc_block_write(card, pBuffer + offset, blk, SDXC_MAX_TRANS_LEN / 512);
            if (error < 0)
            {
                mmc_card_close(pDev->card_no);
                return writen;
            }
            nblk -= SDXC_MAX_TRANS_LEN / 512;
            blk += SDXC_MAX_TRANS_LEN / 512;
            offset += SDXC_MAX_TRANS_LEN;
            writen += SDXC_MAX_TRANS_LEN / 512;
        }
        if (nblk > 0)
        {
            error = mmc_block_write(card, pBuffer + offset, blk, nblk);
            if (error < 0)
            {
                mmc_card_close(pDev->card_no);
                return writen;
            }
            writen += nblk;
        }
    }
    mmc_card_close(pDev->card_no);
    return writen;
}

__u32 sdmmc_dev_read(void *pBuffer, __u32 blk, __u32 n, __hdle hDev)
{
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    if (sdmmc_cache_read(pBuffer, (int)blk, (int)n, 512, hDev) == 0)
    {
        return n;
    }
    return 0;
#else
    return sdmmc_dev_phy_read(pBuffer, blk, n, hDev);
#endif
}

__u32 sdmmc_dev_write(const void *pBuffer, __u32 blk, __u32 n, __hdle hDev)
{
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    if (sdmmc_cache_write((void *)pBuffer, (int)blk, (int)n, 512, hDev) == 0)
    {
        return n;
    }
    return 0;
#else
    return sdmmc_dev_phy_write(pBuffer, blk, n, hDev);
#endif
}

__dev_devop_t sdmmc_dev_op =
{
    sdmmc_dev_open,
    sdmmc_dev_close,
    sdmmc_dev_read,
    sdmmc_dev_write,
    sdmmc_dev_ioctrl
};

__dev_sdmmc_t *pDev = NULL;

void melis_part_init(int card_id)
{
    int aux = 0;
    struct mmc_card *card;

    if (pDev != NULL)
    {
        if (pDev->hReg != NULL)
        {
            esDEV_DevUnreg(pDev->hReg);
        }
        free(pDev);
        pDev = NULL;
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
        discard_block_cache();
#endif
    }

    card = mmc_card_open(card_id);
    if (card == NULL)
    {
        __err("mmc open fail");
        return;
    }

    pDev = (__dev_sdmmc_t *)malloc(sizeof(__dev_sdmmc_t));
    if (NULL == pDev)
    {
        __err("allocate memory failed");
        mmc_card_close(card_id);
        return;
    }

#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    block_cache_manager_init(CONFIG_SDMMC_CACHE_SIZE);
#endif

    memset(pDev, 0, sizeof(__dev_sdmmc_t));
    snprintf(pDev->name, 32, "%s:%d", "SDMMC-DISK", aux);
    __log("sdcard device name: %s", pDev->name);

    pDev->card_no          = aux;
    pDev->boot             = 0;
    pDev->offset           = 0;
    pDev->used             = 0;
    pDev->info.hiddennum   = 0;
    pDev->info.headnum     = 2;
    pDev->info.secnum      = 4;
    pDev->info.partsize    = card->csd.capacity * 2;
    pDev->info.secsize     = 512;

    pDev->hReg = esDEV_DevReg("DISK", pDev->name, &sdmmc_dev_op, (void *)pDev);
    if (NULL == pDev->hReg)
    {
        __err("sdcard register partition failed");
    }

    mmc_card_close(card_id);
}

void melis_part_deinit(void)
{
    if (!pDev)
    {
        return;
    }
    esDEV_DevUnreg(pDev->hReg);
    free(pDev);
    pDev = NULL;
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    discard_block_cache();
#endif
}
#endif

int sunxi_driver_sdmmc_init(void)
{
    int ret = -1;
    struct rt_device *device;

    device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }

    device->init = sunxi_sdmmc_init;
    device->open = sunxi_sdmmc_open;
    device->close = sunxi_sdmmc_close;
    device->read = sunxi_sdmmc_read;
    device->write = sunxi_sdmmc_write;
    device->control = sunxi_sdmmc_control;
    /* device->user_data = (void *)&sunxi_hal_sdmmc_driver; */

    ret = rt_device_register(device, "sdmmc", RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_device_destroy(device);
        return ret;
    }
    int sdmmc_blkpart_init(const char *name);
    ret = sdmmc_blkpart_init("sdmmc");

    return ret;
}

int sunxi_driver_sdmmc_deinit(void)
{
    void shutdown_block_cache(void);
    shutdown_block_cache();
    return 0;
}

device_initcall(sunxi_driver_sdmmc_init);
