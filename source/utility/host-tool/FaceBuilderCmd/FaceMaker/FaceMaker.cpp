//------------------------------------------------------------------------------------------------------------
//
// facemaker.cpp
//
// 2009-5-21 15:57:36
//
// sunny
//
//------------------------------------------------------------------------------------------------------------
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "basetypes.h"

#include "error.h"
#include "fun.h"

#include "../if/facemaker_if.h"

#include "../if/resaccess_if.h"

#include "../script_type.h"

#include "../include/res.h"


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------

#define PLUGINAPI

#include "FaceMaker.h"

#include "facemaker_inc.h"

extern void *GetResAccessInterface();

typedef  void *HFACE;

HFACE CreateFace(char *resFile, char *resHeadFile);

static u32    SetSysInfo(HFACE hFace, void *sysInfo, u32 size);

u32   SetResCount(HFACE hFace, u32 resCount);

static u32    SetResInfo(HFACE hFace, void *resInfo, u32 size, u32 index);

u32   BuildFace(HFACE hFace, char *res_template, void *res_if, u32 isClone);

u32   CloseFace(HFACE hFace);

static int IsFileExist(char *pathname)
{
    if (pathname == NULL)
        return -1;
    if (access(pathname, F_OK) == 0)
        return 0;
    return -1;

}
static unsigned int GetFileLength(signed char *filename)
{
    FILE *fp = NULL;
    fp = fopen((const char *)filename, "rb");
    if (fp == NULL)
    {
        return 0;
    }

    long position = 0;
    fseek(fp, 0, SEEK_END);
    unsigned int len = (unsigned int)ftell(fp) - position;
    fclose(fp);
    return len;
}

//------------------------------------------------------------------------------------------------------------
//
// ????˵??
//     ????????
//
// ????˵??
//
//
// ????ֵ
//
//
// ????
//    ??
//
//------------------------------------------------------------------------------------------------------------
/*
bool APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
    return TRUE;
}
*/
//------------------------------------------------------------------------------------------------------------
//??????ͨ?ýӿ?
//------------------------------------------------------------------------------------------------------------
//#include "../if/plugin_common.c"


//------------------------------------------------------------------------------------------------------------
//
// ????˵??
//
//
// ????˵??
//
//
// ????ֵ
//
//
// ????
//    ??
//
//------------------------------------------------------------------------------------------------------------
/*
PLUGINAPI u32   QueryInterface  (const char * plug_guid)
{
    if (0 == strcmp(plug_guid, (const char *)FACEMAKER_PLUGIN_TYPE))
    {
        return OK;
    }
    else
    {
        return __LINE__;
    }
}
*/
//-------------------------------------------------------------------------------------------------
//
// ?????

//
//
// ?????

//
//
// ?????

//
//
// ????
//    ??
//
//-------------------------------------------------------------------------------------------------
PLUGINAPI void *GetFaceMakerInterface()
{
    static FACE_IF __if;

//  if (0 == strcmp(plug_guid, (const char *)FACEMAKER_PLUGIN_TYPE))
//  {
    __if.CreateFace   =  CreateFace ;   //
    __if.SetSysInfo   =  SetSysInfo ;   //
    __if.SetResCount  =  SetResCount;   //
    __if.SetResInfo   =  SetResInfo ;   //
    __if.BuildFace    =  BuildFace;     //
    __if.CloseFace    =  CloseFace;     //

    return &__if;
//  }
//  else
//  {
//      return NULL;
//  }
}


