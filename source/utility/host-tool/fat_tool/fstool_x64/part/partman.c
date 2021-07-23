#include "err.h"
#include "part.h"
#include "fsys_debug.h"
#include "fsys_libs.h"

__fsys_part_t    *pPDRoot = NULL;
__krnl_event_t   *pPartSem;         /* for lock parts table */

__s32 fsys_vpart_init(void)
{
    pPartSem = esKRNL_SemCreate(1);
    return EPDK_OK;
}

/*
******************************************************************
*
*             fsys_vpart_exit
*
*  Description:
*  API function. Stop the file system.
*
*  Parameters:
*  None.
*
*  Return value:
*  ==0         - File system has been stopped.
*  !=0         - An error has occured.
******************************************************************
*/

__s32 fsys_vpart_exit(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*             esFSYS_findpart
*
*  Description:
*  FSYS internal function. Find correct index in the device information
*  table referred by fsys_pDevInfo for a given fully qualified name.
*
*  Parameters:
*  pFullName   - Fully qualified name.
*
*  Return value:
*  <0          - Unable to find the device.
*  >=0         - Index of the device in the device information table.
**********************************************************************************************************************
*/
__hdle esFSYS_findpart(const char *pFullName)
{
    char    letter;
    char    *s;
    __fsys_part_t *p = pPDRoot;

    /* 获取盘符分隔符 */
    s = eLIBs_strchr((char *)pFullName, ':');
    if (!s || ((int)(s - pFullName)) != 1)
        return 0;

    /* 获取盘符 */
    letter = *pFullName;

    /* 通过盘符查找分区 */
    for (p = pPDRoot; p; p = p->next)
    {
        if (p->letter == letter)
        {
            break;
        }
    }
    return (__hdle)p;
}

/*
******************************************************************
*
*             fsys_regist_part
*
*  Description:
*   注册块设备到文件系统中
*
*  Parameters:
*   pFullName   - Fully qualified name.
*   pFilename   - Address of a pointer, which is modified to point
*                 to the file name part of pFullName.
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information
*                 table.
******************************************************************
*/
__s32 esFSYS_pdreg(__hdle hPD)
{
    __u8              err;
    __fsys_part_t *p, *pd = (__fsys_part_t *)hPD;

    fs_log_trace1("pdreg:");

    esKRNL_SemPend(pPartSem, 0, &err);
    if (err)
    {
        fs_log_error("pdreg err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    /* 判断分区系统是否已经被注册 */
    for (p = pPDRoot; p; p = p->next)
    {
        if (p->letter == pd->letter)
        {
            fs_log_info("pd already registered!\n");
            return EPDK_FAIL;
        }
    }
    /* 将驱动挂接到pPDRoot上                                    */
    pd->next   = pPDRoot;
    pPDRoot     = pd;
    esKRNL_SemPost(pPartSem);

    fs_log_trace1("O\n");
    return EPDK_OK;
}
/*
******************************************************************
*
*             fsys_regist_part
*
*  Description:
*   拔除块设备
*
*  Parameters:
*   pFullName   - Fully qualified name.
*   pFilename   - Address of a pointer, which is modified to point
*                 to the file name part of pFullName.
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information
*                 table.
******************************************************************
*/
__s32 esFSYS_pdunreg(__hdle hPD)
{
    __u8         err;
    __u32        res;
    __fsys_part_t *pd = (__fsys_part_t *)hPD;
    __fsys_part_t *p, **pp = &pPDRoot;

    fs_log_trace1("pdureg:");

    esKRNL_SemPend(pPartSem, 0, &err);
    if (err)
    {
        fs_log_error("pdunreg err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }
    /* seach part first */
    for (p = *pp; p && (pd != p); pp = &(p->next), p = p->next);
    if (!p)
    {
        fs_log_info("BUG when unres pd: try to destroy a fs not exsit"
                    " in pd list!\n");
        res = EPDK_FAIL;
        goto out;
    }
    *pp = pd->next;
    res = EPDK_OK;

out:
    esKRNL_SemPost(pPartSem);

    fs_log_trace1("O%d\n", res);
    return res;
}

