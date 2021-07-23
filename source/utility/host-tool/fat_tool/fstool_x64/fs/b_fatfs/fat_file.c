/*
 *  linux/fs/fat/file.c
 *
 *  Written 1992,1993 by Werner Almesberger
 *
 *  regular file handling primitives for fat-based filesystems
 */


#include "fatfs.h"
#include "buffer_head.h"
#include "err.h"
#include "fsys_debug.h"

extern int __debug_dump_fatchunk(struct inode *inode);

int fat_generic_ioctl(struct inode *inode, struct file *filp,
                      unsigned int cmd, unsigned int aux, void *buffer)
{
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);

    return 0;
}

static int fat_file_release(struct inode *inode, struct file *filp)
{
    if ((filp->f_mode & FMODE_WRITE) &&
            MSDOS_SB(inode->i_sb)->options.flush)
    {
        fat_flush_inodes(inode->i_sb, inode, NULL);
//      congestion_wait(WRITE, HZ/10);
    }
    return 0;
}
static int fat_file_read(struct file *filp, char *buf, unsigned int len, __s64 *ppos)
{
    int phy, err = 0;
    int count, chunk_size;
    __s64 pos;
    int offset, max_blks;
    int sb_blksize, sb_blksize_bits;
    struct buffer_head *bh;
    struct super_block *sb;
    struct inode *ino;

    sb = filp->f_dentry->d_sb;
    ino = filp->f_dentry->d_inode;

    sb_blksize = sb->s_blocksize;
    sb_blksize_bits = sb->s_blocksize_bits;

    offset = *ppos % sb_blksize;
    pos = *ppos;

    if (*ppos >= ino->i_size) goto out;

    len = *ppos + len < ino->i_size ? len : ino->i_size - *ppos;

    if (offset)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0);
        if (err || !phy)
            goto out;

        bh = __bread(sb, phy, sb_blksize);

        chunk_size = len <= sb_blksize - offset ? len : sb_blksize - offset;

        debug_timerstart(memcpytime);
        memcpy(buf, bh->b_data + offset, chunk_size);
        debug_timerend(memcpytime);
        buf += chunk_size;
        brelse(bh);

        len -= chunk_size;
        pos += chunk_size;
    }

//    if(((__u32)buf >= DRAM_BASE) && ((__u32)buf <= (DRAM_BASE + DRAM_SIZE)))
    if (1)  //在PC端不需要做这些判断，可直接向分区去写，可加快文件写速度
    {
        struct buffer_head *bh;
        int i, offset;
        int dio_startphy, dio_blks;
        char *dio_startbuf;


        while (max_blks = len >> sb_blksize_bits)
        {
            err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0);
            if (err || !phy)
                goto out;

            dio_startphy = phy;
            dio_startbuf = buf;
            dio_blks = 0;
            for (offset = i = 0; i < max_blks; offset += sb_blksize, i++)
            {
                debug_timerstart(hashtime);
                bh = get_hash_table(sb, phy++, sb_blksize);
                debug_timerend(hashtime);
                if (bh)
                {
                    if (dio_blks)
                    {
                        debug_timerstart(diodevicetime);
                        esFSYS_pread(dio_startbuf, dio_startphy, dio_blks, sb->s_part);
                        debug_timerend(diodevicetime);
                        dio_blks = 0;
                    }

                    memcpy(buf + offset, bh->b_data, sb_blksize);
                    put_bh(bh);

                    dio_startphy = phy;
                    dio_startbuf = buf + offset + sb_blksize;
                }
                else
                {
                    dio_blks++;
                }
            }

            if (dio_blks)
            {
                debug_timerstart(diodevicetime);
                esFSYS_pread(dio_startbuf, dio_startphy, dio_blks, sb->s_part);
                debug_timerend(diodevicetime);
            }
//            debug_timerstart(diodevicetime);
//            (*part_ops).read(buf, phy, max_blks, part);
//            debug_timerend(diodevicetime);
            chunk_size = max_blks << sb_blksize_bits;
            buf += chunk_size;
            len -= chunk_size;
            pos += chunk_size;
        }
    }
    else
    {
        while (max_blks = len >> sb_blksize_bits)
        {
            err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0);
            if (err || !phy)
                goto out;

            chunk_size = sb_blksize * max_blks;
            while (max_blks--)
            {
                bh = __bread(sb, phy++, sb_blksize);
                debug_timerstart(memcpytime);
                memcpy(buf, bh->b_data, sb_blksize);
                debug_timerend(memcpytime);
                buf += sb_blksize;
                brelse(bh);
            }
            len -= chunk_size;
            pos += chunk_size;
        }
    }

    if (len)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0);
        if (err || !phy)
            goto out;

        bh = __bread(sb, phy, sb_blksize);

        debug_timerstart(memcpytime);
        memcpy(buf,  bh->b_data, len);
        debug_timerend(memcpytime);
        brelse(bh);

        pos += len;
        buf += len;
    }