//-------------------------------------------------------------------------------------------------
//
// ?????
//
//
// ?????
//
//
// ?????
//
//
// ????
//    ??
//
//-------------------------------------------------------------------------------------------------
HFACE CreateFace(char *resFile, char *resHeadFile)
{
#define __FUNCTION__ "CreateFace"

    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == resFile || NULL == resHeadFile)
    {
        return NULL;
    }

    pFaceMaker = (FACE_MAKER *)malloc(sizeof(FACE_MAKER));
    if (NULL == pFaceMaker)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "malloc failed");
        return NULL;
    }
    memset(pFaceMaker, 0, sizeof(FACE_MAKER));

    //----------------------------------------------------------
    //  initi hFace
    //----------------------------------------------------------
    strncpy(pFaceMaker->ResFile, resFile, sizeof(pFaceMaker->ResFile));
    pFaceMaker->hResFile = fopen(resFile, "wb+");        // ??Դ?ļ? : ???????ļ?
    if (NULL == pFaceMaker->hResFile)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "create res file failed (%s)", resFile);
        free(pFaceMaker);
        return NULL;
    }

    strncpy(pFaceMaker->ResHeadFile, resHeadFile, sizeof(pFaceMaker->ResHeadFile));
    pFaceMaker->hResHeadFile = fopen(resHeadFile, "w+"); // ??Դͷ?ļ? : ?ı??ļ?
    if (NULL == pFaceMaker->hResHeadFile)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "create res head file failed (%s)", resHeadFile);
        fclose(pFaceMaker->hResFile);
        free(pFaceMaker);
        return NULL;
    }

    //----------------------------------------------------------
    //  allocate buffer
    //----------------------------------------------------------
    pFaceMaker->Buffer_t.Buffer = malloc(FACE_BUFFER_LEN);
    if (NULL == pFaceMaker->Buffer_t.Buffer)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "malloc failed");
        return NULL;
    }
    pFaceMaker->Buffer_t.BufferLen = FACE_BUFFER_LEN;
    pFaceMaker->SysInfo = NULL;
    pFaceMaker->resCount = 0;
    pFaceMaker->ResTable = NULL;

    return (HFACE)pFaceMaker;

#undef __FUNCTION__
}


//-------------------------------------------------------------------------------------------------
//
// ?????
//
//
// ?????
//
//
// ?????
//
//
// ????
//    ??
//
//-------------------------------------------------------------------------------------------------
static u32    SetSysInfo(HFACE hFace, void *sysInfo, u32 size)
{
#define __FUNCTION__ "SetSysInfo"


    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == sysInfo)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // allocate system info space
    //----------------------------------------------------------
    if (pFaceMaker->SysInfo)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "system info already set");
        return __LINE__;
    }

    pFaceMaker->SysInfo = (FACE_SYS_INFO *)malloc(FACE_SYS_INFO_SIZE);

    if (NULL == pFaceMaker->SysInfo)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "malloc failed");
        return __LINE__;
    }

    //----------------------------------------------------------
    // set system info
    //----------------------------------------------------------
    if (size > FACE_SYS_INFO_SIZE)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "Invalid system info size");
        return __LINE__;
    }

    memcpy(pFaceMaker->SysInfo, sysInfo, size);

    return ret;

#undef __FUNCTION__
}


//-------------------------------------------------------------------------------------------------
//
// ?????
//
//
// ?????
//
//
// ?????
//
//
// ????
//    ??
//
//-------------------------------------------------------------------------------------------------
u32   SetResCount(HFACE hFace, u32 resCount)
{
#define __FUNCTION__ "SetResCount"

    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;


    //----------------------------------------------------------
    // set res count
    //----------------------------------------------------------
    if (pFaceMaker->resCount)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "res count already set");
        return __LINE__;
    }

    pFaceMaker->resCount = resCount;


    //----------------------------------------------------------
    // allocate res info table space
    //----------------------------------------------------------
    if (pFaceMaker->ResTable)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "res info table already allocate");
        return __LINE__;
    }
    pFaceMaker->ResTable = (FACE_RES_ITEM *)malloc(resCount * FACE_RES_ITEM_SIZE);

    return ret;

#undef __FUNCTION__
}


//-------------------------------------------------------------------------------------------------
//
// ?????
//
//
// ?????
//
//
// ?????
//
//
// ????
//    ??
//
//-------------------------------------------------------------------------------------------------
static u32    SetResInfo(HFACE hFace, void *resInfo, u32 size, u32 index)
{
#define __FUNCTION__ "SetResInfo"

    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == resInfo)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;


    //----------------------------------------------------------
    // check set conditions
    //----------------------------------------------------------
    if (size > FACE_RES_ITEM_SIZE)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "Invalid res info size");
        return __LINE__;
    }

    if (index > pFaceMaker->resCount)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "Invalid res info index");
        return __LINE__;
    }

    if (NULL == pFaceMaker->ResTable)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "no res info table space");
        return __LINE__;
    }


    //----------------------------------------------------------
    // set res info
    //----------------------------------------------------------
    memcpy(&(pFaceMaker->ResTable[index]), resInfo, size);

    return ret;

#undef __FUNCTION__
}


u32 SetResHead(HFACE hFace, RESHEAD *res_head, u32 offset)
{
#define __FUNCTION__ "SetResHead"

    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == res_head)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // seek to 'offset' position
    //----------------------------------------------------------
    if (fseek(pFaceMaker->hResFile, offset, SEEK_SET))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fseek failed");
        return __LINE__;
    }

    //----------------------------------------------------------
    // write res_head to 'offset' position
    //----------------------------------------------------------
    if (1 != fwrite(res_head, RESHEAD_SIZE, 1, pFaceMaker->hResFile))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fwrite failed");
        return __LINE__;
    }

    return ret;

