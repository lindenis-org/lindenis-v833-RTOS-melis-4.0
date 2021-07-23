#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basetypes.h"
#include "if/fstool_if.h"
#include "if/part_if.h"
#include "if/Part_File_Plugin_if.h"


#define     PLUGINAPI   extern "C" __declspec(dllexport)

#define PLUGIN_TYPE         PART_FILE_PLUGIN_TYPE           //
#define PLUGIN_NAME         "filepart"                  //
#define PLUGIN_VERSION          0x0100
#define PLUGIN_AUTHOR           "Sam Liu"
#define PLUGIN_COPYRIGHT        "Sam Liu"

typedef long long __int64;

__u32 filepart_read(void *pBuffer, __u32 nSectNum, __u32 nSectorCnt, __hdle hPart)
{
    FILE_PartInfo *pFilePart = (FILE_PartInfo *)(((__fsys_part_t *)hPart)->hPDPrivate);
    fseek(pFilePart->pFile, nSectNum * pFilePart->nSectSize, SEEK_SET);
    return fread(pBuffer, nSectorCnt * pFilePart->nSectSize, 1, pFilePart->pFile);


    //SetFilePtEx(pFilePart->pFile, nSectNum);
    //fwrite(pBuffer, nSectorCnt*pFilePart->nSectSize, 1, pFilePart->pFile);
    //DWORD dwLen;
    //__u32 nLen = ReadFile(pFilePart->pFile, pBuffer, nSectorCnt*pFilePart->nSectSize, &dwLen, NULL);
    //return (dwLen / pFilePart->nSectSize);
}

__u32 filepart_write(const void *pBuffer, __u32 nSectNum, __u32 nSectorCnt, __hdle hPart)
{
    FILE_PartInfo *pFilePart = (FILE_PartInfo *)(((__fsys_part_t *)hPart)->hPDPrivate);

    if (nSectNum >= 4194304)
    {
        nSectNum = nSectNum;
    }

    fseek(pFilePart->pFile, nSectNum * pFilePart->nSectSize, SEEK_SET);

    //SetFilePtEx(pFilePart->pFile, nSectNum);
    return fwrite(pBuffer, nSectorCnt * pFilePart->nSectSize, 1, pFilePart->pFile);
    //DWORD dwLen;
    //__u32 nLen = WriteFile(pFilePart->pFile, pBuffer, nSectorCnt*pFilePart->nSectSize, &dwLen, NULL);

    //return (dwLen / pFilePart->nSectSize);
}

__s32 filepart_ioctrl(__hdle hPart, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    FILE_PartInfo *pRAMPart = (FILE_PartInfo *)(((__fsys_part_t *)hPart)->hPDPrivate);

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
            *((__u32 *)pBuffer) = pRAMPart->nSecCnt;
            printf("FilePart size = %d\n", *((__u32 *)pBuffer));
        }
        break;

        default:
            break;
    }
    return 0;
}

void *filepart_open(__hdle hPart, void *pArg)
{
    FILE_PartInfo *pPart = (FILE_PartInfo *)pArg;
    FILE_PartInfo *pRetPart = (FILE_PartInfo *)malloc(sizeof(FILE_PartInfo));

    memset(pRetPart, 0x00, sizeof(FILE_PartInfo));

#if 0
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle =  0;
    pRetPart->pFile = CreateFile(pPart->szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
    pRetPart->pFile = fopen(pPart->szFileName, "wb+");
    strcpy(pRetPart->szFileName, pPart->szFileName);
    pRetPart->nSectSize = pPart->nSectSize;
    pRetPart->nSecCnt = pPart->nSecCnt;

//  fseek(pRetPart->pFile, pRetPart->nSecCnt * 512 - 4, SEEK_SET);
//  fwrite("end", 3, 1, pRetPart->pFile);

    return (__hdle)pRetPart;
}

int filepart_close(__hdle hPart, void *pPri)
{
    FILE_PartInfo *pPart = (FILE_PartInfo *)pPri;

    fclose(pPart->pFile);
    free(pPart);
    return 0;
}

char *filepart_get_class_name(void)
{
    return FILE_PART_NAME;
}

__u32 filepart_get_real_size(__hdle hPart)
{
    FILE_PartInfo *pPart = (FILE_PartInfo *)(hPart);
    __u32 filelength = 0;
    fseek(pPart->pFile, 0, SEEK_END);
    filelength = ftell(pPart->pFile);

    return filelength;
}
