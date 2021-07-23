#ifndef __SYS_MOD_STUB_H__
#define __SYS_MOD_STUB_H__
#include <typedef.h>
__u32 esMSTUB_GetFuncEntry(__s32 id, __u32 funcnum);
__s32 esMSTUB_UnRegFuncTbl(__s32 id);
__s32 esMSTUB_RegFuncTbl(__s32 id, void *tbl);

#endif  /* __SYS_MOD_MSTUB_H__ */