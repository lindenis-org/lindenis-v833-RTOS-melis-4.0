//------------------------------------------------------------------------------------------------------------
//
// part_if.h
//
// 2009-05-20 16:45:36
//
//
//
//------------------------------------------------------------------------------------------------------------


#ifndef __PART_IF___H____
#define __PART_IF___H____   1


#include "part_ram_plug_type.h"
#include "Part_File_Plugin_if.h"
// other partition

typedef     void           *__hdle;
typedef     unsigned int    __u32;
typedef     int             __s32;
typedef     unsigned char   __u8;

//------------------------------------------------------------------------------
//                  RAM Part分区信息,用于打开分区时的输入参数
//------------------------------------------------------------------------------
#define RAM_PART_NAME       "RAMPartiton"
typedef struct RAMPartInfo
{
    __u8        *BaseAddr;      /* base address of ram part     */
    __u32        SctSize;       /* sector size of ram part      */
    __u32        SctCnt;        /* sector number of ram part    */
    __u32        RealCnt;
} RAMPartInfo;


//------------------------------------------------------------------------------
//                  分区通用操作接口
//------------------------------------------------------------------------------
typedef char *(*pPartGetClassName)(void);
typedef void *(*pPartopen)(__hdle pPart, void *pArg);
typedef __u32(*pPartread)(void *pData, __u32 nSectNum, __u32 nSectorCnt, __hdle pPart);
typedef __u32(*pPartwrite)(const void *pData, __u32 nSectNum, __u32 nSectorCnt, __hdle pPart);
typedef __s32(*pPartioctl)(__hdle pPart, __u32 Cmd, __s32 Aux, void *pBuffer);
typedef __s32(*pPartclose)(__hdle pPart, void *pPri);
typedef __u32(*pPartGetRealSize)(__hdle pPart);
typedef struct tag_PARTITION_IF
{
    pPartopen           PartOpen;
    pPartread           PartRead;
    pPartwrite          PartWrite;
    pPartioctl          PartIoctl;
    pPartclose          PartClose;
    pPartGetClassName   GetClassName;
    pPartGetRealSize    PartGetRealSize;

} __attribute__((packed)) PARTITION_IF;

#endif      //__PART_IF___H____
