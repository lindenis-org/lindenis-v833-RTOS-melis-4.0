#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <rtthread.h>
#include <blkpart.h>
#include "mbr.h"
#include <sdmmc/hal_sdhost.h>
#include <sdmmc/card.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

/* #define ALIGN(x, a) __ALIGN_KERNEL((x), (a)) */
#define ALIGN_DOWN(x, a) __ALIGN_KERNEL((x) - ((a) - 1), (a))
#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

struct mmc_card *mmc_card_open(uint8_t card_id);
int32_t mmc_card_close(uint8_t card_id);

static struct blkpart sdmmcblk;

struct syspart
{
    char name[MAX_BLKNAME_LEN];
    u32 offset;
    u32 bytes;
};
static const struct syspart syspart[] =
{
};

static struct part all_part[16];

static void show_mbr_part(void *mbr_buf)
{
    int i = 0;

    MBR *pMBR = (MBR *)mbr_buf;

    printf("MBR:\n");
    printf("classname        name      size     offset\n");
    for (i = 0; i < pMBR->PartCount; i++)
    {
        int offset      = pMBR->array[i].addrlo;
        int partsize    = pMBR->array[i].lenlo;
        int secsize     = 512;

        printf("%s        %s       %d        %d\n",
               (const char *)pMBR->array[i].classname,
               (const char *)pMBR->array[i].name,
               partsize,
               offset);
    }
}

int mbr_part_cnt(void *mbr_buf)
{
    MBR *pMBR = (MBR *)mbr_buf;
    return pMBR->PartCount;
}

static int sdmmc_get_mbr(rt_device_t dev, char *buf, int len)
{
    int ret = 0;
    if (len < MBR_SIZE)
    {
        pr_err("buf too small for mbr\n");
        return -EINVAL;
    }

    pr_debug("read mbr from 0x%x\n", MBR_START_ADDRESS);
    ret = dev->read(dev, MBR_START_ADDRESS, buf, MBR_SIZE);
    return ret < 0 ? -EIO : 0;
}

static int register_part(rt_device_t dev, struct part *part)
{
    int ret = -1;
    struct rt_device *device;

    device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }

    device->read = part_read;
    device->write = part_erase_without_write;
    device->control = part_control;
    device->user_data = part;

    ret = rt_device_register(device, part->name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        free(device);
        return ret;
    }
    return 0;
}

static int unregister_part(rt_device_t dev)
{
    int ret = -1;
    ret = rt_device_unregister(dev);
    if (ret != 0)
    {
        return ret;
    }
    rt_device_destroy(dev);
    return 0;
}

