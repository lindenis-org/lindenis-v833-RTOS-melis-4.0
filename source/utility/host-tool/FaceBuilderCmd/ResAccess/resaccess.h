//------------------------------------------------------------------------------------------------------------
//
// ResAccess.h
//
// 2009-05-21 14:25:19
//
// scott
//
//------------------------------------------------------------------------------------------------------------


#ifndef ___RES_ACCESS____H____
#define ___RES_ACCESS____H____  1


#include "../if/ResAccess_plug_type.h"

#ifdef PLUGINAPI

#else
//#define PLUGINAPI extern "C" __declspec(dllimport)
#endif

#define PLUGIN_TYPE             RESACCESS_PLUGIN_TYPE   //
#define PLUGIN_NAME             "resaccess"             //
#define PLUGIN_VERSION          0x0100
#define PLUGIN_AUTHOR           "scottyu"
#define PLUGIN_COPYRIGHT        "scottyu"


//插件的通用接口
#include "../if/plugin_common.h"

#endif //___RES_ACCESS____H____
