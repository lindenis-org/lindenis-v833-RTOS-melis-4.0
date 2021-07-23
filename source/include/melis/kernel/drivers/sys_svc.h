#ifndef __SYS_SVC__
#define __SYS_SVC__

//--------------------------------------------------------------------
//      resource management sevices
//--------------------------------------------------------------------
// work mode for resource request.
#define RESOURCE_REQ_MODE_WAIT     0
#define RESOURCE_REQ_MODE_NWAIT    1

// resources define
#define RESOURCE_VE_HW  (1<<0)

__s32 svc_init(void);
__s32 svc_exit(void);

void SVC_default(void);

__s32 esSVC_RegCreatePath(const char *path);
__s32 esSVC_RegDeletePath(const char *path);
__s32 esSVC_RegCloseNode(__hdle handle);
__s32 esSVC_RegDeleteNode(const  char *node);
__s32 esSVC_RegCreateSet(__hdle handle, const char *set_name);
__s32 esSVC_RegDeleteSet(__hdle handle, const char *set_name);
__s32 esSVC_RegGetSetCount(__hdle handle, __u32 *count_p);
__s32 esSVC_RegGetFirstSet(__hdle handle);
__s32 esSVC_RegGetNextSet(__hdle handle, char *set_name);
__s32 esSVC_RegCreateKey(__hdle hNode, const char *set_name, const char *key_name, const char *key_value);
__s32 esSVC_RegDeleteKey(__hdle hNode, const char *set_name, const char *key_name);
__s32 esSVC_RegGetKeyCount(__hdle handle, const char *set_name, __u32 *count_p);
__s32 esSVC_RegGetFirstKey(__hdle handle, const char *set_name);
__s32 esSVC_RegGetNextKey(__hdle hNode, const char *set_name, char *key_name);
__s32 esSVC_RegSetKeyValue(__hdle hNode, const char *set_name, const char *key_name, const char *key_value);
__s32 esSVC_RegGetKeyValue(__hdle hNode, const char *set_name, const char *key_name, char *key_value);
__s32 esSVC_RegGetError(__hdle handle);
__s32 esSVC_RegClrError(__hdle handle);
__s32 esSVC_ResourceRel(__hdle hRes);
__s32 esSVC_RegIni2Reg(const char *ini_file);
__s32 esSVC_RegReg2Ini(const char *ini_file);

void esSVC_RegSetRootPath(const char *path);
void esSVC_RegGetRootPath(char *path);

__hdle esSVC_ResourceReq(__u32 res, __u32 mode, __u32 timeout);
__hdle esSVC_RegOpenNode(const char *node, __s32 mode);

#endif
