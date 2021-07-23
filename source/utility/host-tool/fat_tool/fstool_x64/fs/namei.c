/*
 *  linux/fs/namei.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * Some corrections by tytso.
 */

/* [Feb 1997 T. Schoebel-Theuer] Complete rewrite of the pathname
 * lookup logic.
 */
/* [Feb-Apr 2000, AV] Rewrite to the new namespace architecture.
 */



#include "limits.h"
#include "fs.h"
#include "namei.h"
#include "file.h"
#include "err.h"
#include "slab.h"
#include "nls.h"

#include "fsys_debug.h"

int nr_files = 0;
struct file *files[NR_MAXOPEN] = {0};

extern kmem_cache_t *names_cachep;
extern kmem_cache_t *filp_cachep;

/* [Feb-1997 T. Schoebel-Theuer]
 * Fundamental changes in the pathname lookup mechanisms (namei)
 * were necessary because of omirr.  The reason is that omirr needs
 * to know the _real_ pathname, not the user-supplied one, in case
 * of symlinks (and also when transname replacements occur).
 *
 * The new code replaces the old recursive symlink resolution with
 * an iterative one (in case of non-nested symlink chains).  It does
 * this with calls to <fs>_follow_link().
 * As a side effect, dir_namei(), _namei() and follow_link() are now
 * replaced with a single function lookup_dentry() that can handle all
 * the special cases of the former code.
 *
 * With the new dcache, the pathname is stored at each inode, at least as
 * long as the refcount of the inode is positive.  As a side effect, the
 * size of the dcache depends on the inode cache and thus is dynamic.
 *
 * [29-Apr-1998 C. Scott Ananian] Updated above description of symlink
 * resolution to correspond with current state of the code.
 *
 * Note that the symlink resolution is not *completely* iterative.
 * There is still a significant amount of tail- and mid- recursion in
 * the algorithm.  Also, note that <fs>_readlink() is not used in
 * lookup_dentry(): lookup_dentry() on the result of <fs>_readlink()
 * may return different results than <fs>_follow_link().  Many virtual
 * filesystems (including /proc) exhibit this behavior.
 */

/* [24-Feb-97 T. Schoebel-Theuer] Side effects caused by new implementation:
 * New symlink semantics: when open() is called with flags O_CREAT | O_EXCL
 * and the name already exists in form of a symlink, try to create the new
 * name indicated by the symlink. The old code always complained that the
 * name already exists, due to not following the symlink even if its target
 * is nonexistent.  The new semantics affects also mknod() and link() when
 * the name is a symlink pointing to a non-existant name.
 *
 * I don't know which semantics is the right one, since I have no access
 * to standards. But I found by trial that HP-UX 9.0 has the full "new"
 * semantics implemented, while SunOS 4.1.1 and Solaris (SunOS 5.4) have the
 * "old" one. Personally, I think the new semantics is much more logical.
 * Note that "ln old new" where "new" is a symlink pointing to a non-existing
 * file does succeed in both HP-UX and SunOs, but not in Solaris
 * and in the old Linux semantics.
 */

/* [16-Dec-97 Kevin Buhr] For security reasons, we change some symlink
 * semantics.  See the comments in "open_namei" and "do_link" below.
 *
 * [10-Sep-98 Alan Modra] Another symlink change.
 */

/* [Feb-Apr 2000 AV] Complete rewrite. Rules for symlinks:
 *  inside the path - always follow.
 *  in the last component in creation/removal/renaming - never follow.
 *  if LOOKUP_FOLLOW passed - follow.
 *  if the pathname has trailing slashes - follow.
 *  otherwise - don't follow.
 * (applied in that order).
 *
 * [Jun 2000 AV] Inconsistent behaviour of open() in case if flags==O_CREAT
 * restored for 2.4. This is the last surviving part of old 4.2BSD bug.
 * During the 2.4 we need to fix the userland stuff depending on it -
 * hopefully we will be able to get rid of that wart in 2.5. So far only
 * XEmacs seems to be relying on it...
 */
/*
 * [Sep 2001 AV] Single-semaphore locking scheme (kudos to David Holland)
 * implemented.  Let's see if raised priority of ->s_vfs_rename_mutex gives
 * any extra contention...
 */

/* In order to reduce some races, while at the same time doing additional
 * checking and hopefully speeding things up, we copy filenames to the
 * kernel data space before using them..
 *
 * POSIX.1 2.4: an empty pathname is invalid (ENOENT).
 * PATH_MAX includes the nul terminator --RR.
 */
