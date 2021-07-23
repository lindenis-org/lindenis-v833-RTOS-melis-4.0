#ifndef __SYS_PINS_H__
#define __SYS_PINS_H__
#include <typedef.h>
#include <arch/chip/csp_pin_para.h>

extern __s32 PINS_Init(void);
//PINS
extern __hdle esPINS_PinGrpReq(user_gpio_set_t *pGrpStat, __u32 GrpSize);
extern __s32 esPINS_PinGrpRel(__hdle hPin, __s32 bRestore);
extern __s32 esPINS_GetPinGrpStat(__hdle hPin, user_gpio_set_t *pGrpStat, __u32 GrpSize, __bool bFromHW);
extern __s32 esPINS_GetPinStat(__hdle hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bFromHW);
extern __s32 esPINS_SetPinStat(__hdle hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bSetUserStat);
extern __s32 esPINS_SetPinIO(__hdle hPin, __bool bOut, const char *pPinName);
extern __s32 esPINS_SetPinPull(__hdle hPin, __u32 PullStat, const char *pPinName);
extern __s32 esPINS_SetPinDrive(__hdle hPin, __u32 DriveLevel, const char *pPinName);
extern __s32 esPINS_ReadPinData(__hdle hPin, const char *pPinName);
extern __s32 esPINS_WritePinData(__hdle hPin, __u32 Value, const char *pPinName);
extern __s32 esPINS_EnbaleInt(__hdle hPin);
extern __s32 esPINS_DisbaleInt(__hdle hPin);
extern __s32 esPINS_QueryInt(__hdle hPin, __bool *pIntStatus);
extern __s32 esPINS_SetIntMode(__hdle hPin, __u32 IntMode);
extern __s32 esPINS_RegIntHdler(__hdle hPin, __pCBK_t hdler, void *arg);
extern __s32 esPINS_UnregIntHdler(__hdle hPin, __pCBK_t hdler);
extern __s32 esPINS_ClearPending(__hdle hPin);

#endif