#undef __FUNCTION__
}


u32 SetResStyle(HFACE hFace, STYLE *style, u32 offset)
{
#define __FUNCTION__ "SetResStyle"

    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == style)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // seek to 'offset' position
    //----------------------------------------------------------
    if (fseek(pFaceMaker->hResFile, offset, SEEK_SET))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fseek failed");
        return __LINE__;
    }

    //----------------------------------------------------------
    // write style to 'offset' position
    //----------------------------------------------------------
    if (1 != fwrite(style, STYLE_SIZE, 1, pFaceMaker->hResFile))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fwrite failed");
        return __LINE__;
    }

    return ret;

#undef __FUNCTION__
}


u32 SetResItem(HFACE hFace, RESITEM *res_item, u32 offset)
{
#define  __FUNCTION__ "SetResItem"
    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;


    if (NULL == hFace || NULL == res_item)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // seek to 'offset' position
    //----------------------------------------------------------
    if (fseek(pFaceMaker->hResFile, offset, SEEK_SET))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fseek failed");
        return __LINE__;
    }


    //----------------------------------------------------------
    // write style to 'offset' position
    //----------------------------------------------------------
    if (1 != fwrite(res_item, RESITEM_SIZE, 1, pFaceMaker->hResFile))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fwrite failed");
        return __LINE__;
    }

    return ret;

#undef __FUNCTION__
}


u32 IsNullFileName(char *file_name)
{
    u32 ret = OK;

    if (strcasecmp(file_name, NULL_FILE_NAME))
    {
        return __LINE__;
    }

    return ret;
}


u32 SetResItemData(HFACE hFace, char *res_file, u32 offset)
{
#define __FUNCTION__ "SetResItemData"

    u32 ret = OK;
    FILE *fp = NULL;
    u32  Left = 0;
    u32  FileSize = 0;
    u32  DataSize = 0;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == res_file)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // check res file exist or not
    //----------------------------------------------------------
    ret = IsNullFileName(res_file);
    if (OK != ret)
    {
        ret = IsFileExist(res_file);
        if (OK != ret)
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "res file %s not exist", res_file);
            return  __LINE__;
        }
    }
    else
    {
        Err(__FUNCTION__, __FILE__, __LINE__,  "res file can't be NULL while makeface");
        return  __LINE__;
    }

    //----------------------------------------------------------
    // get file size
    //----------------------------------------------------------
    FileSize = GetFileLength((s8 *)res_file);
    if (0 == FileSize)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "invalid res file size (%s)", res_file);
        return __LINE__;
    }

    //----------------------------------------------------------
    // open res file
    //----------------------------------------------------------
    fp = fopen(res_file, "rb");
    if (NULL == fp)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "open res file failed");
        return __LINE__;
    }

    //----------------------------------------------------------
    // seek to 'offset' position
    //----------------------------------------------------------
    if (fseek(pFaceMaker->hResFile, offset, SEEK_SET))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fseek failed");
        return __LINE__;
    }

    Left = FileSize;
    while (Left)
    {
        memset(pFaceMaker->Buffer_t.Buffer, 0, pFaceMaker->Buffer_t.BufferLen);

        DataSize = Left < pFaceMaker->Buffer_t.BufferLen ? Left : pFaceMaker->Buffer_t.BufferLen;

        if (1 != fread(pFaceMaker->Buffer_t.Buffer, DataSize, 1, fp))
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "read res file fail");
            fclose(fp);
            return __LINE__;
        }
        Left -= DataSize;

        //----------------------------------------------------------
        // write data size align
        //----------------------------------------------------------
        DataSize = RES_ALIGN_FUNC(DataSize);
        if (1 != fwrite(pFaceMaker->Buffer_t.Buffer, DataSize, 1, pFaceMaker->hResFile))
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "fwrite failed");
            fclose(fp);
            return __LINE__;
        }
    }
    fclose(fp);

    return ret;

#undef __FUNCTION__
}