static int do_getname(const char *filename, char *page)
{
    unsigned long len = strlen(filename);

    if (len < PATH_MAX)
    {
        strncpy(page, filename, len + 1);
        return 0;
    }
    else return -ENAMETOOLONG;
}

char *getname(const char *filename)
{
    char *tmp, *result;

    result = ERR_PTR(-ENOMEM);
    tmp = __getname();
    if (tmp)
    {
        int retval = do_getname(filename, tmp);

        result = tmp;
        if (retval < 0)
        {
            __putname(tmp);
            result = ERR_PTR(retval);
        }
    }
    return result;
}

void putname(const char *name)
{
    __putname(name);
}

static  void follow_dotdot(struct nameidata *nd)
{
}

/**
 * release_open_intent - free up open intent resources
 * @nd: pointer to nameidata
 */
void release_open_intent(struct nameidata *nd)
{
    if (nd->intent.open.file->f_dentry == NULL)
        put_filp(nd->intent.open.file);
    else
        fput(nd->intent.open.file);
}

static   struct dentry *
do_revalidate(struct dentry *dentry, struct nameidata *nd)
{
    int status = dentry->d_op->d_revalidate(dentry, nd);
    if (status <= 0)
    {
        /*
         * The dentry failed validation.
         * If d_revalidate returned 0 attempt to invalidate
         * the dentry otherwise d_revalidate is asking us
         * to return a fail status.
         */
        if (!status)
        {
            if (!d_invalidate(dentry))
            {
                dput(dentry);
                dentry = NULL;
            }
        }
        else
        {
            dput(dentry);
            dentry = ERR_PTR(status);
        }
    }
    return dentry;
}

/*------------------------------- path walk -----------------------------*/

/*
 * Internal lookup() using the new generic dcache.
 * SMP-safe
 */
static struct dentry *cached_lookup(struct dentry *parent, struct qstr *name, struct nameidata *nd)
{
    struct dentry *dentry = __d_lookup(parent, name);

    if (dentry && dentry->d_op && dentry->d_op->d_revalidate)
        dentry = do_revalidate(dentry, nd);

    return dentry;
}

/*
 * This is called when everything else fails, and we actually have
 * to go to the low-level filesystem to find out what we should do..
 *
 * We get the directory semaphore, and after getting that we also
 * make sure that nobody added the entry to the dcache in the meantime..
 * SMP-safe
 */
static struct dentry *real_lookup(struct dentry *parent, struct qstr *name, struct nameidata *nd)
{
    struct dentry *result;
    struct inode *dir = parent->d_inode;
    struct dentry *dentry = d_alloc(parent, name);

    result = ERR_PTR(-ENOMEM);
    if (dentry)
    {
        result = dir->i_op->lookup(dir, dentry, nd);
        if (result)
            dput(dentry);
        else
            result = dentry;
    }
    return result;
}

/*
 *  It's more convoluted than I'd like it to be, but... it's still fairly
 *  small and for now I'd prefer to have fast path as straight as possible.
 *  It _is_ time-critical.
 */
static int do_lookup(struct nameidata *nd, struct qstr *name,
                     struct dentry **target)
{
    struct dentry *dentry = __d_lookup(nd->dentry, name);

    if (!dentry)
        goto need_lookup;
    if (dentry->d_op && dentry->d_op->d_revalidate)
        goto need_revalidate;
done:
    *target = dentry;
    return 0;

need_lookup:
    dentry = real_lookup(nd->dentry, name, nd);
    if (IS_ERR(dentry))
        goto fail;
    goto done;

need_revalidate:
    dentry = do_revalidate(dentry, nd);
    if (!dentry)
        goto need_lookup;
    if (IS_ERR(dentry))
        goto fail;
    goto done;

fail:
    return PTR_ERR(dentry);
}

/*
 * convert charset to unicode
 * return
 */
int do_char2uni(const char *in, int in_len, wchar_t *uni)
{
    int len;
    len = nls->char2uni(in, in_len, uni);
    return len;
}

/*
 * Name resolution.
 * This is the basic name resolution function, turning a pathname into
 * the final dentry. We expect 'base' to be positive and a directory.
 *
 * Returns 0 and nd will have valid dentry and mnt on success.
 * Returns error and drops reference to input namei data on failure.
 */
