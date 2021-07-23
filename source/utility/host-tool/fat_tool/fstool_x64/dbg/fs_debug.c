#include "fsys_debug.h"
#include "fsys_libs.h"

debug_para_msg_t debug_para;

int esFSYS_fsdbg(const char *cmd, const char *para)
{
    if ((eLIBs_strncmp(cmd, "log", 3) == 0) || (eLIBs_strncmp(cmd, "LOG", 3) == 0))
    {
        fs_log_parse_option(para);
    }
    else if ((eLIBs_strncmp(cmd, "dump", 4) == 0) || (eLIBs_strncmp(cmd, "DUMP", 4) == 0))
    {
        fs_dump_parse_option(para);
    }
    else if ((eLIBs_strncmp(cmd, "timer", 5) == 0) || (eLIBs_strncmp(cmd, "TIMER", 5) == 0))
    {
        fs_timer_parse_option(para);
    }
    else if ((eLIBs_strncmp(cmd, "fatchunk", 8) == 0) || (eLIBs_strncmp(cmd, "FATCHUNK", 8) == 0))
    {
        __hdle fatfile;

        fatfile = esFSYS_fopen(para, "r");
        if (!fatfile)
        {
            __inf("open file %s fail!\n", para);
        }
        else
        {
//          esFSYS_fioctrl(fatfile, FS_IOC_USR_DEBUG_FATCHUNK, 0, (void*)para);
            esFSYS_fclose(fatfile);
        }
    }
    else if ((eLIBs_strncmp(cmd, "showopenfiles", 13) == 0) || (eLIBs_strncmp(cmd, "SHOWOPENFILES", 13) == 0))
    {
        __debug_dump_openfiles();
    }
    else if ((eLIBs_strncmp(cmd, "fmt.", 4) == 0) || (eLIBs_strncmp(cmd, "FMT.", 4) == 0))
    {
        esFSYS_format(para, cmd + 4, 0);
    }
    else if ((eLIBs_strncmp(cmd, "help", 4) == 0) || (eLIBs_strncmp(cmd, "HELP", 4) == 0))
    {
        __inf("==================================================================================================\n");
        __inf("=                                     Filesystem debuger helper.                                 =\n");
        __inf("=                                          2008.12.30@chenlm                                     =\n");
        __inf("=------------------------------------------------------------------------------------------------=\n");
        __inf("= fs <log>   <[+/-]debug>     : turn on / off debug print                                        =\n");
        __inf("= fs <log>   <[+/-]verbose>   : turn on / off more detail print                                  =\n");
        __inf("= fs <log>   <[+/-]quiet>     : turn on / off a little print                                     =\n");
        __inf("= fs <log>   <[+/-]trace0>    : turn on / off vfs trace print for key functions                  =\n");
        __inf("= fs <log>   <[+/-]trace1>    : turn on / off part trace print for key functions                 =\n");
        __inf("= fs <log>   <[+/-]trace2>    : turn on / off dev trace print for key functions                  =\n");
        __inf("= fs <log>   <[+/-]trace01>   : turn on / off fs trace print for key functions                   =\n");
        __inf("= fs <log>   <[+/-]line>      : turn on / off line print                                         =\n");
        __inf("= fs <log>   <[+/-]objname>   : turn on / off object name print like file name                   =\n");
        __inf("= fs <log>   <[+/-]function>  : turn on / off func name print                                    =\n");
        __inf("= fs <log>   <[+/-]tracef:xxx>: turn on / off and add/del specific func trace                    =\n");
        __inf("= fs <log>   <traceinfo>      : show the state of trace                                          =\n");
        __inf("=                                                                                                =\n");
        __inf("= fs <timer> [on/off]         : turn on/off the timer                                            =\n");
        __inf("= fs <timer> [request]        : get system timer                                                 =\n");
        __inf("= fs <timer> [release]        : release system timer                                             =\n");
        __inf("= fs <timer> [0x--------]     : set timer hdle by 0x--------                                     =\n");
        __inf("=                                                                                                =\n");
        __inf("= fs <dump>  <+dmem:name,offset,len>  : add memory dump item to show mem content (hex)           =\n");
        __inf("= fs <dump>  <-dmem>          : clear all memory dump items                                      =\n");
        __inf("= fs <dump>  <[+/-]sbi>       : turn on / off show sb inode info                                 =\n");
        __inf("= fs <dump>  <[+/-]lrui>      : turn on / off show lru inode info                                =\n");
        __inf("= fs <dump>  <[+/-]sbd>       : turn on / off show sb dentry info                                =\n");
        __inf("= fs <dump>  <[+/-]lrud>      : turn on / off show lru dentry info                               =\n");
        __inf("= fs <dump>  <[+/-]lruf>      : turn on / off show lru and cid fat cache info                    =\n");
        __inf("= fs <dump>  <[+/-]freep>     : turn on / off show free page info                                =\n");
        __inf("= fs <dump>  <[+/-]lrup>      : turn on / off show lru page info                                 =\n");
        __inf("= fs <dump>  <[+/-]bhlist>    : turn on / off show bh list info                                  =\n");
        __inf("= fs <dump>  <[+/-]bhinfo>    : turn on / off show glance of bhs                                 =\n");
        __inf("= fs <dump>  <[+/-]pinfo>     : turn on / off show glance of pages                               =\n");
        __inf("= fs <dump>  <[+/-]allon>     : turn on / off the high priorit dump on-shower                    =\n");
        __inf("= fs <dump>  <[+/-]alloff>    : turn on / off the high priorit dump off-shower                   =\n");
        __inf("= fs <dump>  <[+/-]detail>    : turn on / off show more detail infos for dump                    =\n");
        __inf("=                                                                                                =\n");
        __inf("= fs <fatchunk> <filename>    : show the fat cluster chunks                                      =\n");
        __inf("= fs <showopenfiles>          : show current open files and dirs                                 =\n");
        __inf("=                                                                                                =\n");
        __inf("= fs <ins>   <[fat]/[ntfs]>   : install file system by hand (please wait)                        =\n");
        __inf("= fs <unins> <[fat]/[ntfs]>   : uninstall file system by hand (please wait)                      =\n");
        __inf("= fs <fmt.[fat/ntfs]> <part>  : format the partition by fat or ntfs                              =\n");
        __inf("==================================================================================================\n");
        __inf("\n");
    }
    else
    {
        __inf("invalid input.\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

