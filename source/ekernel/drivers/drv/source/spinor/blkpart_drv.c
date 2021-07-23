#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <sunxi_drv_spinor.h>
#include <rtthread.h>
#include <blkpart.h>
#include <part_efi.h>
#include <debug.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

/* #define ALIGN(x, a) __ALIGN_KERNEL((x), (a)) */
#define ALIGN_DOWN(x, a) __ALIGN_KERNEL((x) - ((a) - 1), (a))
#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

static struct blkpart norblk;

struct syspart
{
    char name[MAX_BLKNAME_LEN];
    u32 offset;
    u32 bytes;
};
static const struct syspart syspart[] =
{
    {"boot0", 0, 48 * 1024},
};

static int nor_get_gpt(rt_device_t dev, char *buf, int len)
{
    int ret = 0;
    if (len < GPT_TABLE_SIZE)
    {
        pr_err("buf too small for gpt\n");
        return -EINVAL;
    }

    pr_debug("read gpt from 0x%x\n", GPT_ADDRESS);
    ret = dev->read(dev, GPT_ADDRESS, buf, GPT_TABLE_SIZE);
    return ret <= 0 ? -EIO : 0;
}

#if defined(RT_USING_POSIX)
#include <dfs_file.h>
#include <rtdevice.h>
#include <string.h>

static int part_fops_open(struct dfs_fd *fd)
{
    rt_device_t dev;
    uint32_t total_bytes = 0;

    dev = (rt_device_t) fd->data;
    dev->control(dev, DEVICE_PART_CMD_GET_TOTAL_SIZE, &total_bytes);

    fd->size = total_bytes;
    fd->type  = FT_DEVICE;

    return 0;
}

static int part_fops_close(struct dfs_fd *fd)
{
    return 0;
}

static int part_fops_ioctl(struct dfs_fd *fd, int cmd, void *args)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return dev->control(dev, cmd, args);
}

static int part_fops_read(struct dfs_fd *fd, void *buf, size_t count)
{
    rt_device_t dev;
    int ret = -1;
    dev = (rt_device_t) fd->data;

    ret = dev->read(dev, fd->pos, buf, count);
    if (ret >= 0)
    {
        fd->pos += ret;
    }

    return ret;
}

static int part_fops_write(struct dfs_fd *fd, const void *buf, size_t count)
{
    rt_device_t dev;
    int ret = -1;
    dev = (rt_device_t) fd->data;

    ret = part_erase_before_write(dev, fd->pos, buf, count);
    if (ret >= 0)
    {
        fd->pos += ret;
    }

    return ret;
}

static int part_fops_lseek(struct dfs_fd *fd, off_t offset)
{
    int ret = -1;

    if (fd->flags & O_DIRECTORY)
    {
        ret = -1;
    }
    else
    {
        if (offset > fd->size)
        {
            ret = -1;
        }
        else
        {
            fd->pos = offset;
            ret = fd->pos;
        }
    }
    return ret;
}

static struct dfs_file_ops part_device_fops =
{
    .open = part_fops_open,
    .close = part_fops_close,
    .ioctl = part_fops_ioctl,
    .lseek = part_fops_lseek,
    .read = part_fops_read,
    .write = part_fops_write,
};
#endif

int register_part(rt_device_t dev, struct part *part)
{
    int ret = -1;
    struct rt_device *device;

    device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }

    device->read = part_read;
    device->control = part_control;
    if (part->erase_flag)
    {
        device->write = part_erase_before_write;
    }
    else
    {
        device->write = part_erase_without_write;
    }

    device->user_data = part;

    ret = rt_device_register(device, part->name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        rt_device_destroy(device);
        return ret;
    }

#if defined(RT_USING_POSIX)
    device->fops = &part_device_fops;
#endif

    return 0;
}

static int register_blk_device(rt_device_t dev)
{
    int ret = -1, index = 0;
    char *gpt_buf;
    struct gpt_part *gpt_part;
    struct part *part;
    unsigned int offset = 0;
    unsigned int *pint64;

    gpt_buf = malloc(GPT_TABLE_SIZE);
    if (!gpt_buf)
    {
        ret = -ENOMEM;
        goto err;
    }
    memset(gpt_buf, 0, GPT_TABLE_SIZE);

    dev->control(dev, BLOCK_DEVICE_CMD_GET_PAGE_SIZE, &norblk.page_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_TOTAL_SIZE, &norblk.total_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_BLOCK_SIZE, &norblk.blk_bytes);
    norblk.dev = dev;

    pint64 = (unsigned int *)&norblk.total_bytes;
    pr_debug("total_bytes = 0x%08x%08x\n", pint64[1], pint64[0]);
    pr_debug("blk_bytes = %d\n", norblk.blk_bytes);
    pr_debug("page_bytes = %d\n", norblk.page_bytes);

    ret = nor_get_gpt(dev, gpt_buf, GPT_TABLE_SIZE);
    if (ret)
    {
        pr_err("get gpt from nor flash failed - %d\n", ret);
        goto err;
    }

#ifdef DEBUG
    show_gpt_part(gpt_buf);
#endif

    ret = gpt_part_cnt(gpt_buf);
    if (ret < 0)
    {
        pr_err("get part count from gpt failed\n");
        goto err;
    }

    norblk.n_parts = ret + ARRAY_SIZE(syspart);
    norblk.parts = malloc(sizeof(struct part) * norblk.n_parts);
    if (norblk.parts == NULL)
    {
        pr_err("allocate part array failed.\n");
        goto err;
    }
    memset(norblk.parts, 0, sizeof(struct part) * norblk.n_parts);

    for (index = 0; index < ARRAY_SIZE(syspart); index++)
    {
        part = &norblk.parts[index];
        part->blk = &norblk;
        part->bytes = syspart[index].bytes;
        part->off = syspart[index].offset;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", syspart[index].name);
        offset += part->bytes;
        part->erase_flag = 1;
    }

    offset += GPT_TABLE_SIZE;

    foreach_gpt_part(gpt_buf, gpt_part)
    {
        part = &norblk.parts[index++];
        part->blk = &norblk;
        part->bytes = gpt_part->sects << SECTOR_SHIFT;
        /* part->off = gpt_part->off_sects << SECTOR_SHIT + GPT_ADDRESS; */
        part->off = gpt_part->off_sects << SECTOR_SHIFT;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", gpt_part->name);
        offset += part->bytes;
        if (!strcmp(part->name, "ROOTFS"))
        {
            part->erase_flag = 0;
        }
        else if (!strcmp(part->name, "data"))
        {
            part->erase_flag = 0;
        }
        else
        {
            part->erase_flag = 1;
        }
    }

    part = &norblk.parts[--index];
    part->bytes = norblk.total_bytes - (offset - part->bytes);

    for (index = 0; index < norblk.n_parts; index++)
    {
        part = &norblk.parts[index];
        if (part->bytes % norblk.blk_bytes)
        {
            pr_err("part %s with bytes %u should align to block size %u\n",
                   part->name, part->bytes, norblk.blk_bytes);
        }
        else
        {
            register_part(dev, part);
        }
    }
    blkpart_add_list(&norblk);
err:
    free(gpt_buf);
    return ret;
}

int nor_blkpart_init(const char *spinor_device_name)
{
    int ret = -1;
    rt_device_t dev = rt_device_find(spinor_device_name);
    if (!dev)
    {
        pr_err("the spinor device %s is not exist!\n", spinor_device_name);
        return ret;
    }

    if (dev->init)
    {
        dev->init(dev);
    }

    ret = register_blk_device(dev);
    return ret;
}
