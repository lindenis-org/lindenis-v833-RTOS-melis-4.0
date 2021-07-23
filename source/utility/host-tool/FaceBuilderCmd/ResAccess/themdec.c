/*
************************************************************************************************************************
*                                                       RESOURCE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : themdec.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.05.19
*
* Description :
*
* Functions list :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.05.19       1.1.0        build the file
*
************************************************************************************************************************
*/
#include "themdec_private.h"
#include "string.h"

#if defined( RES_IN_ELF_FILE )
typedef __resm_rsb_t               *UNI_FP;
#define uni_fopen                   esRESM_ROpen
#define uni_fclose                  esRESM_RClose
#define uni_fseek                   esRESM_RSeek
#define uni_fread                   esRESM_RRead
#else
typedef RES_FILE                   *UNI_FP;
#define uni_fopen                   res_fopen
#define uni_fclose                  res_fclose
#define uni_fseek                   res_fseek
#define uni_fread                   res_fread
#endif

#define ILLEGAL_STYLE_ID                (uint16)-1
#define ILLEGAL_RES_ID                  (uint16)-1

typedef struct _RES_i_t
{
    UNI_FP fp;
    uint32 first_style_pos;
    uint32 style_nr;
    uint32 res_nr;
    STYLE  cur_style;
    uint32 cur_style_pos;
    RESITEM cur_res;
    uint32  cur_res_pos;
} RES_i_t, *HRES_i;

#define EPOS_APP_EXTENTION              ".axf"
#define RES_FILE_EXTENTION              ".res"

#define RI_M_OFFSET_OFFSET              ( (uint32) (&( ( (RESITEM *)0 )->offset ) ) )

/*-----------------------------------------function list------------------------------------------*/
HRES OpenRes(char *szAppFile, __u32 mode)
{
    UNI_FP  fp;
    HRES_i  hdl;
    RESHEAD file_head;

    /* check mode */
    if (mode != 0)
    {
        Msg("Fail in opening resource %s in mode %d. Because mode %d is illegal.\n", szAppFile, mode, mode);
        return NULL;
    }

    /* open file and position file pointer to the head of resource */
#if EPDK_OS == EPDK_OS_EPOS
    fp = uni_fopen(szAppFile, "r_them");
#else
    fp = uni_fopen(szAppFile, "rb");
#endif
    if (fp == NULL)
    {
        Msg("Fail in opening resource %s in mode %d. Because can't open resource file.\n", szAppFile, mode);
        return NULL;
    }

    /* create handle */
    hdl = (HRES_i)res_malloc(sizeof(RES_i_t));
    if (hdl == NULL)
    {
        Msg("Fail in opening resource %s in mode %d. Because can't allocate memory.\n", szAppFile, mode);
        uni_fclose(fp);
        return NULL;
    }

    uni_fread((void *)&file_head, 1, sizeof(file_head), fp);
    //scott
    uni_fseek(fp, 0, SEEK_CUR);

    hdl->first_style_pos = file_head.StyleOffset;
    hdl->style_nr = file_head.StyleNum;
    hdl->res_nr = file_head.ResNum;
    hdl->fp = fp;
    hdl->cur_style.ID = ILLEGAL_STYLE_ID;
    hdl->cur_res.ID   = ILLEGAL_RES_ID;

    return (HRES)hdl;
}

u32 CloseRes(HRES hRes)
{
    HRES_i hdl;

    if (hRes == NULL)
    {
        Msg("Fail in Closing resource. Because handle is NULL.\n");
        return -1;
    }
    hdl = (HRES_i)hRes;

    uni_fclose(hdl->fp);
    res_free(hdl);
    return 0;
}

uint32 ReadRes(HRES hRes, uint32 address, uint32 length, void *buffer)
{
    HRES_i hdl;
    uint32 count;

    if (hRes == NULL)
    {
        Msg("Fail in reading resource buffer. Because handle is NULL.\n");
        return 0;
    }
    hdl = (HRES_i)hRes;

    uni_fseek(hdl->fp, address, SEEK_SET);
    count = uni_fread(buffer, 1, length, hdl->fp);

    //scott
    uni_fseek(hdl->fp, 0, SEEK_CUR);
    return count;
}