static int __link_path_walk(const char *name, struct nameidata *nd)
{
    struct dentry *next;
    struct inode *inode;
    int err;
    unsigned int lookup_flags = nd->flags;
    int len, left;
    wchar_t uni;

    len = 0;
    left = strlen(name);

    /* skip all prefix '\\' */
    while (*name == '\\')
    {
        /* we must consider wide charset condition */
        len = do_char2uni(name, left, &uni);
        if (len <= 0)
        {
            err = -EINVAL;   /* convert encount error */
            goto return_err;
        }
        if (len != 1)        /* must be ASIIC charset */
            break;
        left--;
        name++;
    }
    if (!*name)
        goto return_reval;

    inode = nd->dentry->d_inode;

    /* At this point we know we have a real path component. */
    for (;;)
    {
        unsigned long hash;
        struct qstr this;
        unsigned int c;

        nd->flags |= LOOKUP_CONTINUE;

        this.name = name;
        c = *(const unsigned char *)name;

        hash = init_name_hash();
        do
        {
            len = do_char2uni(name, left, &uni);
            if (len <= 0)
            {
                err = -EINVAL;   /* convert encount error */
                goto return_out;
            }
            left -= len;
            while (c && (len--))
            {
                name++;
                hash = partial_name_hash(c, hash);
                c = *(const unsigned char *)name;
            }
        }
        while (c && (c != '\\'));
        this.len = name - (const char *) this.name;
        this.hash = end_name_hash(hash);

        /* remove trailing slashes? */
        if (!c)
            goto last_component;

        /* skip all postfix '\\' */
        while (*name == '\\')
        {
            /* we must consider wide charset condition */
            len = do_char2uni(name, left, &uni);
            if (len <= 0)
            {
                err = -EINVAL;   /* convert encount error */
                goto return_out;
            }
            if (len != 1)        /* must be ASIIC charset */
                break;
            left--;
            name++;
        }
        if (!*name)
            goto last_with_slashes;

        /*
         * "." and ".." are special - ".." especially so because it has
         * to be able to know about the current root directory and
         * parent relationships.
         */
        if (this.name[0] == '.') switch (this.len)
            {
                default:
                    break;
                case 2:
                    if (this.name[1] != '.')
                        break;
                    follow_dotdot(nd);
                    inode = nd->dentry->d_inode;
                    /* fallthrough */
                case 1:
                    continue;
            }
        /*
         * See if the low-level filesystem might want
         * to use its own hash..
         */
        if (nd->dentry->d_op && nd->dentry->d_op->d_hash)
        {
            err = nd->dentry->d_op->d_hash(nd->dentry, &this);
            if (err < 0)
                break;
        }
        /* This does the actual lookups.. */
        err = do_lookup(nd, &this, &next);
        if (err)
            break;

        err = -ENOENT;
        inode = next->d_inode;
        if (!inode)
            goto out_dput;
        err = -ENOTDIR;
        if (!inode->i_op)
            goto out_dput;

        dput(nd->dentry);
        nd->dentry = next;

        err = -ENOTDIR;
        if (!inode->i_op->lookup)
            break;
        continue;
        /* here ends the main loop */

last_with_slashes:
        lookup_flags |= LOOKUP_DIRECTORY;
last_component:
        /* Clear LOOKUP_CONTINUE iff it was previously unset */
        nd->flags &= lookup_flags | ~LOOKUP_CONTINUE;
        if (lookup_flags & LOOKUP_PARENT)
            goto lookup_parent;
        if (this.name[0] == '.') switch (this.len)
            {
                default:
                    break;
                case 2:
                    if (this.name[1] != '.')
                        break;
                    follow_dotdot(nd);
                    inode = nd->dentry->d_inode;
                    /* fallthrough */
                case 1:
                    goto return_reval;
            }
        if (nd->dentry->d_op && nd->dentry->d_op->d_hash)
        {
            err = nd->dentry->d_op->d_hash(nd->dentry, &this);
            if (err < 0)
                break;
        }
        err = do_lookup(nd, &this, &next);
        if (err)
            break;
        inode = next->d_inode;
        dput(nd->dentry);
        nd->dentry = next;
        err = -ENOENT;
        if (!inode)
            break;
        if (lookup_flags & LOOKUP_DIRECTORY)
        {
            err = -ENOTDIR;
            if (!inode->i_op || !inode->i_op->lookup)
                break;
        }
        goto return_base;
lookup_parent:
        nd->last = this;
        nd->last_type = LAST_NORM;
        if (this.name[0] != '.')
            goto return_base;
        if (this.len == 1)
            nd->last_type = LAST_DOT;
        else if (this.len == 2 && this.name[1] == '.')
            nd->last_type = LAST_DOTDOT;
        else
            goto return_base;
return_reval:
        /*
         * We bypassed the ordinary revalidation routines.
         * We may need to check the cached dentry for staleness.
         */
        if (nd->dentry && nd->dentry->d_sb &&
                (nd->dentry->d_sb->s_type->fs_flags & FS_REVAL_DOT))
        {
            err = -ESTALE;
            /* Note: we do not d_invalidate() */
            if (!nd->dentry->d_op->d_revalidate(nd->dentry, nd))
                break;
        }
return_base:
        return 0;
out_dput:
        dput(next);
        break;
    }
return_out:
    dput(nd->dentry);
return_err:
    return err;
}

