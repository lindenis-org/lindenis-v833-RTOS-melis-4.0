#include "basetypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/config_info.h"
#include "script_parser.h"

#include "if/config_if.h"

u32 ReleaseFS(FS_INFO *fsinfo);

u32 __ParserFSConfig(const char *config_file, CFG_SYSTEM *cfg_system, CFG_FSINFO *cfg_fsinfo, CONFIG_IF *cfg_if);

u32 __DumpFSInfo(CFG_SYSTEM *cfg_system, CFG_FSINFO *cfg_fsinfo);

u32 __VerifyFSInfo(CFG_SYSTEM *cfg_system, CFG_FSINFO *cfg_fsinfo);

u32 ParserConfigInfo(const char *config_file,
                     void /*CFG_SYSTEM*/ * cfg_system,
                     void /*CFG_FSINFO*/ * cfg_fsinfo,
                     void *config_if)
{
    u32 ret = OK;

    if (NULL == config_file || NULL == cfg_system || NULL == cfg_fsinfo || NULL == config_if)
    {
        ret = __LINE__;
        goto Err_end;
    }

    //--------------------------------------------------------------------------
    //解析脚本
    //--------------------------------------------------------------------------
    ret = __ParserFSConfig(config_file, (CFG_SYSTEM *)cfg_system, (CFG_FSINFO *)cfg_fsinfo, (CONFIG_IF *)config_if);
    if (OK != ret)
    {
        goto Err_end;
    }

    ret = __DumpFSInfo((CFG_SYSTEM *)cfg_system, (CFG_FSINFO *)cfg_fsinfo);
    if (OK != ret)
    {
        goto Err_end;
    }

    ret = __VerifyFSInfo((CFG_SYSTEM *)cfg_system, (CFG_FSINFO *)cfg_fsinfo);
    if (OK != ret)
    {
        goto Err_end;
    }

    return ret;

    //--------------------------------------------------------------------------
    //出错处理
    //--------------------------------------------------------------------------
Err_end:

    Msg("ParserFSConfig failed %d\n"    , ret);

    return ret;
}

u32 ReleaseConfigInfo(void /*CFG_FSINFO*/ * cfg_fsinfo)
{
    u32 ret = OK;

    if (NULL == cfg_fsinfo)
    {
        return __LINE__;
    }

    u32 i;
    for (i = 0; i < ((CFG_FSINFO *)cfg_fsinfo)->cnt; i++)
    {
        ret = ReleaseFS(&((CFG_FSINFO *)cfg_fsinfo)->fs[i]);
        if (OK != ret)
        {
            return ret;
        }
    }

    ((CFG_FSINFO *)cfg_fsinfo)->cnt = 0;
    free(((CFG_FSINFO *)cfg_fsinfo)->fs);
    ((CFG_FSINFO *)cfg_fsinfo)->fs = NULL;

    return OK;
}

u32 ReleaseFS(FS_INFO *fsinfo)
{
    u32 ret = OK;

    if (NULL == fsinfo)
        return __LINE__;

    free(fsinfo->root_dir.root);

    memset(fsinfo, 0, sizeof(FS_INFO));

    return ret;
}

