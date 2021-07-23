

#include "buffer_head.h"
#include "list.h"
#include "page_pool.h"

#include "fsys_debug.h"

struct cache_dump_info
{
    int show_sb_inode;
    int show_lru_inode;
    int show_sb_dentry;
    int show_lru_dentry;
    int show_lru_fatcache;
    int show_free_page;
    int show_lru_page;
    int show_bh_list;
    int show_page_info;
    int show_bh_info;

    int dump_mems;

    int show_all_cache_info;
    int disable_all_cache_info;

    int detail;
};

#define can_dump(which)    \
({  \
    int  __res;    \
                        \
    if( cache_dump_set.disable_all_cache_info)   \
        __res = 0;                  \
    else if( cache_dump_set.show_all_cache_info)   \
        __res = 1;                  \
    else if (!cache_dump_set.which)        \
        __res = 0;          \
    else                    \
        __res = 1;              \
                                \
    __res;                  \
})

extern struct cache_dump_info cache_dump_set;

extern void debug_show_sb_inode(struct list_head *head, char *name, int align_level);
extern void debug_show_lru_inode(struct list_head *head, char *name, int align_level);

extern void debug_show_sb_dentry(struct list_head *head, char *name, int align_level);
extern void debug_show_lru_dentry(struct list_head *head, char *name, int align_level);

extern void debug_show_lru_page(struct list_head *head, char *name, int align_level);
extern void debug_show_free_page(struct list_head *head, char *name, int align_level);

extern void debug_show_bh_list(struct buffer_head *head, char *name, int align_level);
extern void debug_show_bh_inpage(struct buffer_head *head, char *name, int align_level);

extern void debug_show_all_bh_list(char *info);
extern void debug_show_all_page_list(char *info);
extern void debug_show_all_inode(char *info);
extern void debug_show_all_dentry(char *info);
extern void debug_show_all_caches(char *when);

extern void debug_show_page_info(char *when);

extern void __dump_mem(char *buf, int dev_start, int len);
extern int debug_dump_mems(char *buf, char *name, int dev_start, int len);

extern BOOL fs_dump_parse_option(const char *option);
extern int __debug_dump_openfiles(void);
