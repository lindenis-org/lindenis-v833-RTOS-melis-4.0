/*
**********************************************************************************************************************
*                                                   ePOS
*                                  the Easy Portable/Player Operation System
*                                              fsys sub-system
*                                               vfs layer(vfs + vpart)
*
*                                (c) Copyright 2007-2008, Steven.ZGJ.China
*                                           All Rights Reserved
*
* File    : part.c
* By      : steven.ZGJ
* Version : V1.00
**********************************************************************************************************************
*/

#include "part.h"
#include "fsys_debug.h"

/*
**********************************************************************************************************************
*
*            esFRead
*
*  Description:
*  API function. read data from a file.
*
*  Parameters:
*  pdata    - Pointer to a data buffer for storing data transferred from file
*  size     - Size of an element to be transferred from file to data buffer
*  n        - Number of elements to be transferred from the file
*  fp       - Pointer to a __fp data structure.
*
*  Return value:
*  Number of elements read.
**********************************************************************************************************************
*/
__u32 esFSYS_pread(void *pData, __u32 sector, __u32 n, __hdle hPart)
{
    __u32           res;
    __fsys_part_t *pPart = (__fsys_part_t *)hPart;

    fs_log_trace1("part read:");
    if (pPart && pPart->Opts.read)
    {
        res = pPart->Opts.read(pData, sector, n, pPart);
        fs_log_trace1("R%ds,F%d,", n, sector);
    }
    else
    {
        fs_log_error("part has no read opers, fail\n");
        res = 0;
    }
    fs_log_trace1("O\n");
    return res;
}
/*
**********************************************************************************************************************
*
*            esFWrite
*
*  Description:
*       API function. write data to a file.
*
*  Parameters:
*       pdata    - Pointer to a data  to be written to a file
*       size     - Size of an element to be transferred.
*       n        - Number of elements to be transferred to the file
*       fp       - Pointer to a __fp data structure.
*
*  Return value:
*       Number of elements written.
**********************************************************************************************************************
*/
__u32 esFSYS_pwrite(const void *pData, __u32 Sector, __u32 N, __hdle hPart)
{
    __u32           res;
    __fsys_part_t *pPart = (__fsys_part_t *)hPart;

    fs_log_trace1("part write:");
    if (pPart && pPart->Opts.write)
    {
        res = pPart->Opts.write(pData, Sector, N, pPart);
        fs_log_trace1("W%ds,T%d,", N, Sector);
    }
    else
    {
        fs_log_error("part has no write opers, fail\n");
        res = 0;
    }
    fs_log_trace1("O\n");
    return res;
}
/*
**************************************************************************************************
*
*             FS_IoCtl
*
  Description:
  API function. Execute device command.

  Parameters:
  pDevName    - Fully qualified directory name.
  Cmd         - Command to be executed.
  Aux         - Parameter depending on command.
  pBuffer     - Pointer to a buffer used for the command.

  Return value:
  Command specific. In general a negative value means an error.
**************************************************************************************************
*/

__s32 esFSYS_pioctrl(__hdle hPart, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32           res;
    __fsys_part_t *pPart = (__fsys_part_t *)hPart;

    fs_log_trace1("pctl:");
    if (IS_PARTIOCSYS(cmd))
    {
        switch (cmd)
        {

            case PART_IOC_SYS_GETLETTER:
                *((char *)pbuffer) = pPart->letter;
                res = EPDK_OK;
                break;

            case PART_IOC_SYS_GETFSPRIV:
                *((int *)pbuffer) = (int)pPart->hFSPrivate;
                res = EPDK_OK;
                break;

            case PART_IOC_SYS_SETFSPRIV:
                pPart->hFSPrivate = (__hdle)aux;
                res = EPDK_OK;
                break;

            default:
                res = EPDK_FAIL;
                break;
        }

        goto out;
    }
    else
        res = pPart->Opts.ioctl(pPart, cmd, aux, pbuffer);

out:
    fs_log_trace1("O%d\n", res);
    return res;
}


