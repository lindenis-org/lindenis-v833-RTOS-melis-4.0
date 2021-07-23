//------------------------------------------------------------------------------------------------------------
//
// scriptparser_if.h
//
// 2009-05-20 16:45:36
//
//
//
//------------------------------------------------------------------------------------------------------------


#ifndef __SCRIPTPARSER_IF___H____
#define __SCRIPTPARSER_IF___H____   1


#include "scriptparser_plug_type.h"

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef void *HSCRIPT;

typedef HSCRIPT(*pOpenScript)(char *script_path, void *config_if);

typedef u32(*pParseScript)(HSCRIPT hScript);

typedef u32(*pGetSysInfo)(HSCRIPT hScript, void *sysinfo, u32 size);

typedef u32(*pGetResCount)(HSCRIPT hScript, u32 *resCount);

typedef u32(*pGetResInfo)(HSCRIPT hScript, void *resinfo, u32 size, u32 index);

typedef u32(*pCloseScript)(HSCRIPT hScript);

// u32 return OK means function operation success, otherwise means failed .

typedef u32(*pSetSysInfo)(HSCRIPT hScript, void *sysinfo, u32 size);

typedef u32(*pSetResInfo)(HSCRIPT hScript, void *resinfo, u32 size, u32 index);

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_SCRIPTPARSER_IF
{

    pOpenScript   OpenScript;       //

    pParseScript  ParseScript;      //

    pGetSysInfo   GetSysInfo;       //

    pGetResCount  GetResCount;      //

    pGetResInfo   GetResInfo;       //

    pCloseScript  CloseScript;      //

    // 补充的接口
    pSetSysInfo   SetSysInfo;       //将system信息写入脚本

    pSetResInfo   SetResInfo;       //将item信息写入脚本

} SCRIPTPARSER_IF;
#pragma pack(pop)


#endif //__SCRIPTPARSER_IF___H____






