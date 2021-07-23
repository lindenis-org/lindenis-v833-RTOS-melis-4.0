//------------------------------------------------------------------------------------------------------------
//
// resaccess_if.h
//
// 2009-05-21 14:30:32
//
// scott
//
//------------------------------------------------------------------------------------------------------------


#ifndef __RESACCESS_IF___H____
#define __RESACCESS_IF___H____  1


#include "ResAccess_plug_type.h"


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------

typedef void *HRES;

typedef HRES(*pOpenRes)(char *szAppFile, u32 mode);

typedef u32(*pCloseRes)(HRES hRes);

typedef u32(*pReadRes)(HRES hRes, u32 address, u32 length, void *buffer);

typedef u32(*pGetResSize)(HRES hRes, u16 StyleID, u16 ID);

typedef u32(*pGetResAddr)(HRES hRes, u16 StyleID, u16 ID);

typedef u16(*pGetResType)(HRES hRes, u16 StyleID, u16 ID);

typedef u32(*pGetRes)(HRES hRes, u16 StyleID, u16 ID, void *Buffer, u32 Length);

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_RESACCESS_IF
{

    pOpenRes        OpenRes;

    pCloseRes       CloseRes;

    pReadRes        ReadRes;

    pGetResSize     GetResSize;

    pGetResAddr     GetResAddr;

    pGetResType     GetResType;

    pGetRes         GetRes;

} RESACCESS_IF;
#pragma pack(pop)

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#endif //__RESACCESS_IF___H____







