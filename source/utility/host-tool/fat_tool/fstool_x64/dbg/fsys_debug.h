#ifndef __FSYS_DEBUG_H__
#define __FSYS_DEBUG_H__

//#define _FS_DEBUG_

typedef struct debug_para_msg
{
    int         inuse;

    struct inode *ino;
    int         pos;
    int         len;
} debug_para_msg_t;
extern debug_para_msg_t debug_para;

//#include "..\dbg\fs_cachedump.h"
//#include "..\dbg\fs_logging.h"
//#include "..\dbg\fs_timer.h"
#include "../dbg/fs_cachedump.h"
#include "../dbg/fs_logging.h"
#include "../dbg/fs_timer.h"

#endif      /* __FSYS_DEBUG_H__ */