out:
    chunk_size = pos - *ppos ;
    debug_dump_mems(buf - chunk_size, (char *)(ino->debug_dentry->d_name.name), *ppos, chunk_size);
    *ppos = pos;
    fs_err = err;
    return chunk_size;
}

static int fat_file_write(struct file *filp, const char *buf, unsigned int len, __s64 *ppos)
{
    int phy, err = 0;
    int res, count, chunk_size;
    __s64 pos;
    int offset, max_blks;
    int sb_blksize, sb_blksize_bits;
    struct buffer_head *bh;
    struct super_block *sb;
    struct inode *ino;
    //debug
    struct msdos_inode_info *ms_inode = MSDOS_I(filp->f_dentry->d_inode);
    struct msdos_sb_info *ms_sbi = MSDOS_SB(filp->f_dentry->d_sb);

    sb = filp->f_dentry->d_sb;
    ino = filp->f_dentry->d_inode;

    sb_blksize = sb->s_blocksize;
    sb_blksize_bits = sb->s_blocksize_bits;

    offset = *ppos % sb_blksize;
    pos = *ppos;

    if (pos > ino->i_size) goto out;

    debug_dump_mems((char *)(buf), (char *)(ino->debug_dentry->d_name.name), *ppos, len);

    if (offset)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
        if (err || !phy)
            goto out;

        bh = __bread(sb, phy, sb_blksize);
        chunk_size = (len <= sb_blksize - offset ? len : sb_blksize - offset);

        debug_timerstart(memcpytime);
        memcpy(bh->b_data + offset, buf, chunk_size);
        debug_timerend(memcpytime);
        buf += chunk_size;
        mark_buffer_dirty(bh);
        brelse(bh);

        len -= chunk_size;
        pos += chunk_size;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
            if (ino->i_size > ms_inode->mmu_private)
                ms_inode->mmu_private = ino->i_size;
        }
    }

    if (((__u32)buf >= DRAM_BASE) && ((__u32)buf <= (DRAM_BASE + DRAM_SIZE)))
    {
        struct buffer_head *bh;
        int i, buf_off;

        while (max_blks = len >> sb_blksize_bits)
        {
            err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
            if (err || !phy)
                goto out;

            debug_timerstart(diodevicetime);
            esFSYS_pwrite(buf, phy, max_blks, sb->s_part);
            debug_timerend(diodevicetime);

            debug_timerstart(hashtime);
            for (buf_off = i = 0; i < max_blks; buf_off += sb_blksize, i++)
            {
                bh = get_hash_table(sb, phy++, sb_blksize);
                if (bh)
                {
                    memcpy(bh->b_data, buf + buf_off, sb_blksize);
                    mark_buffer_clean(bh);
                    put_bh(bh);
                }
            }
            debug_timerend(hashtime);

            chunk_size = sb_blksize * max_blks;
            buf += chunk_size;
            len -= chunk_size;
            pos += chunk_size;
            if (pos > ino->i_size)
                ino->i_size = pos;
        }
    }
    else
    {
        while (max_blks = len >> sb_blksize_bits)
        {
            err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
            if (err || !phy)
                goto out;

            chunk_size = sb_blksize * max_blks;
            while (max_blks--)
            {
                bh = __getblk(sb, phy++, sb_blksize);
                debug_timerstart(memcpytime);
                memcpy(bh->b_data, buf, sb_blksize);
                debug_timerend(memcpytime);
                buf += sb_blksize;
                mark_buffer_dirty(bh);
                fat_sync_bhs(&bh, 1);
                brelse(bh);
            }

            len -= chunk_size;
            pos += chunk_size;
            if (pos > ino->i_size)
                ino->i_size = pos;;
        }
    }

    if (len)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
        if (err || !phy)
            goto out;

        bh = __bread(sb, phy, sb_blksize);
        debug_timerstart(memcpytime);
        memcpy(bh->b_data, buf, len);
        debug_timerend(memcpytime);
        mark_buffer_dirty(bh);
        brelse(bh);

        pos += len;
        buf += len;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
            if (ino->i_size > ms_inode->mmu_private)
                ms_inode->mmu_private = ino->i_size;
        }
    }

