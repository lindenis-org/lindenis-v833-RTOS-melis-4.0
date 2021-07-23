

#include "fs.h"
#include "file.h"
#include "namei.h"
#include "err.h"
#include "fstime.h"
#include "fsys_debug.h"

extern int nr_files;
extern struct file *do_sys_open(const char *filename, int flags, int mode, struct super_block *sb);
extern __s32 filp_close(struct file *pfile);

typedef struct
{
    char  *s_mode;
    __u32   mode;
} __vfs_mode_t;

static const __vfs_mode_t _vfs_valid_modes[] =
{
    { "r"   ,  O_RDONLY                             },
    { "w"   ,  O_WRONLY |   O_CREAT |   O_TRUNC     },
    { "a"   ,  O_WRONLY |   O_CREAT |   O_APPEND    },
    { "rb"  ,  O_RDONLY                             },
    { "wb"  ,  O_WRONLY |   O_CREAT |   O_TRUNC     },
    { "ab"  ,  O_WRONLY |   O_CREAT |   O_APPEND    },
    { "r+"  ,  O_RDWR                               },
    { "w+"  ,  O_RDWR   |   O_CREAT |   O_TRUNC     },
    { "a+"  ,  O_RDWR   |   O_CREAT |   O_APPEND    },
    { "r+b" ,  O_RDWR                               },
    { "rb+" ,  O_RDWR                               },
    { "w+b" ,  O_RDWR   |   O_CREAT |   O_TRUNC     },
    { "wb+" ,  O_RDWR   |   O_CREAT |   O_TRUNC     },
    { "a+b" ,  O_RDWR   |   O_CREAT |   O_APPEND    },
    { "ab+" ,  O_RDWR   |   O_CREAT |   O_APPEND    },
};
#define VFS_VALID_MODE_NUM     (sizeof(_vfs_valid_modes) / sizeof(__vfs_mode_t))

int epdk_mode_trans(const char *p_mode, __u32 *mode)
{
    int j;

    for (j = 0; j < VFS_VALID_MODE_NUM; j++)
    {
        if (!strcmp(p_mode, _vfs_valid_modes[j].s_mode))
        {
            *mode = _vfs_valid_modes[j].mode;
            return OK;
        }
    }
    return FAIL;
}


__hdle esFSYS_fopen(const char *pFileName, const char *pMode)
{
    struct super_block  *sb;
    struct file *retval;
    char *realname;
    int cpu_sr, flags, error = 0, temp_open_nr;
    __u8   err;

    fs_log_trace0("fopn:%s,", pFileName);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("open file err when pend sem, err:%d\n", err);
        return NULL;
    }

    /* 打开的文件个数是否已经达到上限 */
    ENTER_CRITICAL(cpu_sr);
    temp_open_nr = ++nr_files;
    EXIT_CRITICAL(cpu_sr);
    if (temp_open_nr >= NR_MAXOPEN)
    {
        fs_log_warning("max file or dir handle reached!\n");
        error = -EMFILE;
        goto dec_out;
    }

    /*  分析打开模式 */
    if (epdk_mode_trans(pMode, &flags) == FAIL)
    {
        fs_log_warning("mode not supported!\n");
        error = -EBADRQC;
        goto dec_out;
    }

    /* 获取由pFileName路径名中的盘符指定的分区指针，并析取盘符以外的路径名 */
    sb = path_to_sb(pFileName, &realname);
    if (!sb)
    {
        fs_log_warning("part \"%c\" not accessable!\n", *pFileName);
        error = -ENOENT;
        goto dec_out;
    }

    /* 虚拟文件系统打开操作内部流程 */
    retval = do_sys_open(realname, flags, 0, sb);
    if (IS_ERR(PTR_ERR(retval)))
    {
        error = PTR_ERR(retval);
        goto dec_out;
    }
    unmount_check(sb);
    if (retval)
        fs_log_trace0("fd:%d,", retval->f_fd);

    /* 成功打开文件 */
    goto out;

dec_out:
    nr_files--;
out:
    if (error)
    {
        fs_log_warning("error(%d) meet in fopen: %s\n", error, pFileName);
        if (error != EPDK_FAIL)
            fs_err = error;
        retval = NULL;
    }
    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", error);
    return (__hdle)retval;
}