u32 CloneResItemData(HFACE hFace, HRES hRes, void *res_if, u16 StyleID, u16 ID, u32 offset)
{
#define __FUNCTION__ "CloneResItemData"

    u32 ret = OK;
    u32  DataSize = 0;
    void *DataBuffer = NULL;
    FACE_MAKER *pFaceMaker = NULL;
    RESACCESS_IF *pResAccess = NULL;

    if (NULL == hFace || NULL == hRes || NULL == res_if)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    pResAccess = (RESACCESS_IF *)res_if;

    //----------------------------------------------------------
    // get res size
    //----------------------------------------------------------
    DataSize = pResAccess->GetResSize(hRes, StyleID, ID);
    if (0 == DataSize)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "Invalid res data size");
        return __LINE__;
    }

    DataBuffer = malloc(DataSize);
    if (NULL == DataBuffer)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "malloc failed");
        return __LINE__;
    }
    memset(DataBuffer, 0, DataSize);

    //----------------------------------------------------------
    // get res data
    //----------------------------------------------------------
    ret = pResAccess->GetRes(hRes, StyleID, ID, DataBuffer, DataSize);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "read res file fail");
        free(DataBuffer);
        return __LINE__;
    }

    //----------------------------------------------------------
    // seek to 'offset' position
    //----------------------------------------------------------
    if (fseek(pFaceMaker->hResFile, offset, SEEK_SET))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fseek failed");
        free(DataBuffer);
        return __LINE__;
    }

    //----------------------------------------------------------
    // write res data to 'offset' position
    //----------------------------------------------------------
    if (1 != fwrite(DataBuffer, DataSize, 1, pFaceMaker->hResFile))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "fwrite failed");
        free(DataBuffer);
        return __LINE__;
    }

    free(DataBuffer);
    return ret;

#undef __FUNCTION__
}


u32 GetResHead(HFACE hFace, RESHEAD *res_head)
{
    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == res_head)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    strncpy((char *)(res_head->SecName), RES_SEC_NAME, sizeof(res_head->SecName));
    res_head->version       = RES_VERSION;
    res_head->size          = RESHEAD_SIZE;
    res_head->StyleOffset   = RES_ALIGN_FUNC(RESHEAD_SIZE);

    //----------------------------------------------------------
    // ????????Ϊ1??һ????Դ?ļ?Ҳ???ж??????⣬Ŀǰ??δʹ??
    //----------------------------------------------------------
    res_head->StyleNum      = 2;
    res_head->ResNum        = pFaceMaker->resCount;
    res_head->align         = RES_ALIGN;
    res_head->flags         = SYSRES;

    return ret;

}


u32 GetResStyle(HFACE hFace, STYLE *style, u16 styleID, u32 res_size)
{
    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == style)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    style->ID = styleID;
    strncpy((char *)(style->Name), pFaceMaker->SysInfo->UIstyle, sizeof(style->Name));

    //----------------------------------------------------------
    // ??Դ??λ??ƫ??��????Դ????ͷ???? + ??????????
    //----------------------------------------------------------
    style->ResTableOffset = RES_ALIGN_FUNC(RESHEAD_SIZE) + RES_ALIGN_FUNC(STYLE_SIZE);

    //----------------------------------------------------------
    // ??Դ????λ??ƫ??��????Դ????ͷ???? + ?????????? +
    //                     ??Դ??????????
    //----------------------------------------------------------
    style->ResOffset = RES_ALIGN_FUNC(RESHEAD_SIZE) + RES_ALIGN_FUNC(STYLE_SIZE) +
                       pFaceMaker->resCount * RES_ALIGN_FUNC(RESITEM_SIZE);

    style->ResSize = res_size;

    return ret;
}


u32 GetResItem(HFACE hFace, RESITEM *res_item,
               u32 id, u32 offset, u32 size)
{
    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == res_item)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // Ŀǰ??δ????res_item->type????1
    //----------------------------------------------------------
    res_item->type = 1;
    res_item->ID = id;
    strncpy((char *)(res_item->Name), pFaceMaker->ResTable[id - 1].Name, sizeof(res_item->Name));
    res_item->offset = offset;
    res_item->size = size;

    return ret;
}


