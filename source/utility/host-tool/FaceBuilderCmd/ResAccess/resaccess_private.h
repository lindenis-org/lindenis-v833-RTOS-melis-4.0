//------------------------------------------------------------------------------------------------------------
//
// resaccess_private.h
//
// scott
//
// 2009-05-21 14:32:09
//
//------------------------------------------------------------------------------------------------------------
#ifndef __RESACCESS_PRIVATE___H____
#define __RESACCESS_PRIVATE___H____

typedef void *HRES;

HRES OpenRes(char *szAppFile, u32 mode);

u32 CloseRes(HRES hRes);

u32 ReadRes(HRES hRes, u32 address, u32 length, void *buffer);

u32 GetResSize(HRES hRes, u16 StyleID, u16 ID);

u32 GetResAddr(HRES hRes, u16 StyleID, u16 ID);

u16 GetResType(HRES hRes, u16 StyleID, u16 ID);

u32 GetRes(HRES hRes, u16 StyleID, u16 ID, void *Buffer, u32 Length);

#endif //__RESACCESS_PRIVATE___H____
