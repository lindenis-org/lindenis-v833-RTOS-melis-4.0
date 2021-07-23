#include "basetypes.h"
#include "if/part_ram_plug_type.h"

//------------------------------------------------------------------------------
//                  RAM Part分区私有信息
//------------------------------------------------------------------------------
typedef struct RAMPartPri
{
    __u8        *BaseAddr;      /* base address of ram part     */
    __u32        SctSize;       /* sector size of ram part      */
    __u32        SctCnt;        /* sector number of ram part    */
    __u32        RealCnt;
} RAMPartPri;

//------------------------------------------------------------------------------------------------------------
#define PLUGIN_TYPE             RAMPART_PLUGIN_TYPE         //
#define PLUGIN_NAME             "rampart"                   //
#define PLUGIN_VERSION          0x0100
#define PLUGIN_AUTHOR           "Sunny"
#define PLUGIN_COPYRIGHT        "Sunny"

__u32 rampart_read(void *pBuffer, __u32 nSectNum, __u32 nSectorCnt, __hdle hPart);

__u32 rampart_write(const void *pBuffer, __u32 nSectNum, __u32 nSectorCnt, __hdle hPart);
__s32 rampart_ioctrl(__hdle hPart, __u32 Cmd, __s32 Aux, void *pBuffer);
void *rampart_open(__hdle hPart, void *pArg);

int rampart_close(__hdle hPart, void *pPri);
char *rampart_get_class_name(void);
__u32 rampart_get_real_size(__hdle hPart);
