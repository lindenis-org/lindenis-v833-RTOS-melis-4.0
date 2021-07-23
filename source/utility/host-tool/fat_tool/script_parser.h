//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
// File    : config_parser.h                                                                                //
// By      : scottyu                                                                                        //
// Version : V1.00                                                                                          //
// Time    : 2009-07-01 14:50:06                                                                            //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

u32 ParserConfigInfo(const char *config_file,
                     void /*CFG_SYSTEM*/ * cfg_system,
                     void /*CFG_FSINFO*/ * cfg_fsinfo,
                     void *config_if);

u32 ReleaseConfigInfo(void /*CFG_FSINFO*/ * cfg_fsinfo);

#endif  //__CONFIG_PARSER_H__