static int __path_lookup_intent_open(const char *name,
                                     unsigned int lookup_flags, struct nameidata *nd,
                                     int open_flags, int create_mode)
{
    struct file *filp = get_empty_filp();
    int err;

    if (filp == NULL)
        return -ENFILE;

    nd->intent.open.file = filp;
    nd->intent.open.flags = open_flags;
    nd->intent.open.create_mode = create_mode;
    nd->last_type = LAST_ROOT; /* if there are only slashes... */
    nd->flags = lookup_flags | LOOKUP_OPEN;
    err = __link_path_walk(name, nd);

    if (IS_ERR(nd->intent.open.file))
    {
        if (err == 0)
        {
            err = PTR_ERR(nd->intent.open.file);
            dput(nd->dentry);
        }
    }
    else if (err != 0)
        release_open_intent(nd);
    return err;
}

static struct dentry *lookup_hash(struct nameidata *nd)
{
    struct dentry *dentry, *base;
    struct inode *inode;
    struct qstr *name;
    int err;

    base = nd->dentry;
    inode = base->d_inode;
    name = &nd->last;

    /*
     * See if the low-level filesystem might want
     * to use its own hash..
     */
    if (base->d_op && base->d_op->d_hash)
    {
        err = base->d_op->d_hash(base, name);
        dentry = ERR_PTR(err);
        if (err < 0)
            goto out;
    }

    dentry = cached_lookup(base, name, nd);
    if (!dentry)
    {
        struct dentry *new = d_alloc(base, name);
        dentry = ERR_PTR(-ENOMEM);
        if (!new)
            goto out;
        dentry = inode->i_op->lookup(inode, new, nd);
        if (!dentry)
            dentry = new;
        else
            dput(new);
    }
out:
    return dentry;
}

/*-------------------------- create(open) file ----------------------------*/

/*  Check whether we can create an object with dentry child in directory
 *  dir.
 *  1. We can't do it if child already exists (open has special treatment for
 *     this case, but since we are inlined it's OK)
 *  2. We can't do it if dir is read-only (done in permission())
 *  3. We should have write and exec permissions on dir
 *  4. We can't do it if dir is immutable (done in permission())
 */
static   int may_create(struct inode *dir, struct dentry *child,
                        struct nameidata *nd)
{
    if ((dir->i_sb->s_flags) & MS_RDONLY)
        return -EROFS;
    if (child->d_inode)
        return -EEXIST;
    if (IS_DEADDIR(dir))
        return -ENOENT;
    return 0;
}

int vfs_create(struct inode *dir, struct dentry *dentry, int mode,
               struct nameidata *nd)
{
    int error = may_create(dir, dentry, nd);

    if (error)
        return error;

    if (!dir->i_op || !dir->i_op->create)
        return -EACCES; /* shouldn't it be ENOSYS? */
    mode &= S_IALLUGO;
    mode |= S_IFREG;
    error = dir->i_op->create(dir, dentry, mode, nd);
    return error;
}