u32 __ParserFSConfig(const char *config_file, CFG_SYSTEM *cfg_system, CFG_FSINFO *cfg_fsinfo, CONFIG_IF *cfg_if)
{
    u32 ret = OK;
    HCONFIG hConfig = NULL;
    s8 szValue[MAX_VALUE];
    s8 *pSet = NULL;
    s8 *pKey = NULL;
    s8 *pValue = NULL;
    u32 i , j;

    //--------------------------------------------------------------------------
    //打开脚本
    //--------------------------------------------------------------------------
    hConfig = cfg_if->OpenConfig((char *)config_file, NULL);
    if (NULL == hConfig)
    {
        ret = __LINE__;
        goto Err_end;
    }

    pValue = (s8 *)szValue;
    //--------------------------------------------------------------------------
    //system
    //--------------------------------------------------------------------------
    pSet = (s8 *)SET_SYSTEM;

    pKey = (s8 *)KEY_VER;
    memset(szValue, 0, MAX_VALUE);
    ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
    if (OK != ret)
    {
        Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
        goto Err_end;
    }
    cfg_system->ver = atoi((const char *)pValue);

    pKey = (s8 *)KEY_DATE;
    memset(szValue, 0, MAX_VALUE);
    ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
    if (OK != ret)
    {
        Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
        goto Err_end;
    }
    strcpy(cfg_system->date, (const char *)pValue);

    pKey = (s8 *)KEY_ID;
    memset(szValue, 0, MAX_VALUE);
    ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
    if (OK != ret)
    {
        Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
        goto Err_end;
    }
    strcpy(cfg_system->GUID, (const char *)pValue);

    if (0 != strncmp(cfg_system->GUID, KEY_ID_STR, GUID_LEN))
    {
        ret = __LINE__;
        goto Err_end;
    }

    //--------------------------------------------------------------------------
    //fsinfo
    //--------------------------------------------------------------------------
    pSet = (s8 *)SET_FSINFO;

    pKey = (s8 *)KEY_DISCCNT;
    memset(szValue, 0, MAX_VALUE);
    ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
    if (OK != ret)
    {
        Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
        goto Err_end;
    }
    cfg_fsinfo->cnt = atoi((const char *)pValue);
    cfg_fsinfo->fs = (FS_INFO *)malloc(cfg_fsinfo->cnt * sizeof(FS_INFO));
    if (NULL == cfg_fsinfo->fs)
    {
        ret = __LINE__;
        goto Err_end;
    }
    memset(cfg_fsinfo->fs, 0, cfg_fsinfo->cnt * sizeof(FS_INFO));

    //--------------------------------------------------------------------------
    //fs
    //--------------------------------------------------------------------------
    for (i = 0 ; i < cfg_fsinfo->cnt; i++)
    {
        char szKey[MAX_KEY];
        memset(szKey, 0, MAX_KEY);
        sprintf(szKey, "%s%d", KEY_DISCN, i);

        pSet = (s8 *)SET_FSINFO;            //
        pKey = (s8 *)szKey;
        memset(szValue, 0, MAX_VALUE);
        ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
        if (OK != ret)
        {
            Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
            goto Err_end;
        }

        char szFSSet[MAX_SECTION];
        memset(szFSSet, 0, MAX_SECTION);
        strcpy(szFSSet, (const char *)szValue);

        pSet = (s8 *)szFSSet;               //
        FS_INFO *fs = &cfg_fsinfo->fs[i];

        pKey = (s8 *)KEY_DISC;
        memset(szValue, 0, MAX_VALUE);
        ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
        if (OK != ret)
        {
            Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
            goto Err_end;
        }
        strcpy(fs->disc, (const char *)pValue);

        pKey = (s8 *)KEY_FSNAME;
        memset(szValue, 0, MAX_VALUE);
        ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
        if (OK != ret)
        {
            Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
            goto Err_end;
        }
        strcpy(fs->fsname, (const char *)pValue);

        pKey = (s8 *)KEY_FORMAT;
        memset(szValue, 0, MAX_VALUE);
        ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
        if (OK != ret)
        {
            Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
            goto Err_end;
        }
        strcpy(fs->format, (const char *)pValue);

        pKey = (s8 *)KEY_SIZE;
        memset(szValue, 0, MAX_VALUE);
        ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
        if (OK != ret)
        {
            Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
            goto Err_end;
        }
        fs->size = atoi((const char *)pValue) * 2;  //配置文件中的数据以KB为单位 // 转换之后用扇区为单位

        pKey = (s8 *)KEY_ATTR;
        memset(szValue, 0, MAX_VALUE);
        ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
        if (OK != ret)
        {
            Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
            goto Err_end;
        }
        fs->attr = atoi((const char *)pValue);

        pKey = (s8 *)KEY_ROOTCOUNT;
        memset(szValue, 0, MAX_VALUE);
        ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
        if (OK != ret)
        {
            Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
            goto Err_end;
        }
        fs->root_dir.cnt = atoi((const char *)pValue);

        fs->root_dir.root = (DIR_ITEM *)malloc(fs->root_dir.cnt * sizeof(DIR_ITEM));
        if (NULL == fs->root_dir.root)
        {
            ret = __LINE__;
            goto Err_end;
        }
        memset(fs->root_dir.root, 0, fs->root_dir.cnt * sizeof(DIR_ITEM));

        for (j = 0; j < fs->root_dir.cnt; j++)
        {
            char szKey[MAX_KEY];
            sprintf(szKey, "%s%d", KEY_PCROOT, j);
            pKey = (s8 *)szKey;
            memset(szValue, 0, MAX_VALUE);
            ret = cfg_if->GetKeyString(hConfig, pSet, pKey, pValue);
            if (OK != ret)
            {
                Msg("__ParserFSConfig(%s) faild %d\n", pKey, ret);
                goto Err_end;
            }
            strcpy(fs->root_dir.root[j].dir, (const char *)pValue);
        }
    }

    //--------------------------------------------------------------------------
    //关闭脚本
    //--------------------------------------------------------------------------
    cfg_if->CloseConfig(hConfig);
    hConfig = NULL;

    return ret;

    //--------------------------------------------------------------------------
    //出错处理
    //--------------------------------------------------------------------------
Err_end:
    if (NULL != hConfig)
    {
        cfg_if->CloseConfig(hConfig);
        hConfig = NULL;
    }
    Msg("__ParserFSConfig failed %d\n"  , ret);

    return ret;
}

u32 __DumpFSInfo(CFG_SYSTEM *cfg_system, CFG_FSINFO *cfg_fsinfo)
{
    u32 ret = OK;
    u32 i, j;

    if (NULL == cfg_fsinfo)
        return __LINE__;

    //--------------------------------------------------------------------------
    //system
    //--------------------------------------------------------------------------
    Msg("\n");
    Msg("ver=%d\n",     cfg_system->ver);
    Msg("date=%s\n",    cfg_system->date);
    Msg("ID=%s\n",      cfg_system->GUID);
    Msg("\n");

    Msg("fscnt=%d\n",   cfg_fsinfo->cnt);
    //--------------------------------------------------------------------------
    //fs
    //--------------------------------------------------------------------------
    for (i = 0 ; i < cfg_fsinfo->cnt; i++)
    {
        FS_INFO *fs = &cfg_fsinfo->fs[i];

        Msg("disc=%s\n",   fs->disc);
        Msg("fsname=%s\n", fs->fsname);
        Msg("format=%s\n", fs->format);
        Msg("%d KB\n",     fs->size);
        Msg("attr=%x\n",   fs->attr);

        Msg("rootcnt=%d\n", fs->root_dir.cnt);
        for (j = 0; j < fs->root_dir.cnt; j++)
        {
            Msg("root[%d]=%s\n", j, fs->root_dir.root[j].dir);
        }
        Msg("\n");
    }

    return ret;
}

u32 __VerifyFSInfo(CFG_SYSTEM *cfg_system, CFG_FSINFO *cfg_fsinfo)
{
    return OK;
}
