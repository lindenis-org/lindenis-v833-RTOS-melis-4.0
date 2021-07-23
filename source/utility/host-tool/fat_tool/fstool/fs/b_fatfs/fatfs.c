

#include "fatfs.h"

#include "fsys_debug.h"

struct file_system_type fat_fs_type;
extern int _FS_FAT_AutoFormat(__hdle pPart, __hdle fmtpara);
extern int fat_init_inodecache(void);
extern void fat_destroy_inodecache(void);

int fat_format(__hdle p_part, __hdle fmtpara)
{
    int res;

    fs_log_trace01("fat fomat enter...\n");

    res = _FS_FAT_AutoFormat(p_part, fmtpara);

    fs_log_trace01("fat fomat return(%x)\n", res);

    return res;
}

int fat_identify(__hdle pPart)
{
    __s32 res;

    unsigned char *buffer;

    fs_log_trace01("lfs identify enter...\n");

    buffer = palloc(512);
    if (!buffer)
    {
        fs_log_error("no memory for identify operation.\n");
        res = EPDK_FALSE;
        goto out;
    }

    res = esFSYS_pread((void *)buffer, 0, 1, pPart);
    if (res <= 0)
    {
        res = EPDK_FALSE;
        goto out;
    }

    if ((buffer[512 - 2] + 256 * buffer[512 - 1] != 0xaa55) ||
            (buffer[16] != 2))
    {
        res = EPDK_FALSE;
        goto out;
    }

    res = EPDK_TRUE;
out:
    pfree(buffer, 512);
    fs_log_trace01("lfs identify return(%x)\n", res);
    return res;
}

int fat_mount(__hdle part)
{
    int res;

    fs_log_trace01("fsmount:\n");

    res = get_sb_bdev(&fat_fs_type, 0, part, NULL, fat_fill_super);

    if (!res)
    {
        char *partName;

        esFSYS_pioctrl(part, PART_IOC_SYS_GETNAME, 0, &partName);
    }

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int fat_unmount(struct super_block *sb, int force)
{
    int res;

    fs_log_trace01("fat unmount...\n");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

struct file_system_type fat_fs_type =
{
    .name       = "fat",
    .mount      = fat_mount,
    .unmount    = fat_unmount,
    .identify   = fat_identify,
    .format     = fat_format,
    .fs_flags   = FS_REQUIRES_DEV,
};

/* ----------------------- init && exit --------------------*/
int fsys_fatfs_init(void)
{
    int err;

    err = fat_cache_init();
    if (err)
        goto out;

    err = fat_init_inodecache();
    if (err)
        goto free2;

    err = esFSYS_fsreg((__hdle)&fat_fs_type);
    if (err)
        goto free1;
    goto out;

free1:
    fat_destroy_inodecache();
free2:
    fat_cache_destroy();
out:
    return err;
}

int fsys_fatfs_exit(void)
{
    esFSYS_fsunreg((__hdle)&fat_fs_type);

    fat_cache_destroy();
    fat_destroy_inodecache();

    return EPDK_OK;
}