int may_open(struct nameidata *nd, int acc_mode, int flag)
{
    struct dentry *dentry = nd->dentry;
    struct inode *inode = dentry->d_inode;

    if (!inode)
        return -ENOENT;

    if (!(dentry->d_sb->s_flags & MS_ACTIVE))
        return -EBADSLT;

    if (S_ISDIR(inode->i_mode) && (flag & FMODE_WRITE))
        return -EISDIR;

    if (IS_RDONLY(inode) && (flag & FMODE_WRITE))
        return -EROFS;
    /*
     * An append-only file must be opened in append mode for writing.
     */
    if (IS_APPEND(inode))
    {
        if ((flag & FMODE_WRITE) && !(flag & O_APPEND))
            return -EPERM;
        if (flag & O_TRUNC)
            return -EPERM;
    }

    if ((flag & O_TRUNC) && inode->i_op->truncate)
    {
        invalidate_inode_buffers(inode);

        inode->i_size = 0;
        inode->i_op->truncate(inode);
    }

    return 0;
}

static int open_namei_create(struct nameidata *nd, struct dentry *target,
                             int flag, int mode)
{
    int error;
    struct dentry *dir = nd->dentry;

    error = vfs_create(dir->d_inode, target, mode, nd);
    dput(nd->dentry);
    nd->dentry = target;
    if (error)
        return error;
    /* Don't check for write permission, don't truncate */
    return may_open(nd, 0, flag & ~O_TRUNC);
}

/*
 *  open_namei()
 *
 * namei for open - this is in fact almost the whole open-routine.
 *
 * Note that the low bits of "flag" aren't the same as in the open
 * system call - they are 00 - no permissions needed
 *            01 - read permission needed
 *            10 - write permission needed
 *            11 - read/write permissions needed
 * which is a lot more logical, and also allows the "no perm" needed
 * for symlinks (where the permissions are checked later).
 * SMP-safe
 */
int open_namei(const char *pathname, int flag,
               int mode, struct nameidata *nd)
{
    int error;
    struct dentry *dir, *target;
    int count = 0;
    int lookup_flag = 0;

    if (flag & O_DIRECTORY)
        lookup_flag = LOOKUP_DIRECTORY;

    /*
     * The simplest case - just a plain lookup.
     */
    if (!(flag & O_CREAT))
    {
        error = __path_lookup_intent_open(pathname, lookup_flag, nd,
                                          flag, 0);
        if (error)
            return error;
        goto ok;
    }

    /*
     * Create - we need to know the parent.
     */
    error = __path_lookup_intent_open(pathname, LOOKUP_PARENT | LOOKUP_CREATE,
                                      nd, flag, mode);

    if (error)
        return error;

    /*
     * We have the parent and last component. First of all, check
     * that we are not asked to creat(2) an obvious directory - that
     * will not do.
     */
    error = -EISDIR;
    if (nd->last_type != LAST_NORM || nd->last.name[nd->last.len])
        goto exit;

    dir = nd->dentry;
    nd->flags &= ~LOOKUP_PARENT;
    target = lookup_hash(nd);

    error = PTR_ERR(target);
    if (IS_ERR(target))
    {
        goto exit;
    }

    if (IS_ERR(nd->intent.open.file))
    {
        error = PTR_ERR(nd->intent.open.file);
        goto exit_dput;
    }

    /* Negative dentry, just create the file */
    if (!target->d_inode)
    {
        error = open_namei_create(nd, target, flag, mode);
        if (error)
            goto exit;
        return 0;
    }

    /*
     * It already exists.
     */
    error = -EEXIST;
    if (flag & O_EXCL)
        goto exit_dput;

    dput(nd->dentry);
    nd->dentry = target;

    /*
     * Can not overwrite a directory.
     */
    error = -EISDIR;
    if (target->d_inode && S_ISDIR(target->d_inode->i_mode))
        goto exit;
ok:
    error = may_open(nd, 0, flag);
    if (error)
        goto exit;
    return 0;

exit_dput:
    dput(target);
exit:
    if (!IS_ERR(nd->intent.open.file))
        release_open_intent(nd);
    dput(nd->dentry);
    return error;
}

/*--------------------------- create dir --------------------------*/

int vfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
    int error = may_create(dir, dentry, NULL);
    if (error)
        return error;

    if (!dir->i_op || !dir->i_op->mkdir)
        return -EPERM;

    mode &= (S_IRWXUGO | S_ISVTX);
    error = dir->i_op->mkdir(dir, dentry, mode);

    return error;
}

