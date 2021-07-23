//------------------------------------------------------------------------------------------------------------
//
//2008-11-03 9:45:50
//
//scott
//
//ImgDecode_if.h
//
//------------------------------------------------------------------------------------------------------------


#ifndef __IMGDECODE_IF___H____
#define __IMGDECODE_IF___H____  1


#include "Imgdecode_plug_type.h"


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef void *HIMAGE;

typedef void *HIMAGEITEM;


typedef HIMAGE(*pImg_Open)(char *ImageFile);

typedef HIMAGEITEM(*pImg_OpenItem)(HIMAGE hImage, char *MainType, char *subType);

typedef u32(*pImg_GetItemSize)(HIMAGE hImage, HIMAGEITEM hItem);

typedef u32(*pImg_ReadItemData)(HIMAGE hImage, HIMAGEITEM hItem, void *buffer, u32 Length);

typedef u32(*pImg_CloseItem)(HIMAGE hImage, HIMAGEITEM hItem);

typedef void (*pImg_Close)(HIMAGE hImage);

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_IMGDECODE_IF
{
    pImg_Open           Img_Open;
    pImg_OpenItem       Img_OpenItem;
    pImg_GetItemSize    Img_GetItemSize;
    pImg_ReadItemData   Img_ReadItemData;
    pImg_CloseItem      Img_CloseItem;
    pImg_Close          Img_Close;
} IMGDECODE_IF;
#pragma pack(pop)

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------

#endif //__IMGDECODE_IF___H____



