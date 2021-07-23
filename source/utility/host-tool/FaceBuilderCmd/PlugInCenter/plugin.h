//------------------------------------------------------------------------------------------------------------
//
// PlugIn.h
//
// 2009-05-26 14:20:17
//
// scott
//
//------------------------------------------------------------------------------------------------------------

#ifndef ___PLUGIN___H_____
#define ___PLUGIN___H_____

//------------------------------------------------------------------------------------------------------------
// 插件接口
//------------------------------------------------------------------------------------------------------------

#include "if/config_if.h"                   //接口
#include "if/resaccess_if.h"                    //接口
#include "if/scriptparser_if.h"             //接口
#include "if/facemaker_if.h"                    //接口
#include "if/imagebuilder_if.h"             //接口
#include "if/ImgDecode_if.h"                    //接口

#define CONFIG_PLUG         "config.dll"        //插件
#define RESACCESS_PLUG      "resaccess.dll"     //插件
#define SCRIPTPARSER_PLUG   "scriptparser.dll"  //插件
#define FACEMAKER_PLUG      "facemaker.dll"     //插件
#define IMGBUILDER_PLUG     "ImageBuilder.dll"  //插件
#define IMGDECODE_PLUG      "imgdecode.dll"     //插件

typedef struct tag_PLUG_INFO
{
    char *plug_guid;    //插件ID
    char *plug_name;    //插件文件
} PLUG_INFO_t;

#ifndef GUID_LEN
#define GUID_LEN    36
#endif //GUID_LEN
#endif //___PLUGIN___H_____