int sys_mkdirat(const char *pathname, struct super_block *sb, int mode)
{
    int error = 0;
    char *tmp;
    struct dentry *dentry;
    struct nameidata nd;

    tmp = getname(pathname);
    error = PTR_ERR(tmp);
    if (IS_ERR(tmp))
        goto out_err;

    nd.dentry = dget(sb->s_root);
    nd.last_type = LAST_ROOT; /* if there are only slashes... */
    nd.flags = LOOKUP_PARENT;
    error = __link_path_walk(tmp, &nd);
    if (error)
        goto out;

    /*
     * you can't mkdir like '.' or '..'
     */
    error = -EEXIST;
    if (nd.last_type != LAST_NORM)
        goto out_unlock;

    nd.flags &= ~LOOKUP_PARENT;
    nd.flags |= LOOKUP_CREATE;
    nd.intent.open.flags = O_EXCL;
    dentry = lookup_hash(&nd);
    if (IS_ERR(dentry))
        goto out_unlock;

    error = vfs_mkdir(nd.dentry->d_inode, dentry, mode);
out_releasdentry:
    dput(dentry);
out_unlock:
    dput(nd.dentry);
out:
    putname(tmp);
out_err:
    return error;
}

/*----------------------------- remove dir -------------------------*/

/*
 * It's inline, so penalty for filesystems that don't use sticky bit is
 * minimal.
 */
static   int check_sticky(struct inode *dir, struct inode *inode)
{
    return 0;
}

/*
 *  Check whether we can remove a link victim from directory dir, check
 *  whether the type of victim is right.
 *  1. We can't do it if dir is read-only (done in permission())
 *  2. We should have write and exec permissions on dir
 *  3. We can't remove anything from append-only dir
 *  4. We can't do anything with immutable dir (done in permission())
 *  5. If the sticky bit on dir is set we should either
 *  a. be owner of dir, or
 *  b. be owner of victim, or
 *  c. have CAP_FOWNER capability
 *  6. If the victim is append-only or immutable we can't do antyhing with
 *     links pointing to it.
 *  7. If we were asked to remove a directory and victim isn't one - ENOTDIR.
 *  8. If we were asked to remove a non-directory and victim isn't one - EISDIR.
 *  9. We can't remove a root or mountpoint.
 * 10. We don't allow removal of NFS sillyrenamed files; it's handled by
 *     nfs_async_unlink().
 */
static int may_delete(struct inode *dir, struct dentry *victim, int isdir)
{
    int error;

    if (!victim->d_inode)
        return -ENOENT;

    BUG_ON(victim->d_parent->d_inode != dir);

    if ((dir->i_sb->s_flags) & MS_RDONLY)
        return -EROFS;
    if (atomic_read(&(victim->d_count)) > 1)
        return -EBUSY;
    if (IS_APPEND(dir))
        return -EPERM;
    if (check_sticky(dir, victim->d_inode) || IS_APPEND(victim->d_inode) ||
            IS_IMMUTABLE(victim->d_inode))
        return -EPERM;
    if (isdir)
    {
        if (!S_ISDIR(victim->d_inode->i_mode))
            return -ENOTDIR;
    }
    else if (S_ISDIR(victim->d_inode->i_mode))
        return -EISDIR;
    if (IS_DEADDIR(dir))
        return -ENOENT;
    if (victim->d_flags & DCACHE_NFSFS_RENAMED)
        return -EBUSY;
    return 0;
}


/*
 * We try to drop the dentry early: we should have
 * a usage count of 2 if we're the only user of this
 * dentry, and if that is true (possibly after pruning
 * the dcache), then we drop the dentry now.
 *
 * A low-level filesystem can, if it choses, legally
 * do a
 *
 *  if (!d_unhashed(dentry))
 *      return -EBUSY;
 *
 * if it cannot handle the case of removing a directory
 * that is still in use by something else..
 */
void dentry_unhash(struct dentry *dentry)
{
    dget(dentry);
    if (atomic_read(&dentry->d_count) == 2)
        __d_drop(dentry);
}

int vfs_rmdir(struct inode *dir, struct dentry *dentry)
{
    int error = may_delete(dir, dentry, 1);

    if (error)
        return error;

    if (!dir->i_op || !dir->i_op->rmdir)
        return -EPERM;

    dentry_unhash(dentry);

    error = dir->i_op->rmdir(dir, dentry);
    if (!error)
        dentry->d_inode->i_flags |= S_DEAD;

    if (!error)
    {
        d_delete(dentry);
    }
    dput(dentry);

    return error;
}