u32 MakeResFile(HFACE hFace, HRES hRes, char *res_template, void *res_if, u32 isClone)
{
#define __FUNCTION__ "MakeResFile"


    u32 i;
    u32 ret = OK;
    u32 ResSize = 0;
    RESHEAD ResHead;
    STYLE   Style;
    RESITEM res_item;
    FACE_MAKER *pFaceMaker = NULL;
    RESACCESS_IF *pResAccess = NULL;
    u32 ResTableOffset = 0;
    u32 ResDataOffset = 0;

    /*
    if (NULL == hFace || (isClone && (NULL == res_template || NULL == res_if)))
    {
        return __LINE__;
    }
    */
    pFaceMaker = (FACE_MAKER *)hFace;



//    pResAccess = (RESACCESS_IF *)res_if; GetResAccessInterface();
    pResAccess = (RESACCESS_IF *)GetResAccessInterface();

    //----------------------------------------------------------
    // set res head
    //----------------------------------------------------------
    ret = GetResHead(hFace, &ResHead);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "get res head failed");
        return ret;
    }

    ret = SetResHead(hFace, &ResHead, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "set res head failed");
        return ret;
    }
    //----------------------------------------------------------
    // set res items
    //----------------------------------------------------------
    ResSize = 0;

    //----------------------------------------------------------
    // ??Դ??λ??ƫ??��????Դ????ͷ???? + ??????????
    //----------------------------------------------------------
    ResTableOffset = RES_ALIGN_FUNC(RESHEAD_SIZE) + RES_ALIGN_FUNC(STYLE_SIZE);

    //----------------------------------------------------------
    // ??Դ????λ??ƫ??��????Դ????ͷ???? + ?????????? +
    //                     ??Դ??????????
    //----------------------------------------------------------
    ResDataOffset = RES_ALIGN_FUNC(RESHEAD_SIZE) + RES_ALIGN_FUNC(STYLE_SIZE) +
                    pFaceMaker->resCount * RES_ALIGN_FUNC(RESITEM_SIZE);


    for (i = 0; i < pFaceMaker->resCount; i++)
    {
        u32 ItemPos = 0;
        u32 DataPos = 0;
        u32 DataSize = 0;

        ItemPos = ResTableOffset + i * RES_ALIGN_FUNC(RESITEM_SIZE);
        DataPos = ResDataOffset + RES_ALIGN_FUNC(ResSize);

        //----------------------------------------------------------
        // ??Դ?ļ???Ϊ??ʱ????ģ??????ȡ??
        //----------------------------------------------------------
        ret = IsNullFileName(pFaceMaker->ResTable[i].File);
        if (OK == ret)
        {
            if (NULL == res_template || NULL == res_if)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "Wrong clone mode");
                return __LINE__;
            }

            DataSize = pResAccess->GetResSize(hRes, pFaceMaker->SysInfo->StyleID, pFaceMaker->ResTable[i].ID);
            if (0 == DataSize)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "Invalid res data size");
                return __LINE__;
            }

            ret = GetResItem(hFace, &res_item, pFaceMaker->ResTable[i].ID, DataPos, DataSize);
            if (OK != ret)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "get res item failed");
                return __LINE__;
            }

            ret = SetResItem(hFace, &res_item, ItemPos);
            if (OK != ret)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "set res data failed");
                return __LINE__;
            }

            ret = CloneResItemData(hFace, hRes, res_if, pFaceMaker->SysInfo->StyleID, pFaceMaker->ResTable[i].ID, DataPos);
            if (OK != ret)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "clone res data failed");
                return __LINE__;
            }
        }
        else
        {
            DataSize = GetFileLength((s8 *)(pFaceMaker->ResTable[i].File));
            if (0 == DataSize)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "invalid res file size (%s)", pFaceMaker->ResTable[i].File);
                return __LINE__;
            }

            ret = GetResItem(hFace, &res_item,
                             pFaceMaker->ResTable[i].ID, DataPos, DataSize);
            if (OK != ret)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "get res item failed");
                return __LINE__;
            }

            ret = SetResItem(hFace, &res_item, ItemPos);
            if (OK != ret)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "set res data failed");
                return __LINE__;
            }

            ret = SetResItemData(hFace, pFaceMaker->ResTable[i].File, DataPos);
            if (OK != ret)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "set res data failed");
                return __LINE__;
            }
        }
        ResSize += RES_ALIGN_FUNC(DataSize);
    }

    //----------------------------------------------------------
    // set res style
    //----------------------------------------------------------
    ret = GetResStyle(hFace, &Style, pFaceMaker->SysInfo->StyleID, ResSize);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "get res style failed");
        return __LINE__;
    }

    ret = SetResStyle(hFace, &Style, RES_ALIGN_FUNC(RESHEAD_SIZE));
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "set res style failed");
        return __LINE__;
    }

    return ret;

#undef __FUNCTION__
}