out:
    chunk_size = pos - *ppos ;
    *ppos = pos;
    fs_err = err;
    return chunk_size;
}

const struct file_operations fat_file_operations =
{
    .llseek     = generic_file_llseek,
    .read       = fat_file_read,
    .write      = fat_file_write,
    .release    = fat_file_release,
    .ioctl      = fat_generic_ioctl,
    .fsync      = file_fsync,
};

/* Free all clusters after the skip'th cluster. */
static int fat_free(struct inode *inode, int skip)
{
    struct super_block *sb = inode->i_sb;
    int err, wait, free_start, i_start, i_logstart;

    if (MSDOS_I(inode)->i_start == 0)
        return 0;

    fat_cache_inval_inode(inode);

    wait = IS_DIRSYNC(inode);
    i_start = free_start = MSDOS_I(inode)->i_start;
    i_logstart = MSDOS_I(inode)->i_logstart;

    /* First, we write the new file size. */
    if (!skip)
    {
        MSDOS_I(inode)->i_start = 0;
        MSDOS_I(inode)->i_logstart = 0;
    }
    MSDOS_I(inode)->i_attrs |= ATTR_ARCH;
    inode->i_ctime = inode->i_mtime = CURRENT_TIME_SEC;
    if (wait)
    {
        err = fat_sync_inode(inode);
        if (err)
        {
            MSDOS_I(inode)->i_start = i_start;
            MSDOS_I(inode)->i_logstart = i_logstart;
            return err;
        }
    }
    else
        mark_inode_dirty(inode);

    /* Write a new EOF, and get the remaining cluster chain for freeing. */
    if (skip)
    {
        struct fat_entry fatent;
        int ret, fclus, dclus;

        ret = fat_get_cluster(inode, skip - 1, &fclus, &dclus, NULL);
        if (ret < 0)
            return ret;
        else if (ret == FAT_ENT_EOF)
            return 0;

        fatent_init(&fatent);
        ret = fat_ent_read(inode, &fatent, dclus);
        if (ret == FAT_ENT_EOF)
        {
            fatent_brelse(&fatent);
            return 0;
        }
        else if (ret == FAT_ENT_FREE)
        {
            fat_fs_panic(sb,
                         "%s: invalid cluster chain (i_pos %d)",
                         __FUNCTION__, (int)MSDOS_I(inode)->i_pos);
            ret = -EIO;
        }
        else if (ret > 0)
        {
            err = fat_ent_write(inode, &fatent, FAT_ENT_EOF, wait);
            if (err)
                ret = err;
        }
        fatent_brelse(&fatent);
        if (ret < 0)
            return ret;

        free_start = ret;
    }
    inode->i_blocks = skip << (MSDOS_SB(sb)->cluster_bits - 9);

    /* Freeing the remained cluster chain */
    return fat_free_clusters(inode, free_start);
}

void fat_truncate(struct inode *inode)
{
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
    const unsigned int cluster_size = sbi->cluster_size;
    int nr_clusters;

    /*
     * This protects against truncating a file bigger than it was then
     * trying to write into the hole.
     */
    if (MSDOS_I(inode)->mmu_private > inode->i_size)
        MSDOS_I(inode)->mmu_private = inode->i_size;

    nr_clusters = (inode->i_size + (cluster_size - 1)) >> sbi->cluster_bits;

    fat_free(inode, nr_clusters);
    fat_flush_inodes(inode->i_sb, inode, NULL);
}

int fat_getattr(struct dentry *dentry, struct kstat *stat)
{
    struct inode *inode = dentry->d_inode;
    generic_fillattr(inode, stat);
    stat->blksize = MSDOS_SB(inode->i_sb)->cluster_size;
    return 0;
}
const struct inode_operations fat_file_inode_operations =
{
    .truncate   = fat_truncate,
    .getattr    = fat_getattr,
};