__s32 esFSYS_fclose(__hdle hFile)
{
    __u8 err;
    int retval;

    fs_log_trace0("fcls:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("close file err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    retval = filp_close((struct file *)hFile);
    unmount_check(NULL);

    /* 文件数减一 */
    nr_files--;

out:
    if (retval && retval != EPDK_FAIL)
    {
        fs_err = retval;
        retval = EPDK_FAIL;
    }

    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", retval);
    return retval;
}


__s32 esFSYS_fsync(__hdle hFile)
{
    struct file *file = (struct file *)hFile;
    struct dentry *dentry;
    struct inode *inode;
    int err;

    fs_log_trace0("fsync:%d,", file->f_fd);
    if (file->f_dentry)
        fs_log_objname("%s,", file->f_dentry->d_name.name);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("close file err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    /* 文件不时以写的方式打开 */
    if (!file->f_mode & FMODE_WRITE)
    {
        fs_log_warning("%s is not open by write mode.\n", file->f_dentry->d_name.name);
        err = -EACCES;
        goto out;
    }

    /* 是否是死的分区文件系统 */
    if (! file->f_dentry)
    {
        err = -EDEADLK;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    dentry = file->f_dentry;
    inode = dentry->d_inode;

    err = -EINVAL;
    if (!file->f_op || !file->f_op->fsync)
        goto out;

    /* We need to protect against concurrent writers.. */
    err = file->f_op->fsync(file, dentry, 0);

out:
    if (err && err != EPDK_FAIL)
    {
        file->f_err = err;
        err = EPDK_FAIL;
    }

    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", err);
    return err;
}

__s32 esFSYS_remove(const char *pFileName)
{
    struct super_block *sb;
    char *realname;
    int error = 0;
    __u8   err;

    fs_log_trace0("rmf:%s,", pFileName);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("rmfile err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    /* 获取由pFileName路径名中的盘符指定的分区指针，并析取盘符以外的路径名 */
    sb = path_to_sb(pFileName, &realname);
    if (!sb)
    {
        fs_log_error("part \"%c\" not accessable!\n", *pFileName);
        error = -ENOENT;
        goto out;
    }

    error = do_unlink(realname, sb);
    unmount_check(sb);

out:
    if (error && error != EPDK_FAIL)
    {
        fs_log_warning("rmfile return value:%d\n", error);
        fs_err = error;
        error = EPDK_FAIL;
    }
    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", error);

    return error;
}


int esFSYS_fstat(__hdle hFile, void *stat_buf)
{
    __u8  err;
    struct kstat *fstat = (struct kstat *)(stat_buf);
    struct inode *inode = ((struct file *)hFile)->f_dentry->d_inode;

    fs_log_trace0("fstat\n");

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("fstat err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }
    fstat->mode = inode->i_mode;
    fstat->nlink = inode->i_nlink;
    fstat->size = inode->i_size;
    fs_timespec_to_kstamp(&(fstat->atime), inode->i_atime);
    fs_timespec_to_kstamp(&(fstat->mtime), inode->i_mtime);
    fs_timespec_to_kstamp(&(fstat->ctime), inode->i_ctime);
    fstat->blksize = 1 << inode->i_blkbits;
    fstat->blocks = inode->i_blocks;
    fstat->pos = ((struct file *)hFile)->f_pos;

    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O\n");
    return EPDK_OK;
}

__u32 esFSYS_fread(void *pData, __u32 Size, __u32 N, __hdle hFile)
{
    int   total;
    __u32 ret = 0;
    __u8  err;

    fs_log_trace0("fr:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("fread err when pend sem, err:%d\n", err);
        return 0;
    }

    debug_timerclr(alltime);
    debug_timerstart(oneaccesstime);

    /* 是否是死文件 */
    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err = -EDEADLK;
        ret = 0;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    /* 文件不时以读的方式打开 */
    if (!(((struct file *)hFile)->f_mode & FMODE_READ))
    {
        fs_log_warning("%s is not open by read mode.\n", ((struct file *)hFile)->f_dentry->d_name.name);
        ((struct file *)hFile)->f_err = -EACCES;
        ret = 0;
        goto out;
    }

    /* 文件句柄对应的节点无fread操作 */
    if (!((struct file *)hFile)->f_op || !((struct file *)hFile)->f_op->read)
    {
        fs_log_error("%s has no fread method.\n", ((struct file *)hFile)->f_dentry->d_name.name);
        ((struct file *)hFile)->f_err = -EPERM;
        ret = 0;
        goto out;
    }

    /* 非设备文件 */
    if (((struct file *)hFile)->f_dev == 0)
    {
        /* 判断是否为0，避免除0异常 */
        total = Size * N;
        if (!total)
        {
            ret = 0;
            fs_log_warning("0 read req. %s(fd:%d)\n",
                           ((struct file *)hFile)->f_dentry->d_name.name,
                           ((struct file *)hFile)->f_fd);
            goto out;
        }

        /* 读取total个字节的数据 */
        ret = ((struct file *)hFile)->f_op->read((struct file *)hFile, pData, total, &(((struct file *)hFile)->f_pos));
        fs_log_trace0("R%d,F%d,", ret, (int)(((struct file *)hFile)->f_pos) - ret);

        /* 处理返回信息 */
        if (ret == total)
            ret = N;
        else
        {
            ret = ret / Size;
            if (((struct file *)hFile)->f_pos == ((struct file *)hFile)->f_dentry->d_inode->i_size)
            {
                if (!ret)
                    fs_log_debug("EOF, %s(fd:%d).\n",
                                 ((struct file *)hFile)->f_dentry->d_name.name,
                                 ((struct file *)hFile)->f_fd);
                ((struct file *)hFile)->f_err = FSYS_ERR_EOF;
            }
        }
    }
    /* 设备文件 */
    else
    {
        if (!N)
        {
            ret = 0;
            fs_log_warning("0 read req. %s(fd:%d)\n",
                           ((struct file *)hFile)->f_dentry->d_name.name,
                           ((struct file *)hFile)->f_fd);
            goto out;
        }

        /* 读取total个字节的数据 */
        ret = ((struct file *)hFile)->f_op->read((struct file *)hFile, pData, N, &Size);
        fs_log_trace0("R%dS,F%d,", ret, Size);
    }

out:
    debug_timerend(oneaccesstime);
    debug_timershow(alltime);

    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O\n");
    return ret;
}

__u32 esFSYS_fwrite(const void *pData, __u32 Size, __u32 N, __hdle hFile)
{
    int     total;
    int     ret = 0;
    __u8    err;

    fs_log_trace0("fw:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("fwrite err when pend sem, err:%d\n", err);
        return 0;
    }

    /* 文件不时以写的方式打开 */
    if (!(((struct file *)hFile)->f_mode & FMODE_WRITE))
    {
        fs_log_warning("%s is not open by write mode.\n", ((struct file *)hFile)->f_dentry->d_name.name);
        ((struct file *)hFile)->f_err = -EACCES;
        ret = 0;
        goto out;
    }

    /* 是否是死的分区文件系统 */
    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err = -EDEADLK;
        ret = 0;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    /* 文件句柄对应的节点无fwrite操作 */
    if (!((struct file *)hFile)->f_op || !((struct file *)hFile)->f_op->write)
    {
        fs_log_error("%s has no fwrite method.\n", ((struct file *)hFile)->f_dentry->d_name.name);
        ((struct file *)hFile)->f_err = -EPERM;
        ret = 0;
        goto out;
    }

    /* 非设备文件 */
    if (((struct file *)hFile)->f_dev == 0)
    {
        /* 判断是否为0，避免除0异常 */
        total = Size * N;
        if (!total)
        {
            ret = 0;
            fs_log_warning("0 write req. %s(fd:%d)\n",
                           ((struct file *)hFile)->f_dentry->d_name.name,
                           ((struct file *)hFile)->f_fd);
            goto out;
        }

        /* 写入total字节个数据 */
        ret = ((struct file *)hFile)->f_op->write(((struct file *)hFile), pData, total, &(((struct file *)hFile)->f_pos));
        fs_log_trace0("W%d,T%d,", ret, (int)((struct file *)hFile)->f_pos - ret);

        /* 处理返回信息 */
        if (ret == total)
            ret = N;
        else
        {
            fs_log_warning("write bytes less than you want.\n");
            ret = ret / Size;
            ((struct file *)hFile)->f_err = fs_err;
        }
    }
    else
    {
        if (!N)
        {
            ret = 0;
            fs_log_warning("0 write req. %s(fd:%d)\n",
                           ((struct file *)hFile)->f_dentry->d_name.name,
                           ((struct file *)hFile)->f_fd);
            goto out;
        }

        /* 读取total个字节的数据 */
        ret = ((struct file *)hFile)->f_op->write((struct file *)hFile, pData, N, &Size);
        fs_log_trace0("W%dS,T%d,", ret, Size);
    }

out:
    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O\n");
    return ret;
}


int generic_file_llseek(struct file *file, __s64 offset, int origin)
{
    long long retval;
    struct inode *inode = file->f_dentry->d_inode;

    switch (origin)
    {
        case ES_SEEK_END:
            offset += inode->i_size;
            break;
        case ES_SEEK_CUR:
            offset += file->f_pos;
    }
    retval = -EINVAL;
    if (offset >= 0 && offset <= inode->i_size)
    {
        if (offset != file->f_pos)
        {
            file->f_pos = offset;
        }
        retval = 0;
    }

    if (retval)
        file->f_err = retval;

    return retval;
}

int no_llseek(struct file *file, __s64 offset, int origin)
{
    return -ESPIPE;
}

__s32 esFSYS_fseekex(__hdle hFile, __s32 l_off, __s32 h_off, __s32 Whence)
{
    __u8 err;
    int res;
    __s64   Offset = (0x00000000FFFFFFFF & ((__s64)l_off)) | (((__s64)h_off) << 32);
    int (*fn)(struct file *, __s64, int);

    fs_log_trace0("fllsk:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("fseekex err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err = -EDEADLK;
        ((struct file *)hFile)->f_err = fs_err;
        res = 0;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    fn = no_llseek;
    if (((struct file *)hFile)->f_op && ((struct file *)hFile)->f_op->llseek)
        fn = ((struct file *)hFile)->f_op->llseek;
    res = fn((struct file *)hFile, Offset, Whence);

out:
    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O%d\n", (int)(((struct file *)hFile)->f_pos));

    return res;
}

__s32 esFSYS_ftellex(__hdle hFile, __s32 *l_pos, __s32 *h_pos)
{
    int   res;
    __s64 x;
    __u8  err;

    fs_log_trace0("flltl:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("ftellex err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err = -EDEADLK;
        ((struct file *)hFile)->f_err = fs_err;
        res = EPDK_FAIL;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    x = ((struct file *)hFile)->f_pos;
    *l_pos = x;
    *h_pos = x >> 32;
    res = EPDK_OK;

out:
    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O%d\n", (int)(((struct file *)hFile)->f_pos));

    return res;
}

int esFSYS_fseek(__hdle hFile, __s32 Offset, __s32 Whence)
{
    __u8 err;
    int res;
    int (*fn)(struct file *, __s64, int);

    fs_log_trace0("fsk:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("fseek err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err = -EDEADLK;
        ((struct file *)hFile)->f_err = fs_err;
        res = EPDK_FAIL;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    fn = no_llseek;
    if (((struct file *)hFile)->f_op && ((struct file *)hFile)->f_op->llseek)
        fn = ((struct file *)hFile)->f_op->llseek;
    res = fn((struct file *)hFile, Offset, Whence);

out:
    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O%d\n", (int)(((struct file *)hFile)->f_pos));

    return res;
}

int esFSYS_ftell(__hdle hFile)
{
    __u8  err;
    __s64 res;

    fs_log_trace0("ftel:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("ftell err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err = -EDEADLK;
        ((struct file *)hFile)->f_err = fs_err;
        res = EPDK_FAIL;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    res = ((struct file *)hFile)->f_pos;

out:
    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O%d\n", (int)(((struct file *)hFile)->f_pos));

    return res;
}

void generic_fillattr(struct inode *inode, struct kstat *stat)
{
    stat->mode = inode->i_mode;
    stat->nlink = inode->i_nlink;
    fs_timespec_to_kstamp(&(stat->atime), inode->i_atime);
    fs_timespec_to_kstamp(&(stat->mtime), inode->i_mtime);
    fs_timespec_to_kstamp(&(stat->ctime), inode->i_ctime);
    stat->size = inode->i_size;
    stat->blocks = inode->i_blocks;
    stat->blksize = (1 << inode->i_blkbits);
}

__s32 esFSYS_fioctrl(__hdle hFile, __s32 Cmd, __s32 Aux, void *pBuffer)
{
    struct inode *ino = NULL;
    int    ret;
    __u8   err;

    /* NOTE ! you can't print any information in "fioctrl",
     * because UART dirver will call "fioctrl"
     * by sunny, 2009.8.19 */
    //fs_log_trace0("fctl:%d,%d,%d,",((struct file *)hFile)->f_fd, Cmd, Aux);
    //if(((struct file *)hFile)->f_dentry)
    //    fs_log_objname("%s,",((struct file *)hFile)->f_dentry->d_name.name);

    if (((struct file *)hFile)->f_dentry && ((struct file *)hFile)->f_dentry->d_inode)
        ino = ((struct file *)hFile)->f_dentry->d_inode;
    else
    {
        //    fs_log_warning("file is not accessable.\n");
        fs_err = -EBADFD;
        ((struct file *)hFile)->f_err = fs_err;
        ret = EPDK_FAIL;
        goto out;
    }
    if (((struct file *)hFile)->f_op && ((struct file *)hFile)->f_op->ioctl)
    {
        ret = ((struct file *)hFile)->f_op->ioctl(ino,
                (struct file *)hFile, Cmd,   Aux, pBuffer);
    }
    else
    {
        fs_err = -EPERM;
        ret = EPDK_FAIL;
    }

out:

    //fs_log_trace0("O%d", ret);
    return ret;
}

__s32 esFSYS_ferror(__hdle hFile)
{
    int error;
    if (!hFile)
    {
        return EPDK_FAIL;
    }
    if (((struct file *)hFile)->f_err > 0)
    {
        error = -(((struct file *)hFile)->f_err);
    }
    else
    {
        error = ((struct file *)hFile)->f_err;
    }
    return error;
}

void esFSYS_ferrclr(__hdle hFile)
{
}

__s32 esFSYS_rename(const char *newfilename, const char *oldfilename)
{
    return EPDK_OK;
}