static int register_blk_device(rt_device_t dev)
{
    int ret = -1, index = 0;
    char *mbr_buf;
    struct mbr_part *mbr_part;
    struct part *part;
    unsigned int offset = 0;
    int i = 0;

    mbr_buf = malloc(MBR_SIZE);
    if (!mbr_buf)
    {
        ret = -ENOMEM;
        goto err;
    }
    memset(mbr_buf, 0, MBR_SIZE);

    dev->control(dev, BLOCK_DEVICE_CMD_GET_PAGE_SIZE, &sdmmcblk.page_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_TOTAL_SIZE, &sdmmcblk.total_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_BLOCK_SIZE, &sdmmcblk.blk_bytes);
    sdmmcblk.dev = dev;

    pr_debug("total_bytes = %lld\n", sdmmcblk.total_bytes);
    pr_debug("blk_bytes = %d\n", sdmmcblk.blk_bytes);
    pr_debug("page_bytes = %d\n", sdmmcblk.page_bytes);

#if 0
    show_mbr_part(mbr_buf);
    ret = mbr_part_cnt(mbr_buf);
    if (ret < 0)
    {
        pr_err("get part count from mbr failed\n");
        goto err;
    }

    printf("part cnt = %d\n", ret);
    sdmmcblk.n_parts = ret + ARRAY_SIZE(syspart);
    sdmmcblk.parts = malloc(sizeof(struct part) * sdmmcblk.n_parts);
    if (sdmmcblk.parts == NULL)
    {
        pr_err("allocate part array failed.\n");
        goto err;
    }
    memset(sdmmcblk.parts, 0, sizeof(struct part) * sdmmcblk.n_parts);

    for (index = 0; index < ARRAY_SIZE(syspart); index++)
    {
        part = &sdmmcblk.parts[index];
        part->blk = &sdmmcblk;
        part->bytes = syspart[index].bytes;
        part->off = syspart[index].offset;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", syspart[index].name);
        offset += part->bytes;
    }

    MBR *pMBR = (MBR *)mbr_buf;

    for (i = 0; i < pMBR->PartCount; i++)
    {
        part = &sdmmcblk.parts[index++];
        part->blk = &sdmmcblk;
        part->bytes = pMBR->array[i].lenlo;
        part->off = pMBR->array[i].addrlo;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", pMBR->array[i].name);
        offset += part->bytes;
    }

    sdmmcblk.parts[--index].bytes = sdmmcblk.total_bytes - offset;

    for (index = 0; index < sdmmcblk.n_parts; index++)
    {
        part = &sdmmcblk.parts[index];
        if (part->bytes % sdmmcblk.blk_bytes)
        {
            pr_err("part %s with bytes %u should align to block size %u\n",
                   part->name, part->bytes, sdmmcblk.blk_bytes);
        }
        else
        {
            register_part(dev, part);
        }
    }
    blkpart_add_list(&sdmmcblk);
#else
    sdmmcblk.n_parts = 1;
    sdmmcblk.parts = malloc(sizeof(struct part) * sdmmcblk.n_parts);
    if (sdmmcblk.parts == NULL)
    {
        pr_err("allocate part array failed.\n");
        goto err;
    }
    memset(sdmmcblk.parts, 0, sizeof(struct part) * sdmmcblk.n_parts);

    struct mmc_card *card = mmc_card_open((int)dev->user_data);

    part = &sdmmcblk.parts[index++];
    part->blk = &sdmmcblk;
    part->bytes = card->csd.capacity * 1024;
    part->off = 0;
    snprintf(part->name, MAX_BLKNAME_LEN, "%s", "SDMMC-DISK");

    register_part(dev, part);
    blkpart_add_list(&sdmmcblk);

    mmc_card_close((int)dev->user_data);
    ret = 0;
#endif
err:
    free(mbr_buf);
    return ret;
}

static int unregister_blk_device(rt_device_t dev)
{
    int ret = -1;

    rt_device_t part_dev = rt_device_find("SDMMC-DISK");
    if (!part_dev)
    {
        return ret;
    }

    ret = unregister_part(part_dev);
    if (ret != 0)
    {
        return ret;
    }
    blkpart_del_list(&sdmmcblk);
    free(sdmmcblk.parts);
    return 0;
}

int sdmmc_blkpart_init(const char *sdmmc_device_name)
{
    int ret = -1;
    rt_device_t dev = rt_device_find(sdmmc_device_name);
    if (!dev)
    {
        pr_err("the sdmmc device %s is not exist!\n", sdmmc_device_name);
        return ret;
    }

    if (dev->init)
    {
        ret = dev->init(dev);
    }

    return ret;
}

void melis_part_init(int card_id);
void melis_part_deinit(void);

int mount(
    const char *device_name,
    const char *path,
    const char *filesystemtype,
    unsigned long rwflag,
    const void *data);

int umount(const char *specialfile);

void mount_sdmmc_filesystem(int card_id)
{
#ifdef CONFIG_RT_USING_DFS_ELMFAT
    int ret = -1;
    rt_device_t dev = rt_device_find("sdmmc");
    if (!dev)
    {
        pr_err("the sdmmc device [sdmmc] is not exist!\n");
        return;
    }

    ret = register_blk_device(dev);
    if (ret == 0)
    {
        ret = mount("SDMMC-DISK", "/sdmmc", "elm", 0, 0);
        if (ret != 0)
        {
            unregister_blk_device(dev);
            return;
        }
    }
#elif defined CONFIG_MELIS_LEGACY_DRIVER_MAN
    melis_part_init(card_id);
#else
#warning "Not filesystem support!"
#endif
    return;
}

void unmount_sdmmc_filesystem(void)
{
#ifdef CONFIG_RT_USING_DFS_ELMFAT
    int ret = -1;

    rt_device_t dev = rt_device_find("sdmmc");
    if (!dev)
    {
        pr_err("the sdmmc device [sdmmc] is not exist!\n");
        return;
    }

    ret = umount("/sdmmc");
    if (ret == 0)
    {
        unregister_blk_device(dev);
    }
#elif defined CONFIG_MELIS_LEGACY_DRIVER_MAN
    melis_part_deinit();
#else
#warning "Not filesystem support!"
#endif
    return;
}
