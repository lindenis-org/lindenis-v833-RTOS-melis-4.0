#ifndef _LINUX_DIRENT_H
#define _LINUX_DIRENT_H

#define NAME_OFFSET(de) ((int) ((de)->d_name - (char *) (de)))

struct dirent
{
    long        d_ino;
    unsigned short  d_reclen;
    char        d_name[256]; /* We must not include limits.h! */
};

struct linux_dirent64
{
    __u64           d_ino;
    __s64           d_off;
    unsigned short  d_reclen;
    unsigned char   d_type;
    __u64           d_size;
    char            d_name[1];
};

#endif
