//------------------------------------------------------------------------------------------------------------
//
// scriptparser_inc.h
//
// 2009-5-22 8:55:41
//
// sunny
//
//------------------------------------------------------------------------------------------------------------


#ifndef ___SCRIPT_PARSER_INC_H____
#define ___SCRIPT_PARSER_INC_H____  1

#include "../script_type.h"

#include "../include/res_id.h"

//------------------------------------------------------------------------------------------------------------
//  资源系统信息
//------------------------------------------------------------------------------------------------------------
#define SECNAME_SYSTEM              "system"            //
#define KEYNAME_SYS_SCRIPTVER       "scriptver"         //
#define KEYNAME_SYS_UISTYLE         "uistyle"           //
#define KEYNAME_SYS_NAME            "name"              //
#define KEYNAME_SYS_AUTHOR          "author"            //
#define KEYNAME_SYS_SOLUTION        "solution"          //
#define KEYNAME_SYS_STYLENAME       "stylename"         //
//#define KEYNAME_SYS_STYLEID         "styleid"         //


//------------------------------------------------------------------------------------------------------------
//  资源类型
//------------------------------------------------------------------------------------------------------------
#define SECNAME_RES_PREFIX          "res_"              //
#define KEYNAME_RES_NAME            "name"              //
#define KEYNAME_RES_FILE            "file"              //
#define KEYNAME_RES_COMMENTCN       "commentCN"         //
#define KEYNAME_RES_COMMENTEN       "commentEN"         //
//#define KEYNAME_RES_ID            "id"                //


//------------------------------------------------------------------------------------------------------------
//   空文件名
//------------------------------------------------------------------------------------------------------------
#define NULL_FILE_NAME          "NULL"


//------------------------------------------------------------------------------------------------------------
//  Face Script : 512 Bytes
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_FACE_SCRIPT
{
    char            ScriptFileName[MAX_PATH];
    u8              Zero0;
    FACE_SYS_INFO  *SysInfo;
    u32             ResCount;
    FACE_RES_ITEM  *ResInfoTable;
    void           *config_if;
    void           *hConfig;
    u8              Reserved[512 - MAX_PATH - 21];
} FACE_SCRIPT;
#pragma pack(pop)

//------------------------------------------------------------------------------------------------------------
// END
//------------------------------------------------------------------------------------------------------------


#endif //___SCRIPT_PARSER_INC_H____