int do_rmdir(const char *pathname, struct super_block *sb)
{
    int error = 0;
    char *name;
    struct dentry *dentry;
    struct nameidata nd;

    name = getname(pathname);
    if (IS_ERR(name))
        return PTR_ERR(name);

    nd.dentry = dget(sb->s_root);
    nd.last_type = LAST_ROOT; /* if there are only slashes... */
    nd.flags = LOOKUP_PARENT;
    error = __link_path_walk(name, &nd);

    if (error)
        goto exit;

    switch (nd.last_type)
    {
        case LAST_DOTDOT:
            error = -ENOTEMPTY;
            goto exit1;
        case LAST_DOT:
            error = -EINVAL;
            goto exit1;
        case LAST_ROOT:
            error = -EBUSY;
            goto exit1;
    }
    dentry = lookup_hash(&nd);
    error = PTR_ERR(dentry);
    if (IS_ERR(dentry))
        goto exit2;
    error = vfs_rmdir(nd.dentry->d_inode, dentry);
    dput(dentry);
exit2:
exit1:
    dput(nd.dentry);
exit:
    putname(name);
    return error;
}

/*-----------------------------remove file -------------------------*/
int vfs_unlink(struct inode *dir, struct dentry *dentry)
{
    int error = may_delete(dir, dentry, 0);

    if (error)
        return error;

    if (!dir->i_op || !dir->i_op->unlink)
        return -EPERM;

    error = dir->i_op->unlink(dir, dentry);

    /* We don't d_delete() NFS sillyrenamed files--they still exist. */
    if (!error && !(dentry->d_flags & DCACHE_NFSFS_RENAMED))
    {
        d_delete(dentry);
    }

    return error;
}


/*
 * Make sure that the actual truncation of the file will occur outside its
 * directory's i_mutex.  Truncate can take a long time if there is a lot of
 * writeout happening, and we don't want to prevent access to the directory
 * while waiting on the I/O.
 */
int do_unlink(const char *pathname, struct super_block *sb)
{
    int error = 0;
    char *name;
    struct dentry *dentry;
    struct nameidata nd;
    struct inode *inode = NULL;

    name = getname(pathname);
    if (IS_ERR(name))
        return PTR_ERR(name);

    nd.dentry = dget(sb->s_root);
    nd.last_type = LAST_ROOT; /* if there are only slashes... */
    nd.flags = LOOKUP_PARENT;
    error = __link_path_walk(name, &nd);
    if (error)
        goto exit;

    error = -EISDIR;
    if (nd.last_type != LAST_NORM)
        goto exit1;
    dentry = lookup_hash(&nd);
    error = PTR_ERR(dentry);
    if (!IS_ERR(dentry))
    {
        /* Why not before? Because we want correct error value */
        if (nd.last.name[nd.last.len])
            goto slashes;
        inode = dentry->d_inode;
        if (inode)
            atomic_inc(&inode->i_count);
        error = vfs_unlink(nd.dentry->d_inode, dentry);
exit2:
        dput(dentry);
    }
    if (inode)
        iput(inode);    /* truncate the inode here */
exit1:
    dput(nd.dentry);
exit:
    fsync_dev(sb);
    putname(name);
    return error;

slashes:
    error = !dentry->d_inode ? -ENOENT :
            S_ISDIR(dentry->d_inode->i_mode) ? -EISDIR : -ENOTDIR;
    goto exit2;
}

static struct file *__dentry_open(struct dentry *dentry,
                                  int flags, struct file *f,
                                  int (*open)(struct inode *, struct file *))
{
    struct inode *inode;
    int error;
    int i;

    f->f_flags = flags;
    f->f_mode = ((flags + 1) & O_ACCMODE) | FMODE_LSEEK |
                FMODE_PREAD | FMODE_PWRITE;
    inode = dentry->d_inode;

    f->f_dentry = dentry;
    f->f_pos = flags & O_APPEND ? inode->i_size : 0;
    f->f_op = inode->i_fop;

    if (!open && f->f_op)
        open = f->f_op->open;
    if (open)
    {
        error = open(inode, f);
        if (error)
            goto cleanup_all;
    }

    f->f_flags &= ~(O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC);

    for (i = 0; i < NR_MAXOPEN; i++)
    {
        if (files[i] == 0)
        {
            files[i] = f;
            f->f_fd = i;
            break;
        }
    }
    if (i == NR_MAXOPEN)
    {
        error = -EMFILE;
        goto cleanup_all;
    }

    return f;

cleanup_all:
    f->f_dentry = NULL;
cleanup_file:
    put_filp(f);
    dput(dentry);
    return ERR_PTR(error);
}

