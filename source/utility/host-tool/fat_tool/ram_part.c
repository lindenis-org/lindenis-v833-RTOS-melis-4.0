#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basetypes.h"
#include "if/fstool_if.h"
#include "if/part_if.h"
#include "ram_part.h"

#define     PLUGINAPI   extern "C" __declspec(dllexport)

__u32 rampart_read(void *pBuffer, __u32 nSectNum, __u32 nSectorCnt, __hdle hPart)
{
    RAMPartPri *pRAMPart = (RAMPartPri *)(((__fsys_part_t *)hPart)->hPDPrivate);

    if (nSectNum >= pRAMPart->SctCnt)
    {
        /* Out of physical range */
        return 0;
    }
    if (nSectNum + nSectorCnt >= pRAMPart->SctCnt)
    {
        nSectorCnt = pRAMPart->SctCnt - nSectNum;
    }

    memcpy(pBuffer, (char *)(pRAMPart->BaseAddr + nSectNum * pRAMPart->SctSize), nSectorCnt * pRAMPart->SctSize);
    return nSectorCnt;
}

__u32 rampart_write(const void *pBuffer, __u32 nSectNum, __u32 nSectorCnt, __hdle hPart)
{
    RAMPartPri *pRAMPart = (RAMPartPri *)(((__fsys_part_t *)hPart)->hPDPrivate);

    if (nSectNum >= pRAMPart->SctCnt)
    {
        /* Out of physical range */
        return 0;
    }
    if (pRAMPart->RealCnt < nSectNum)
    {
        pRAMPart->RealCnt = nSectNum + nSectorCnt;
    }

    if (nSectNum + nSectorCnt >= pRAMPart->SctCnt)
    {
        nSectorCnt = pRAMPart->SctCnt - nSectNum;
        pRAMPart->RealCnt = pRAMPart->SctCnt;
    }

    memcpy((char *)(pRAMPart->BaseAddr + nSectNum * pRAMPart->SctSize), pBuffer, nSectorCnt * pRAMPart->SctSize);
    return nSectorCnt;
}

__s32 rampart_ioctrl(__hdle hPart, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    RAMPartPri *pRAMPart = (RAMPartPri *)(((__fsys_part_t *)hPart)->hPDPrivate);

    /* remove compiler warning */
    Aux  = Aux;

    switch (Cmd)
    {
        case PART_IOC_USR_GETPARTSIZE:
        {
            if (!pBuffer)
            {
                return -1;
            }
            *((__u32 *)pBuffer) = pRAMPart->SctCnt;
        }
        break;

        default:
            break;
    }
    return 0;
}

void *rampart_open(__hdle hPart, void *pArg)
{
    RAMPartPri   *pRAMPart = (RAMPartPri *)pArg;
    RAMPartPri    *pRAMPri = NULL;

    pRAMPri = (RAMPartPri *)calloc(1, sizeof(RAMPartPri));
    if (NULL == pRAMPri)
    {
        return NULL;
    }
    /* 设置分区驱动私有数据块句柄信息 */
    pRAMPri->BaseAddr   = pRAMPart->BaseAddr;
    pRAMPri->SctSize    = pRAMPart->SctSize;
    pRAMPri->SctCnt     = pRAMPart->SctCnt;
    pRAMPri->RealCnt    = pRAMPart->RealCnt;

    return pRAMPri;
}

int rampart_close(__hdle hPart, void *pPri)
{
    if (pPri)
        free(pPri);
    return 0;
}

char *rampart_get_class_name(void)
{
    return RAM_PART_NAME;
}

__u32 rampart_get_real_size(__hdle hPart)
{
    RAMPartPri *pRAMPart = (RAMPartPri *)(hPart);
    return pRAMPart->RealCnt;
}