u32 SetResHeadFileBeginInfo(HFACE hFace)
{
    u32 ret = OK;
    u32 i = 0;
    u32 j = 0;
    char BeginInfo[1024];
    char Buffer[MAX_PATH + 32];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // ͷ?ļ???ͷ??Ϣδ��??
    // 1.???ܴ???һ???????ļ??У????????ļ???ȡ
    // 2.ͷ?ļ???ʱ????Ϣ???ܴ?ϵͳ??̬??ȡ
    // 3.???ܰ?????Ȩ??Ϣ
    //----------------------------------------------------------

    //........

    //----------------------------------------------------------
    // Buffer store "____XXX_XXX_H____" format string
    // in head file
    //----------------------------------------------------------
    memset(Buffer, 0, sizeof(Buffer));
    memset(Buffer, '_', 4);                 // set string begin with "____"

    //_splitpath(pFaceMaker->ResHeadFile, drive, dir, fname, ext);
    get_file_fname(pFaceMaker->ResHeadFile, fname);
    get_file_ext(pFaceMaker->ResHeadFile, ext);
    get_file_dir(pFaceMaker->ResHeadFile, dir);

    while (i < strlen(fname))
    {
        Buffer[i + 4] = toupper(fname[i]);
        i++;
    }
    while (j < strlen(ext))
    {
        if ('.' == ext[j])
        {
            Buffer[i + j + 4] = '_';
        }
        else
        {
            Buffer[i + j + 4] = toupper(ext[j]);
        }
        j++;
    }
    memset(&Buffer[i + 4], '_', 4);          // set string end with "____"

    //----------------------------------------------------------
    // set head file begin info
    //----------------------------------------------------------
    memset(BeginInfo, 0, sizeof(BeginInfo));
    sprintf(BeginInfo, "//---------------------------------------"
            "-----------------------------------------"
            "----------------------------\n"
            "//\n"
            "// %s.%s\n"
            "//\n"
            "// 2009.5.26\n"
            "//\n"
            "// Sunny\n"
            "//\n"
            "//---------------------------------------"
            "-----------------------------------------"
            "----------------------------\n\n\n"
            "#ifndef  %s\n"
            "#define  %s     1\n\n",
            fname, ext, Buffer, Buffer);

    if (1 != fwrite(BeginInfo, strlen(BeginInfo), 1, pFaceMaker->hResHeadFile))
    {
        Err("ResHeadFileBeginInfo", __FILE__, __LINE__, "write res head file begin info failed");
        return __LINE__;
    }

    return ret;
}


u32 SetResHeadFileStyleID(HFACE hFace, u16 styleID)
{
    u32 ret = OK;
    u32 len = 0;
    char StyleID[RES_NAME_LEN + 32];
    char StyleAlign[RES_NAME_LEN + 8];
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    //  ??ͷ?ļ??ж?????????Դ??
    //  ??Դ?????油?ո????룬??????һ???ַ??????ǽ?????'\0'
    //----------------------------------------------------------
    memset(StyleID, 0, sizeof(StyleID));
    memset(StyleAlign, 0, sizeof(StyleAlign));
    sprintf(StyleAlign, "%s", "STYLEID");
    len = strlen(StyleAlign);
    memset(&StyleAlign[len], ' ', sizeof(StyleAlign) - len - 1);
    sprintf(StyleID, "\n#define    %s0x%x\n", StyleAlign, styleID);

    if (1 != fwrite(StyleID, strlen(StyleID), 1, pFaceMaker->hResHeadFile))
    {
        Err("SetResHeadFileStyleID", __FILE__, __LINE__,
            "write res head file styleID failed");
        return __LINE__;
    }

    return ret;
}


u32 SetResHeadFileEndInfo(HFACE hFace)
{
    u32 ret = OK;
    u32 i = 0;
    u32 j = 0;
    char EndInfo[1024];
    char Buffer[MAX_PATH + 32];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // Buffer store "____XXX_XXX_H____" format string
    // in head file
    //----------------------------------------------------------
    memset(Buffer, 0, sizeof(Buffer));
    memset(Buffer, '_', 4);                 // set string begin with "____"

    //_splitpath(pFaceMaker->ResHeadFile, drive, dir, fname, ext);
    get_file_fname(pFaceMaker->ResHeadFile, fname);
    get_file_ext(pFaceMaker->ResHeadFile, ext);
    get_file_dir(pFaceMaker->ResHeadFile, dir);

    while (i < strlen(fname))
    {
        Buffer[i + 4] = toupper(fname[i]);
        i++;
    }
    while (j < strlen(ext))
    {
        if ('.' == ext[j])
        {
            Buffer[i + j + 4] = '_';
        }
        else
        {
            Buffer[i + j + 4] = toupper(ext[j]);
        }
        j++;
    }
    memset(&Buffer[i + 4], '_', 4);          // set string end with "____"

    //----------------------------------------------------------
    // set head file end info
    //----------------------------------------------------------
    memset(EndInfo, 0, sizeof(EndInfo));
    sprintf(EndInfo, "\n\n//-------------------------------------"
            "-----------------------------------------"
            "------------------------------\n"
            "// THE END !\n"
            "//---------------------------------------"
            "-----------------------------------------"
            "----------------------------\n\n\n"
            "#endif      // %s\n\n\n", Buffer);
    if (1 != fwrite(EndInfo, strlen(EndInfo), 1, pFaceMaker->hResHeadFile))
    {
        Err("ResHeadFileEndInfo", __FILE__, __LINE__,
            "write res head file end info failed");
        return __LINE__;
    }

    return ret;
}