/**
 * nameidata_to_filp - convert a nameidata to an open filp.
 * @nd: pointer to nameidata
 * @flags: open flags
 *
 * Note that this function destroys the original nameidata
 */
struct file *nameidata_to_filp(struct nameidata *nd, int flags)
{
    struct file *filp;

    /* Pick up the filp from the open intent */
    filp = nd->intent.open.file;
    /* Has the filesystem initialised the file for us? */
    if (filp->f_dentry == NULL)
        filp = __dentry_open(nd->dentry, flags, filp, NULL);
    else
        dput(nd->dentry);
    return filp;
}

struct file *do_sys_open(const char *filename, int flags, int mode, struct super_block *sb)
{
    struct nameidata nd;
    struct file *retval;
    char *tmp;
    int namei_flags;

    /* 在文件系统内部分配文件名空间并复制用户空间传来的文件名字符串 */
    tmp = getname(filename);
    if (IS_ERR(tmp))
        return ERR_PTR(tmp);

    namei_flags = flags;
    if ((namei_flags + 1) & O_ACCMODE)
        namei_flags++;

    /* 始终从根目录开始 */
    nd.dentry = dget(sb->s_root);

    /* 对每级目录进行打开遍历 */
    retval = (struct file *)open_namei(tmp, namei_flags, mode, &nd);
    if (!retval)  /* == EPDK_OK */
        retval = nameidata_to_filp(&nd, flags);

    /* 释放文件名空间 */
    putname(tmp);

    return retval;
}

__s32 filp_close(struct file *pfile)
{
    int retval = 0;

    if (!pfile->f_dentry)
        goto free_out;

    /* flush文件的所有缓存 */
    if (pfile->f_op && pfile->f_op->flush)
        retval = pfile->f_op->flush(pfile);
    if (retval)
        goto out;

    /* 文件系统close */
    if (pfile->f_op && pfile->f_op->close)
        retval = pfile->f_op->close(pfile);
    if (retval)
        goto out;


    /* 安全起见，回写磁盘上的所有缓存(暂时) */
    retval = fsync_dev(pfile->f_dentry->d_sb);

free_out:
    /* 销毁struct file数据结构，并递减dentry的引用计数 */
    fput(pfile);

out:
    return retval;
}

static void file_free(struct file *f)
{
    kmem_cache_free(filp_cachep, f);
}


/* Find an unused file structure and return a pointer to it.
 * Returns NULL, if there are no more free file structures or
 * we run out of memory.
 */
struct file *get_empty_filp(void)
{
    struct file *f;

    f = kmem_cache_alloc(filp_cachep, GFP_KERNEL);
    if (f == NULL)
        goto fail;

    memset(f, 0, sizeof(*f));

    atomic_set(&f->f_count, 1);

    return f;

fail:
    return NULL;
}

void fput(struct file *file)
{
    struct dentry *dentry = file->f_dentry;
    struct inode *inode = dentry->d_inode;

//  might_sleep();
    if (file->f_op && file->f_op->release && dentry)
        file->f_op->release(inode, file);
    file->f_dentry = NULL;
    files[file->f_fd] = NULL;
    file_free(file);
    dput(dentry);
}

void fget(struct file *file)
{
    atomic_inc(&file->f_count);
}

void put_filp(struct file *file)
{
    if (atomic_dec_and_test(&file->f_count))
    {
        file_free(file);
    }
}

/* -------------------------------- debug -------------------------*/
#ifdef _FS_DEBUG_
int __debug_dump_openfiles(void)
{
    int i;

    fs_log_info("Open files number: %d (Max:%d)\n", nr_files, NR_MAXOPEN);

    for (i = 0; i < NR_MAXOPEN; i++)
    {
        if (files[i])
        {
            struct dentry *de;

            fs_log_info("   fd: %d  ", i);
            if (de = files[i]->f_dentry)
            {
                if (S_ISDIR(de->d_inode->i_mode))
                    fs_log_info("d ");
                else
                    fs_log_info("f ");

                fs_log_info("%s          [%c: %dbytes]\n", de->d_name.name, de->d_sb->s_letter, (int)de->d_inode->i_size);
            }
            else
            {
                fs_log_info("- [on plugout media]\n");
            }
        }
    }
    return 0;
}
#endif



