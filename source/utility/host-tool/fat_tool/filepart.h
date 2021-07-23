#ifndef __FILE_PART_H_
#define __FILE_PART_H_

#include "basetypes.h"

__u32 filepart_read(void *pBuffer, __u32 nSectNum, __u32 nSectorCnt, __hdle hPart);

__u32 filepart_write(const void *pBuffer, __u32 nSectNum, __u32 nSectorCnt, __hdle hPart);

__s32 filepart_ioctrl(__hdle hPart, __u32 Cmd, __s32 Aux, void *pBuffer);

void *filepart_open(__hdle hPart, void *pArg);
int filepart_close(__hdle hPart, void *pPri);

char *filepart_get_class_name(void);

__u32 filepart_get_real_size(__hdle hPart);
#endif
