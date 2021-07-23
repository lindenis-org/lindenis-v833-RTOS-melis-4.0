#ifndef __PART_H__
#define __PART_H__

//#include "fsys_libs.h"
//#include "..\if\fstool_if.h"
#include "../fsys_libs.h"
#include "../if/fstool_if.h"

extern __krnl_event_t *pPartSem;

extern __u32  esFSYS_pread(void *pData, __u32 sector, __u32 n, __hdle hPart);
extern __u32  esFSYS_pwrite(const void *pData, __u32 Sector, __u32 N, __hdle hPart);
extern __s32  esFSYS_pioctrl(__hdle hPart, __u32 cmd, __s32 aux, void *pbuffer);
extern __hdle esFSYS_findpart(const char *pFullName);

#endif