u32  SetResHeadFileItemInfo(HFACE hFace, char *name, u32 name_len, u32 ID)
{
    u32 i = 0;
    u32 ret = OK;
    u32 len = 0;
    char IDString[RES_NAME_LEN + 32];
    char NameString[2 * RES_NAME_LEN + 32];
    char UpCaseName[RES_NAME_LEN + 1];
    char NameAlign[RES_NAME_LEN + 8];
    char IDAlign[RES_NAME_LEN + 8];

    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace || NULL == name)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // convert name to upcase type
    //----------------------------------------------------------
    memset(UpCaseName, 0, sizeof(UpCaseName));
    for (i = 0; i < name_len; i++)
    {
        UpCaseName[i] = toupper(name[i]);
    }

    //----------------------------------------------------------
    // maybe write comment info first
    //----------------------------------------------------------

    //......

    //----------------------------------------------------------
    //  ??ͷ?ļ??ж?????????Դ??
    //  ??Դ?????油?ո????룬??????һ???ַ??????ǽ?????'\0'
    //----------------------------------------------------------
    memset(NameString, 0, sizeof(NameString));
    memset(NameAlign, 0, sizeof(NameAlign));
    sprintf(NameAlign, "NAME_%s", UpCaseName);
    len = strlen(NameAlign);
    memset(&NameAlign[len], ' ', sizeof(NameAlign) - len - 1);
    sprintf(NameString, "\n#define    %s\"%s\"\n", NameAlign, UpCaseName);
    if (1 != fwrite(NameString, strlen(NameString), 1, pFaceMaker->hResHeadFile))
    {
        Err("SetResHeadFileItemInfo", __FILE__, __LINE__, "write ID info failed");
        return __LINE__;
    }

    //----------------------------------------------------------
    //  ??ͷ?ļ??ж???????ID??
    //  ID?ź??油?ո????룬??????һ???ַ??????ǽ?????'\0'
    //----------------------------------------------------------
    memset(IDString, 0, sizeof(IDString));
    memset(IDAlign, 0, sizeof(IDAlign));
    sprintf(IDAlign, "ID_%s", UpCaseName);
    len = strlen(IDAlign);
    memset(&IDAlign[len], ' ', sizeof(IDAlign) - len - 1);
    sprintf(IDString, "#define    %s%d\n", IDAlign, ID);
    if (1 != fwrite(IDString, strlen(IDString), 1, pFaceMaker->hResHeadFile))
    {
        Err("SetResHeadFileItemInfo", __FILE__, __LINE__, "write ID info failed");
        return __LINE__;
    }

    return ret;
}


u32 MakeResHeadFile(HFACE hFace, u32 isClone)
{
#define __FUNCTION__ "MakeResHeadFile"

    u32 ret = OK;
    u32 len = 0;
    u32 i = 0;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // res head file begin info
    //----------------------------------------------------------
    ret = SetResHeadFileBeginInfo(hFace);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,  "set res head file begin info");
        return __LINE__;
    }

    //----------------------------------------------------------
    // set styleID
    //----------------------------------------------------------
    ret = SetResHeadFileStyleID(hFace, pFaceMaker->SysInfo->StyleID);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,   "set res head file styleID");
        return __LINE__;
    }

    //----------------------------------------------------------
    // set res items info
    //----------------------------------------------------------
    for (i = 0; i < pFaceMaker->resCount; i++)
    {
        len = strlen(pFaceMaker->ResTable[i].Name);
        if (0 == len)
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "Invalid res item name");
            return __LINE__;
        }
        ret = SetResHeadFileItemInfo(hFace, pFaceMaker->ResTable[i].Name, len, pFaceMaker->ResTable[i].ID);

        if (OK != ret)
        {
            Err(__FUNCTION__, __FILE__, __LINE__,  "set res item failed");
            return __LINE__;
        }
    }

    //----------------------------------------------------------
    // res head file end info
    //----------------------------------------------------------
    ret = SetResHeadFileEndInfo(hFace);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "set res head file end info");
        return __LINE__;
    }

    return ret;