static uint32 search_res(HRES_i hdl, uint16 style_id, uint16 res_id)
{
    uint32 i;
    STYLE  style_item;
    RESITEM res_item;

    /* search style */
    if (hdl->cur_style.ID != style_id)
    {
        uni_fseek(hdl->fp, hdl->first_style_pos, SEEK_SET);
        for (i = 0;  i < hdl->style_nr;  i++)
        {
            uni_fread((void *)&style_item, 1, sizeof(style_item), hdl->fp);
            //scott
            uni_fseek(hdl->fp, 0, SEEK_CUR);

            if (style_item.ID == style_id)
            {
                hdl->cur_style_pos = hdl->first_style_pos + i * sizeof(STYLE);
                memcpy(&(hdl->cur_style), &style_item, sizeof(STYLE));
                Msg(" search_res find style_id %d\n", i);
                break;
            }
        }
        if (i >= hdl->style_nr)
        {
            Msg("Fail in search_res find style_id failed.\n");
            return 0;
        }
    }

    Msg("hdl->res_nr=%d", hdl->res_nr);

    /* search resource */
    uni_fseek(hdl->fp, hdl->cur_style.ResTableOffset, SEEK_SET);
    for (i = 0;  i < hdl->res_nr;  i++)
    {
        uni_fread((void *)&res_item, 1, sizeof(res_item), hdl->fp);
        //scott
        uni_fseek(hdl->fp, 0, SEEK_CUR);

        if (res_item.ID == res_id)
        {
            hdl->cur_res_pos = hdl->cur_style.ResTableOffset + i * sizeof(RESITEM) ;
            memcpy(&(hdl->cur_res), &res_item, sizeof(RESITEM));
            return (hdl->cur_style.ResTableOffset + i * sizeof(RESITEM));
        }
    }
    Msg("Fail in search_res find res_id failed.\n");
    return 0;
}

uint32 GetResSize(HRES hRes, uint16 StyleID, uint16 ID)
{
    HRES_i hdl;
    uint32 res_pos;

    if (hRes == NULL)
    {
        Msg("Fail in getting resource size. Because handle is NULL.\n");
        return 0;
    }
    hdl = (HRES_i)hRes;

    /* find in cache */
    if (hdl->cur_style.ID == StyleID  &&  hdl->cur_res.ID == ID)
    {
        return hdl->cur_res.size;
    }

    res_pos = search_res(hdl, StyleID, ID);
    if (res_pos == 0)
    {
        Msg("Fail in getting the size of resource whose style id is %d and resource id is %d. Because can't find resouce.\n", StyleID, ID);
        return 0;
    }
    return hdl->cur_res.size;
}

uint32 GetResAddr(HRES hRes, uint16 StyleID, uint16 ID)
{
    HRES_i hdl;
    uint32 res_pos;

    if (hRes == NULL)
    {
        Msg("Fail in getting resource size. Because handle is NULL.\n");
        return 0;
    }
    hdl = (HRES_i)hRes;

    /* find in cache */
    if (hdl->cur_style.ID == StyleID  &&  hdl->cur_res.ID == ID)
    {
        return hdl->cur_res.offset;
    }

    res_pos = search_res(hdl, StyleID, ID);
    if (res_pos == 0)
    {
        Msg("Fail in getting the size of resource whose style id is %d and resource id is %d. Because can't find resouce.\n", StyleID, ID);
        return 0;
    }
    return hdl->cur_res.offset;
}

uint16 GetResType(HRES hRes, uint16 StyleID, uint16 ID)
{
    HRES_i hdl;
    uint32 res_pos;

    if (hRes == NULL)
    {
        Msg("Fail in getting resource size. Because handle is NULL.\n");
        return 0xFFFF;
    }
    hdl = (HRES_i)hRes;

    /* find in cache */
    if (hdl->cur_style.ID == StyleID  &&  hdl->cur_res.ID == ID)
    {
        return hdl->cur_res.type;
    }

    res_pos = search_res(hdl, StyleID, ID);
    if (res_pos == 0)
    {
        Msg("Fail in getting the size of resource whose style id is %d and resource id is %d. Because can't find resouce.\n", StyleID, ID);
        return 0xFFFF;
    }
    return hdl->cur_res.type;
}

u32 GetRes(HRES hRes, uint16 StyleID, uint16 ID, void *Buffer, uint32 Length)
{
    HRES_i hdl;
    uint32 res_pos;
    uint32 count;

    if (hRes == NULL)
    {
        Msg("Fail in getting resource size. Because handle is NULL.\n");
        return -1;
    }
    hdl = (HRES_i)hRes;

    /* find in cache */
    if (hdl->cur_style.ID == StyleID  &&  hdl->cur_res.ID == ID)
    {
        res_pos = hdl->cur_res_pos;
    }

    res_pos = search_res(hdl, StyleID, ID);
    if (res_pos == 0)
    {
        Msg("Fail in getting the size of resource whose style id is %d and resource id is %d. Because can't find resouce.\n", StyleID, ID);
        return -1;
    }

    count = (hdl->cur_res.size < Length ? hdl->cur_res.size : Length);
    uni_fseek(hdl->fp, hdl->cur_res.offset, SEEK_SET);
    uni_fread(Buffer, 1, count, hdl->fp);

    // scott
    uni_fseek(hdl->fp, 0, SEEK_CUR);

    return 0;
}
