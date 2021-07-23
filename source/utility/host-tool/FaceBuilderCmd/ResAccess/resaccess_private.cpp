//------------------------------------------------------------------------------------------------------------
//
// resaccess_private.cpp
//
// scott
//
// 2009-05-21 14:32:09
//
//------------------------------------------------------------------------------------------------------------
#include <string.h>
#include "resaccess_private.h"
#include "fun.h"
/*
#include "themdec.c"
*/
#define min(x,y) (x>y ? y : x)
#include "../include/res.h"         //

typedef struct tag_RES_HANDLE
{
    char        filepath[MAX_PATH];     //文件路径
    u32         mode;                   //打开模式
    FILE       *fp;                     //文件句柄
    RESHEAD     head;                   //资源文件头
    STYLE      *style;                  //风格表
    RESITEM    *resarray;               //一种风格的资源表
    u16         CurrentStyleID;         //当前风格ID
    u16         reserved;               //
} RES_HANDLE;

static RESITEM *GetResItem(HRES hRes, u16 StyleID, u16 ID);

HRES OpenRes(char *szAppFile, u32 mode)
{
    RES_HANDLE *handle = NULL;
    u32 ret = OK;
    u32 i = 0;

    ret = IsFileExist(szAppFile);
    if (OK != ret)
    {
        return NULL;
    }
    handle = (RES_HANDLE *)malloc(sizeof(RES_HANDLE));
    if (NULL == handle)
    {
        goto Error_End;
    }

    memset(handle, 0, sizeof(RES_HANDLE));

    strcpy(handle->filepath, szAppFile);
    handle->mode = mode;

    handle->fp = fopen(handle->filepath, "rb");
    if (NULL == handle->fp)
    {
        goto Error_End;
    }

    if (1 != fread(&handle->head, sizeof(RESHEAD), 1, handle->fp))
    {
        goto Error_End;
    }
    fseek(handle->fp, 0, SEEK_CUR);

    if (0 == handle->head.StyleNum)
    {
        goto Error_End;
    }

    handle->style = (STYLE *)malloc(handle->head.StyleNum * sizeof(STYLE));
    if (NULL == handle->style)
    {
        goto Error_End;
    }
    memset(handle->style, 0, handle->head.StyleNum * sizeof(STYLE));

    fseek(handle->fp, handle->head.StyleOffset, SEEK_SET);
    if (1 != fread(handle->style, handle->head.StyleNum * sizeof(STYLE), 1, handle->fp))
    {
        goto Error_End;
    }
    fseek(handle->fp, 0, SEEK_CUR);

    handle->CurrentStyleID = handle->style[0].ID;

    handle->resarray = (RESITEM *)malloc(handle->head.ResNum * sizeof(RESITEM));
    if (NULL == handle->resarray)
    {
        goto Error_End;
    }

    memset(handle->resarray, 0, handle->head.ResNum * sizeof(RESITEM));

    for (i = 0; i < handle->head.StyleNum; i++)
    {
        if (handle->style[i].ID == handle->CurrentStyleID)
        {
            fseek(handle->fp, handle->style[i].ResTableOffset, SEEK_SET);
            if (1 != fread(handle->resarray, handle->head.ResNum * sizeof(RESITEM), 1, handle->fp))
            {
                goto Error_End;
            }

            fseek(handle->fp, 0, SEEK_CUR);

            break;
        }
    }

    return handle;

Error_End:
    if (NULL != handle)
    {
        if (NULL != handle->style)
        {
            free(handle->style);
            handle->style = NULL;
        }

        if (NULL != handle->resarray)
        {
            free(handle->resarray);
            handle->resarray = NULL;
        }

        if (NULL != handle->fp)
        {
            fclose(handle->fp);
            handle->fp = NULL;
        }
        free(handle);
        handle = NULL;
    }
    return NULL;
}

