//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//    I BBMMBB      BBMMBBI     BBI EEMMFFI BBMMI     BBI EEMMFFI BBMMI       BBMMBBI   EEMMI EEMMLL        //
//    EEI   EEFF  EEI   LLBB  EEMMBBI I BBEE  I EE  EEMMBBI I BBEE  I EE    EEI   LLBB    EEBBI I BB        //
//  I FF        I EE      BBI   EEI     EEI   I EE    EEI     EEI   I EE  I EE      BBI   EEI   I EE        //
//  LLFF        LLEE      EELL  EEI     EEI   I EE    EEI     EEI   I EE  LLEE      EELL  EEI   I EE        //
//  I EE      I I BB      EEI   EEI     EEI   I EE    EEI     EEI   I EE  I BB      EEI   EEI   I EE        //
//    BBLL  I I   BBFF  I EE    EEI     EEI   I BB    EEI     EEI   I BB    BBFF  I EE    BBI   I EE        //
//    I BBMMEE    I BBMMBB    EEMMMMLLBBMMBBLLMMMMEEEEMMMMLLBBMMBBLLMMMMEE  I BBMMBB    EEMMMMLLBBMMBB      //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                                Scott System                                              //
//                                                                                                          //
//                               (c) Copyright 2006-2007, Scottyu China                                     //
//                                                                                                          //
//                                           All Rights Reserved                                            //
//                                                                                                          //
// File    : plugin_common.h                                                                                //
// By      : scottyu                                                                                        //
// Version : V1.00                                                                                          //
// Time    : 2008-10-25 14:39:43                                                                            //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
// HISTORY                                                                                                  //
//                                                                                                          //
// 1 2008-10-25 14:39:46                                                                                    //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//

#ifndef ___PLUGIN__COMMON___H______
#define ___PLUGIN__COMMON___H______         1


//------------------------------------------------------------------------------------------------------------
//插件的通用接口
//------------------------------------------------------------------------------------------------------------

PLUGINAPI s8 *GetPlugInType();

PLUGINAPI s8 *GetPlugInName();

PLUGINAPI u32  GetPlugInVersion();

PLUGINAPI s8 *GetAuthor();

PLUGINAPI s8 *GetCopyRight();


PLUGINAPI u32   QueryInterface(const char *plug_guid);

PLUGINAPI void *GetInterface(const char *plug_guid);

//------------------------------------------------------------------------------------------------------------
//插件的通用接口 for script
//------------------------------------------------------------------------------------------------------------

#ifdef SCRIPT_LUA

static int proxy_GetPlugInType(lua_State *L);

static int proxy_GetPlugInName(lua_State *L);

static int proxy_GetPlugInVersion(lua_State *L);

static int proxy_GetAuthor(lua_State *L);

static int proxy_GetCopyRight(lua_State *L);

#endif //SCRIPT_LUA

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //___PLUGIN__COMMON___H______

