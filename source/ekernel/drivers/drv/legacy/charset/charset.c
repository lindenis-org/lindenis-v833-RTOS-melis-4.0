/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Charset Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : charset.c
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-24 9:24
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-24 9:24     kevin.z     2.0     build the file.
*********************************************************************************************************
*/
#include <typedef.h>
#include <mod_defs.h>
#include <mod_charset.h>
#include <log.h>

static __u32    CharsetMid = 0;
__s32 UTF8_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
__s32 UTF8_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
__u32 UTF8_get_charset2upper_table(void *buf, __u32 size);
__u32 UTF8_get_charset2lower_table(void *buf, __u32 size);

/*
*********************************************************************************************************
*                                   CHARSET INITIALISE
*
* Description: charset initialise.
*
* Arguments  : mode     mode of charset initialise;
*              arg      argument for charset initialise;
*
* Returns    : result, EPDK_OK/EPDK_FAIL
*
* Note       : charset is an external module, just create a link to access the module here.
*********************************************************************************************************
*/
__s32 eschs_init(__u32 mode, void *p_arg)
{
    //install the charset module
    CharsetMid = esMODS_MInstall("d:\\mod\\charset.mod", 0);
    if (!CharsetMid)
    {
        __wrn("try to install charset module failed!");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   CHARSET EXIT
*
* Description: charset exit.
*
* Arguments  : none
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void esCHS_Exit(void)
{
    if (CharsetMid)
    {
        esMODS_MUninstall(CharsetMid);
        CharsetMid = 0;
    }

    return;
}


/*
*********************************************************************************************************
*                               TRANSLATE UNICODE TO THE GIVEN CHARSET
*
* Description: translate unicode to the given charset.
*
* Arguments  : type
*              uni
*              str
*              len
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__s32 esCHS_Uni2Char(__s32 type, __u16 uni, __u8 *str, __u32 len)
{
    if (CharsetMid)
    {
        return CHS_Uni2Char(type, uni, str, len);
    }
    else
    {
        //use default charset (utf8)
        return UTF8_uni2char(uni, str, len);
    }
}


/*
*********************************************************************************************************
*                               TRANSLIATE THE GIVEN CHARSET TO UNICODE
*
* Description: translate the given charset to unicode.
*
* Arguments  : type
*              str
*              len
*              uni
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__s32 esCHS_Char2Uni(__s32 type, const __u8 *str, __u32 len, __u16 *uni)
{
    if (CharsetMid)
    {
        return CHS_Char2Uni(type, str, len, uni);
    }
    else
    {
        return UTF8_char2uni(str, len, uni);
    }
}


/*
*********************************************************************************************************
*                                   GET CHARSET UPPER TABLE
*
* Description: get charset upper table.
*
* Arguments  : type
*              buf
*              size
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 esCHS_GetChUpperTbl(__s32 type, void *buf, __u32 size)
{
    if (CharsetMid)
    {
        return CHS_GetChUpperTbl(type, buf, size);
    }

    return UTF8_get_charset2upper_table(buf, size);
}


/*
*********************************************************************************************************
*                                   GET CHARSET LOWER TABLE
*
* Description: get charset lower table.
*
* Arguments  : type
*              buf
*              size
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 esCHS_GetChLowerTbl(__s32 charset_type, void *buf, __u32 size)
{
    if (CharsetMid)
    {
        return CHS_GetChLowerTbl(charset_type, buf, size);
    }

    return UTF8_get_charset2lower_table(buf, size);
}