u32 CloseRes(HRES hRes)
{
    u32 ret = OK;
    RES_HANDLE *handle = (RES_HANDLE *)hRes;
    if (NULL == handle)
    {
        return __LINE__;
    }

    if (NULL != handle->style)
    {
        free(handle->style);
        handle->style = NULL;
    }

    if (NULL != handle->resarray)
    {
        free(handle->resarray);
        handle->resarray = NULL;
    }


    if (NULL != handle->fp)
    {
        fclose(handle->fp);
        handle->fp = NULL;
    }

    free(handle);
    handle = NULL;

    return OK;
}

u32 ReadRes(HRES hRes, u32 address, u32 length, void *buffer)
{
    u32 ret = OK;
    RES_HANDLE *handle = (RES_HANDLE *)hRes;
    if (NULL == handle)
    {
        return __LINE__;
    }

    if (NULL == buffer)
    {
        return __LINE__;
    }

    if (NULL == handle->fp)
    {
        return __LINE__;
    }

    fseek(handle->fp, address, SEEK_SET);
    if (1 != fread(buffer, length, 1, handle->fp))
    {
        return __LINE__;
    }

    return ret;
}

static RESITEM *GetResItem(HRES hRes, u16 StyleID, u16 ID)
{
    u32 i = 0;
    u32 j = 0;

    RES_HANDLE *handle = (RES_HANDLE *)hRes;
    if (NULL == handle)
    {
        return NULL;
    }

    if (NULL == handle->resarray)
    {
        return NULL;
    }
    if (StyleID == handle->CurrentStyleID)
    {
        for (j = 0; j < handle->head.ResNum; j++)
        {
            if (ID == handle->resarray[j].ID)
            {
                return &handle->resarray[j];
            }
        }
    }
    else
    {
        memset(handle->resarray, 0, handle->head.ResNum * sizeof(RESITEM));

        for (i = 0; i < handle->head.StyleNum; i++)
        {
            if (handle->style[i].ID == StyleID)//)
            {
                fseek(handle->fp, handle->style[i].ResTableOffset, SEEK_SET);
                if (1 != fread(handle->resarray, handle->head.ResNum * sizeof(RESITEM), 1, handle->fp))
                {
                    return NULL;
                }

                fseek(handle->fp, 0, SEEK_CUR);

                handle->CurrentStyleID = StyleID;

                for (j = 0; j < handle->head.ResNum; j++)
                {
                    if (ID == handle->resarray[j].ID)
                    {
                        return &handle->resarray[j];
                    }
                }

                break;
            }
        }
    }

    return NULL;
}

u32 GetResSize(HRES hRes, u16 StyleID, u16 ID)
{
    u32 ret = OK;

    RESITEM *resitem = GetResItem(hRes, StyleID, ID);
    if (NULL == resitem)
    {
        return 0;
    }
    return resitem->size;
}

u32 GetResAddr(HRES hRes, u16 StyleID, u16 ID)
{
    u32 ret = OK;

    RESITEM *resitem = GetResItem(hRes, StyleID, ID);
    if (NULL == resitem)
    {
        return 0;
    }
    return resitem->offset;

}

u16 GetResType(HRES hRes, u16 StyleID, u16 ID)
{
    u32 ret = OK;

    RESITEM *resitem = GetResItem(hRes, StyleID, ID);
    if (NULL == resitem)
    {
        return 0;
    }
    return resitem->type;

}

u32 GetRes(HRES hRes, u16 StyleID, u16 ID, void *Buffer, u32 Length)
{
    u32 readsize = 0;

    RES_HANDLE *handle = (RES_HANDLE *)hRes;
    if (NULL == handle)
    {
        return __LINE__;
    }

    if (NULL == Buffer)
    {
        return __LINE__;
    }

    if (0 == Length)
    {
        return __LINE__;
    }

    RESITEM *resitem = GetResItem(hRes, StyleID, ID);
    if (NULL == resitem)
    {
        return __LINE__;
    }

    readsize = min(Length, resitem->size);

    fseek(handle->fp, resitem->offset, SEEK_SET);
    if (1 != fread(Buffer, readsize, 1, handle->fp))
    {
        return __LINE__;
    }

    fseek(handle->fp, 0, SEEK_CUR);

    return OK;
}