#undef __FUNCTION__
}


u32 MakeFace(HFACE hFace)
{
#define __FUNCTION__ "MakeFace"
    u32 ret = OK;
    if (NULL == hFace)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    // make res file
    //----------------------------------------------------------
    ret = MakeResFile(hFace, NULL, NULL, NULL, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "make res file failed");
        return __LINE__;
    }
    //----------------------------------------------------------
    // make res head file
    //----------------------------------------------------------
    ret = MakeResHeadFile(hFace, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "make res head file failed");
        return __LINE__;
    }
    //----------------------------------------------------------
    // maybe merge res file and res head file here
    //----------------------------------------------------------

    return ret;
#undef __FUNCTION__
}


u32 CloneFace(HFACE hFace, char *res_template, void *res_if)
{
#define __FUNCTION__ "CloneFace"

    u32 ret = OK;
    HRES  hRes = NULL;
    RESACCESS_IF *pResAccess = NULL;

    if (NULL == hFace || NULL == res_template || NULL == res_if)
    {
        return __LINE__;
    }

    pResAccess = (RESACCESS_IF *)res_if;

    //----------------------------------------------------------
    // open res
    //----------------------------------------------------------
    hRes = pResAccess->OpenRes(res_template, 0);
    if (NULL == hRes)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "OpenRes failed");
        return __LINE__;
    }

    //----------------------------------------------------------
    // make res file
    //----------------------------------------------------------
    ret = MakeResFile(hFace, hRes, res_template, res_if, 1);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "make res file failed");
        return __LINE__;
    }

    //----------------------------------------------------------
    // make res head file
    //----------------------------------------------------------
    ret = MakeResHeadFile(hFace, 1);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "make res head file failed");
        return __LINE__;
    }

    //----------------------------------------------------------
    // maybe merge res file and res head file here
    //----------------------------------------------------------

    //.....

    //----------------------------------------------------------
    // close res
    //----------------------------------------------------------

    ret = pResAccess->CloseRes(hRes);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CloseRes failed");
        return __LINE__;
    }

    return ret;

#undef __FUNCTION__
}


//-------------------------------------------------------------------------------------------------
//
// ?????
//
//
// ?????
//
//
// ?????
//
//
// ????
//    ??
//
//-------------------------------------------------------------------------------------------------
u32  BuildFace(HFACE hFace, char *res_template, void *res_if, u32 isClone)
{
    u32 ret = OK;

    if (isClone)
    {
        ret = CloneFace(hFace, res_template, res_if);
    }
    else
    {
        ret = MakeFace(hFace);
    }

    return ret;
}


//-------------------------------------------------------------------------------------------------
//
// ?????
//
//
// ?????
//
//
// ?????
//
//
// ????
//    ??
//
//-------------------------------------------------------------------------------------------------
u32   CloseFace(HFACE hFace)
{
    u32 ret = OK;
    FACE_MAKER *pFaceMaker = NULL;

    if (NULL == hFace)
    {
        return __LINE__;
    }

    pFaceMaker = (FACE_MAKER *)hFace;

    //----------------------------------------------------------
    // first : close res file and res head file
    //----------------------------------------------------------
    if (pFaceMaker->hResFile)
    {
        fclose(pFaceMaker->hResFile);
        pFaceMaker->hResFile = NULL;
    }

    if (pFaceMaker->hResHeadFile)
    {
        fclose(pFaceMaker->hResHeadFile);
        pFaceMaker->hResHeadFile = NULL;
    }

    //----------------------------------------------------------
    // second : free system info space
    //----------------------------------------------------------
    if (pFaceMaker->SysInfo)
    {
        free(pFaceMaker->SysInfo);
        pFaceMaker->SysInfo = NULL;
    }


    //----------------------------------------------------------
    // third : free res info table space
    //----------------------------------------------------------
    if (pFaceMaker->ResTable)
    {
        free(pFaceMaker->ResTable);
        pFaceMaker->resCount = 0;
        pFaceMaker->ResTable = NULL;
    }

    //----------------------------------------------------------
    // fourth : free buffer
    //----------------------------------------------------------
    if (pFaceMaker->Buffer_t.Buffer)
    {
        free(pFaceMaker->Buffer_t.Buffer);
        pFaceMaker->Buffer_t.BufferLen = 0;
    }

    //----------------------------------------------------------
    // last : free FACEMAKER body space
    //----------------------------------------------------------
    free(pFaceMaker);
    pFaceMaker = NULL;

    return ret;
}


//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

