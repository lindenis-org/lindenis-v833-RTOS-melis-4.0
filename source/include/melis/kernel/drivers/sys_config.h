#ifndef  __SYS_CONFIG_HH__
#define  __SYS_CONFIG_HH__
#include  <typedef.h>
#include  <csp.h>

H_PARSER esCFG_Init_Ex(char *path);
__s32 esCFG_GetSecKeyCount(char *SecName);
__s32 esCFG_GetSecCount(void);
__s32 esCFG_GetGPIOSecKeyCount(char *GPIOSecName);
__s32 esCFG_GetGPIOSecData(char *GPIOSecName, void *pGPIOCfg, __s32 GPIONum);
__s32 esCFG_Exit_Ex(H_PARSER parser);
__s32 esCFG_GetKeyValue_Ex(H_PARSER parser, char *KeyName, __s32 Value[], __s32 Count);
__s32 esCFG_GetSecKeyCount_Ex(H_PARSER parser, char *SecName);
__s32 esCFG_GetSecCount_Ex(H_PARSER parser);
__s32 esCFG_GetGPIOSecKeyCount_Ex(H_PARSER parser, char *GPIOSecName);
__s32 esCFG_GetGPIOSecData_Ex(H_PARSER parser, char *GPIOSecName, void *pGPIOCfg, __s32 GPIONum);
__s32 esCFG_Init(__u8 *CfgVAddr, __u32 size);
__s32 esCFG_Exit(void);
__s32 esCFG_GetKeyValue(char *SecName, char *KeyName, __s32 Value[], __s32 Count);

#endif  // __SYS_CONFIG_H__
