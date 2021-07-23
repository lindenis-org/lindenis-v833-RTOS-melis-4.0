#include "basetypes.h"
#include "include/config_info.h"
#include "if/config_if.h"
#include "if/part_if.h"
#include "if/fstool_if.h"
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#include "filepart.h"
#include "ram_part.h"
#include "fsprocess_inter.h"


FSYS_TOOL_IF *GetInterface();

static CONFIG_IF config_if =
{
    .OpenConfig =  OpenConfig,
    .CloseConfig = CloseConfig,
    .ConfigDump = ConfigDump,
    .GetSectionCount = GetSectionCount,
    .GetFirstSection = GetFirstSection,
    .GetNextSection = GetNextSection,
    .GetSectionKeyCount = GetSectionKeyCount,
    .GetSectionFirstKey = GetSectionFirstKey,
    .GetSectionNextKey = GetSectionNextKey,
    .SetKeyValue = NULL,
    .SetKeyString = NULL,
    .GetKeyValue = GetKeyValue,
    .GetKeyString = GetKeyString,
    .CreateSection = NULL,
    .DeleteSection = NULL,
    .DeleteKey = NULL
};

static PARTITION_IF ram_partition_if =
{
    .PartOpen = rampart_open,
    .PartRead = rampart_read,
    .PartWrite = rampart_write,
    .PartIoctl = rampart_ioctrl,
    .PartClose = rampart_close,
    .GetClassName = rampart_get_class_name,
    .PartGetRealSize = rampart_get_real_size
};

static PARTITION_IF file_partition_if =
{
    .PartOpen = filepart_open,
    .PartRead = filepart_read,
    .PartWrite = filepart_write,
    .PartIoctl = filepart_ioctrl,
    .PartClose = filepart_close,
    .GetClassName = filepart_get_class_name,
    .PartGetRealSize = filepart_get_real_size
};

static FSYS_TOOL_IF *fstool_if;

u32 BuildFileSystem(const char *config_file, u8 *SplitBuf, u32 SplitLen, bool bCutSplit)
{
    u32 ret = OK;
    CFG_SYSTEM  cfg_system;
    CFG_FSINFO  cfg_fsinfo;

    Msg("BuildFileSystem to do ......\n");

    if (NULL == config_file)
        printf("config_file error!\n");

    fstool_if = GetInterface();
    ret = ParserConfigInfo(config_file, (void *)&cfg_system, (void *)&cfg_fsinfo, &config_if);
    if (OK != ret)
    {
        printf("parse config file error!\n");
        return ret;
    }

    /*create file system ; mem filesystem to file ; split file system by splitdata*/
    ret = BuildFileSystemOnly(&cfg_fsinfo, fstool_if, &ram_partition_if, &file_partition_if, SplitBuf, SplitLen, bCutSplit);
    if (OK != ret)
        printf("create file system error!\n");

    return ret;

}
