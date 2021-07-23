
#ifndef __FSPROCESS_INTER___CPP____
#define __FSPROCESS_INTER___CPP____
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "basetypes.h"
#include "if/config_if.h"
#include "if/fstool_if.h"
#include "if/part_if.h"

#include "fsprocess_inter.h"
u32 CalcRootFileSize(char *pcdir);

typedef long long __int64;

#define RAM_PART_SCT_SIZE   0X200           // sector size : 512 byte
#define RAM_PART_SCT_BITS   9               // sector size bits : 9

#define BUF_LEN             (8 * 1024)      //

#define VOLUMN              "Volumn"        //


//#define THIS_LINE         Msg("[%d]%s \n", __LINE__, __FILE__);


unsigned long GetFileLength(const char *path)
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if (stat(path, &statbuff) < 0)
    {
        return filesize;
    }
    else
    {
        filesize = statbuff.st_size;
    }
    return filesize;
}


u32 BuildFileSystemOnly(CFG_FSINFO    *cfg_fsinfo,
                        FSYS_TOOL_IF *fstool_if,
                        PARTITION_IF *ram_partition_if,
                        PARTITION_IF *file_partition_if,
                        u8 *SplitBuf, u32 SplitLen, bool bCutSplit)
{
    u32             ret             = OK;
    void           *RAMPartBaseAddr = NULL;
    __fsys_part_t   rampart;

    printf("init\n");
    if (fstool_if == NULL)
    {
        printf("error1\n");
    }
    if (fstool_if->fsys_init == NULL)
    {
        printf("error2\n");
    }

    printf("init2\n");
    ret = fstool_if->fsys_init();
    if (OK != ret)
    {
        printf("fstool_if init fail\n");
        return ret;
    }
    printf("fstool_if init success\n");
    //--------------------------------------------------------------------------
    // 创建文件系统
    //--------------------------------------------------------------------------
    u32 i;
    for (i = 0 ; i < cfg_fsinfo->cnt; i++)
    {
        FS_INFO *fs_info = &cfg_fsinfo->fs[i];
        u32 nTolCnt = 0;
        int n;
        for (n = 0; n < fs_info->root_dir.cnt; n++)
        {
            u32 nTmpCnt = CalcRootFileSize(fs_info->root_dir.root[n].dir);
            nTolCnt += nTmpCnt;
        }
        nTolCnt = (nTolCnt * 10 / 100  + nTolCnt);
        if (nTolCnt > fs_info->size)
        {
            printf("fsbuild error. The size if filesystem must more than %dk  now is %d\n", nTolCnt / 1024, fs_info->size / 1024);
            ret = __LINE__;
            break;
        }


        // Add by Sam Liu On 2010-8-23
        // 解决大容量盘打包的问题
        if (fs_info->size >= 2097152) // 1G
        {
            FilePartConstruct(fs_info, fstool_if, file_partition_if, &rampart);
            ret = FsysConstruct(fs_info, fstool_if, &rampart);
            ret = BuildFS(fstool_if, SplitBuf, SplitLen, bCutSplit, fs_info);
            FsysDestruct(fs_info, fstool_if, &rampart);
            FilePartDestruct(fs_info, fstool_if, file_partition_if, &rampart);
        }
        else
        {

            //--------------------------------------------------------------------------
            //  初始化分区环境, 目前使用RAM分区操作
            //--------------------------------------------------------------------------
            fs_info->size = fs_info->size * 512;
            RAMPartBaseAddr = RAMPartConstruct(fs_info, fstool_if, ram_partition_if, &rampart);
            if (NULL == RAMPartBaseAddr)
            {
                ret = __LINE__;
                break;
            }

            //--------------------------------------------------------------------------
            //  初始化文件系统环境
            //--------------------------------------------------------------------------
            ret = FsysConstruct(fs_info, fstool_if, &rampart);
            if (OK != ret)
            {
                RAMPartDestruct(fs_info, fstool_if, ram_partition_if, RAMPartBaseAddr, &rampart);
                break;
            }

            printf("%d\n", i);
            ret = BuildFS(fstool_if, SplitBuf, SplitLen, bCutSplit, fs_info);
            if (OK != ret)
            {
                goto PT_Fail;
            }

            //realLen = partition_if->PartGetRealSize(rampart.hPDPrivate);
            //--------------------------------------------------------------------------
            //  保存数据到文件(just for debug use)
            //--------------------------------------------------------------------------
            ret = SaveRAMDataToFile(fs_info->fsname, RAMPartBaseAddr, fs_info->size, SplitBuf, SplitLen, bCutSplit);
            if (OK != ret)
            {
                goto PT_Fail;
            }

            FsysDestruct(fs_info, fstool_if, &rampart);
            RAMPartDestruct(fs_info, fstool_if, ram_partition_if, RAMPartBaseAddr, &rampart);
            continue;

PT_Fail:
            FsysDestruct(fs_info, fstool_if, &rampart);
            RAMPartDestruct(fs_info, fstool_if, ram_partition_if, RAMPartBaseAddr, &rampart);
            return ret;
        }

        fstool_if->fsys_exit();
    }


    return ret;
}

u32 BuildFS(FSYS_TOOL_IF *fstool_if, u8 *SplitBuf, u32 SplitLen, bool bCutSplit, FS_INFO *fs)
{
    char disk   = 0;
    u32  ret    = OK;
    u32  i      = 0;

    if (NULL == fstool_if || NULL == fs)
    {
        return __LINE__;
    }

    disk = fs->disc[0];
    Msg("disk : %c\n", disk);
    for (i = 0; i < fs->root_dir.cnt; i++)
    {
        Msg("CopyRootToFS(%s)\n", fs->root_dir.root[i].dir);
        //ret = __CopyRootToFS(fstool_if, disk, fs->root_dir.root[i].dir, fs->root_dir.root[i].dir);
        ret = __CopyRootToFS(fstool_if, disk, "", fs->root_dir.root[i].dir);
        if (OK != ret)
        {
            break;
        }
    }

    //--------------------------------------------------------------------------
    //有magic数据才创建magic.bin文件
    //--------------------------------------------------------------------------
    if (NULL != SplitBuf)
    {
        ret = __CopyMagicToFS(fstool_if, SplitBuf, SplitLen, disk);
        if (OK != ret)
        {
            goto Exit_End;
        }
    }

Exit_End:
    if (OK != ret)
    {
        Msg("BuildFS failed %d\n", ret);
        return ret;
    }

    return ret;
}


void *RAMPartConstruct(FS_INFO *fsinfo,
                       FSYS_TOOL_IF *fstool_if,
                       PARTITION_IF *partition_if,
                       __fsys_part_t *rampart)
{
    RAMPartInfo RamPartInfo;
    char       *part_cname      = NULL;
    u8         *RAMPartBaseAddr = NULL;

    //--------------------------------------------------------------------------
    //  partition_if必须是RAM分区操作接口
    //--------------------------------------------------------------------------
    part_cname = partition_if->GetClassName();
    if (strcmp(RAM_PART_NAME, part_cname))
    {
        return NULL;
    }

    RAMPartBaseAddr = (__u8 *)calloc(fsinfo->size , sizeof(__u8));
    if (NULL == RAMPartBaseAddr)
    {
        return NULL;
    }

    //--------------------------------------------------------------------------
    //  设置RAM分区信息
    //--------------------------------------------------------------------------
    RamPartInfo.BaseAddr    = (u8 *)RAMPartBaseAddr;
    RamPartInfo.SctSize     = RAM_PART_SCT_SIZE;
    RamPartInfo.SctCnt      = fsinfo->size >> RAM_PART_SCT_BITS;
    RamPartInfo.RealCnt = 0;

    //--------------------------------------------------------------------------
    //  设置RAM分区操作、分区信息
    //--------------------------------------------------------------------------
    rampart->letter     = fsinfo->disc[0];
    rampart->Opts.open  = partition_if->PartOpen;
    rampart->Opts.read  = partition_if->PartRead;
    rampart->Opts.write = partition_if->PartWrite;
    rampart->Opts.ioctl = partition_if->PartIoctl;
    rampart->Opts.close = partition_if->PartClose;
    rampart->hPDPrivate = NULL;
    rampart->hFSPrivate = NULL;
    rampart->next       = NULL;

    //--------------------------------------------------------------------------
    //  打开RAM分区，设置RAM分区私有句柄(暂时在这里打开)
    //--------------------------------------------------------------------------
    rampart->hPDPrivate = partition_if->PartOpen((__hdle)(rampart), (__hdle)(&RamPartInfo));
    if (rampart->hPDPrivate == NULL)
    {
        free(RAMPartBaseAddr);
        return NULL;
    }

    //--------------------------------------------------------------------------
    //  注册RAM分区
    //--------------------------------------------------------------------------
    fstool_if->pdreg(rampart);

    return RAMPartBaseAddr;
}



u32 RAMPartDestruct(FS_INFO *fsinfo,
                    FSYS_TOOL_IF *fstool_if,
                    PARTITION_IF *partition_if,
                    void *BaseAddress,
                    __fsys_part_t *rampart)
{
    //--------------------------------------------------------------------------
    //  注销RAM分区
    //--------------------------------------------------------------------------
    fstool_if->pdunreg(rampart);

    //--------------------------------------------------------------------------
    //  关闭RAM分区
    //--------------------------------------------------------------------------
    partition_if->PartClose((__hdle)(rampart), rampart->hPDPrivate);

    //--------------------------------------------------------------------------
    //  销毁RAM分区其他数据结构
    //--------------------------------------------------------------------------
    rampart->letter     = 0;
    rampart->Opts.open  = NULL;
    rampart->Opts.read  = NULL;
    rampart->Opts.write = NULL;
    rampart->Opts.ioctl = NULL;
    rampart->Opts.close = NULL;
    rampart->hPDPrivate = NULL;
    rampart->hFSPrivate = NULL;
    rampart->next       = NULL;

    free(BaseAddress);

    return 0;
}

u32 FsysConstruct(FS_INFO *fsinfo, FSYS_TOOL_IF *fstool_if, __fsys_part_t *rampart)
{
    u32        ret = OK;
    ES_FMTPARA fmtpara;
    char       fsmaintype[8];
    char       path[12];
    bool       IsFAT = false;

    //--------------------------------------------------------------------------
    //  格式化分区
    //--------------------------------------------------------------------------
    memset(&fmtpara, 0, sizeof(fmtpara));
    if (0 == strcmp(fsinfo->format, "fat12"))
    {
        fmtpara.subtype = T_FAT12;
        IsFAT = true;

    }
    else if (0 == strcmp(fsinfo->format, "fat16"))
    {
        fmtpara.subtype = T_FAT16;
        IsFAT = true;
    }
    else if (0 == strcmp(fsinfo->format, "fat32"))
    {
        fmtpara.subtype = T_FAT32;
        IsFAT = true;
    }
    else
    {
        //--------------------------------------------------------------------------
        //  默认文件系统格式由格式化工具根据容量匹配
        //--------------------------------------------------------------------------
        fmtpara.subtype = 0;
    }

    if (!IsFAT)
    {
        Msg("Only support FAT file system now !\n");
        return __LINE__;
    }
    sprintf(fsmaintype, "%s", "fat");

    sprintf(path, "%c:\\", fsinfo->disc[0]);
    strcpy(fmtpara.label, VOLUMN);
    ret = fstool_if->format(path, fsmaintype, &fmtpara);
    if (0 != ret)
    {
        return ret;
    }

    //--------------------------------------------------------------------------
    //  挂载RAM分区文件系统
    //--------------------------------------------------------------------------
    return fstool_if->mntfs(rampart);
}


u32 FsysDestruct(FS_INFO *fsinfo, FSYS_TOOL_IF *fstool_if, __fsys_part_t *rampart)
{

    //--------------------------------------------------------------------------
    //  卸载RAM分区文件系统(强制卸载)
    //--------------------------------------------------------------------------
    return fstool_if->umntfs(rampart, 1);
}

u32 __CopyMagicToFS(FSYS_TOOL_IF *fstool_if, u8 *SplitBuf, u32 SplitLen, char disk)
{

    u32     ret = OK;
    char    fs_file[MAX_PATH];
    __hdle  hFile = NULL;

    memset(fs_file, 0, MAX_PATH);
    sprintf(fs_file, "%c:\\magic.bin", disk);
    Msg("%s\n", fs_file);

    hFile = fstool_if->fopen(fs_file, "wb+");
    if (NULL == hFile)
    {
        return  __LINE__;
    }
    fstool_if->fwrite(SplitBuf, SplitLen, 1, hFile);

    fstool_if->fclose(hFile);
    hFile = NULL;

    return ret;
}

u32 __CopyRootToFS(FSYS_TOOL_IF *fstool_if, char disk, char *melis_parent_dir, char *pDir)
{
    u32 ret = OK;
    char path[512];
    struct dirent *filename;//readdir 的返回类型
    DIR *dir;//血的教训阿，不要随便把变量就设成全局变量。。。。
    dir = opendir(pDir);
    char melis_dir[512];
    if (dir == NULL)
    {
        printf("open dir %s error!\n", pDir);
        exit(1);
    }

    while ((filename = readdir(dir)) != NULL)
    {
        //目录结构下面问什么会有两个.和..的目录？ 跳过着两个目录
        if (!strcmp(filename->d_name, ".") || !strcmp(filename->d_name, ".."))
            continue;

        //非常好用的一个函数，比什么字符串拼接什么的来的快的多
        sprintf(path, "%s/%s", pDir, filename->d_name);
        sprintf(melis_dir, "%s\\%s", melis_parent_dir, filename->d_name);

        struct stat s;
        lstat(path, &s);
        if (S_ISDIR(s.st_mode))
        {
            if (0 == strcmp(filename->d_name, ".svn"))      //不加入文件系统
            {
                continue;
            }

            u32 __MakeDirToFSNew(FSYS_TOOL_IF *fstool_if, char disk, char *melis_dir, char *pcdir);
            __MakeDirToFSNew(fstool_if, disk, melis_dir, path);
            ret = __CopyRootToFS(fstool_if, disk, melis_dir, path);
            if (OK != ret)
            {
                Msg("Failed:%s %d \n", path, ret);
                break;
            }
        }
        else if (S_ISREG(s.st_mode))
        {
            if (0 == strcmp("Thumbs.db", filename->d_name)) //不加入文件系统
            {
                continue;
            }
            ret = __CopyFileToFS(fstool_if, disk, melis_dir, path);
            if (OK != ret)
            {
                Msg("Failed:%s %d \n", path, ret);
                break;
            }
        }
    }
    closedir(dir);
    return ret;
}

u32 __MakeDirToFSNew(FSYS_TOOL_IF *fstool_if, char disk, char *melis_dir, char *pcdir)
{
    u32 ret = OK;
    char fs_dir[MAX_PATH];

    memset(fs_dir, 0, MAX_PATH);
    sprintf(fs_dir, "%c:%s", disk, melis_dir);
    Msg("\n%s\n", fs_dir);

    fstool_if->mkdir((const char *)fs_dir);

    return ret;
}

u32 __MakeDirToFS(FSYS_TOOL_IF *fstool_if, char disk, char *root, char *pcdir)
{
    u32 ret = OK;
    char fs_dir[MAX_PATH];

    memset(fs_dir, 0, MAX_PATH);
    sprintf(fs_dir, "%c:%s", disk, pcdir + strlen(root));
    Msg("\n%s\n", fs_dir);

    fstool_if->mkdir((const char *)fs_dir);

    return ret;
}

u32 __CopyFileToFS(FSYS_TOOL_IF *fstool_if, char disk, char *root, char *pcfile)
{
    char    fs_file[MAX_PATH];
    u8      buffer[BUF_LEN];
    u32     ret         = OK;
    __hdle  hFile       = NULL;
    __int64     filelen     = 0;
    FILE  *fp          = NULL;
    __int64     total_read  = 0;

#ifdef HAS_DATA_CHECK
    u32     pc_sum      = 0;
    u32     fs_sum      = 0;
#endif //HAS_DATA_CHECK

    //--------------------------------------------------------------------------
    //生成文件路径
    //--------------------------------------------------------------------------
    memset(fs_file, 0, MAX_PATH);
    //sprintf(fs_file, "%c:%s", disk, pcfile + strlen(root));
    sprintf(fs_file, "%c:%s", disk, root);
    //--------------------------------------------------------------------------
    //获取文件原始长度
    //--------------------------------------------------------------------------
    filelen = GetFileLength(pcfile);
    if (0 == filelen)
    {
        //return __LINE__;
        return OK;
    }

#ifdef HAS_DATA_CHECK
    //--------------------------------------------------------------------------
    //原始文件校验和
    //--------------------------------------------------------------------------
    pc_sum = PCFileAddSum(pcfile, filelen);
#endif

    //--------------------------------------------------------------------------
    //打开pc上的文件
    //--------------------------------------------------------------------------
    fp = fopen(pcfile, "rb");
    if (NULL == fp)
    {
        goto Err_End;
    }

    //--------------------------------------------------------------------------
    //创建文件
    //--------------------------------------------------------------------------
    hFile = fstool_if->fopen(fs_file, "wb+");
    if (NULL == hFile)
    {
        printf("空间不够请修改文件系统打包配置参数\n");
        ret =  __LINE__;
        goto Err_End;
    }
    //--------------------------------------------------------------------------
    //文件数据复制
    //--------------------------------------------------------------------------
    while (total_read < filelen)
    {
        __int64 this_read = BUF_LEN > (filelen - total_read) ? (filelen - total_read) : BUF_LEN;
        //__int64 this_read = min(BUF_LEN, filelen - total_read);
        fread(buffer, this_read, 1, fp);

        fstool_if->fwrite(buffer, this_read, 1, hFile);

        total_read += this_read;
    }
    //--------------------------------------------------------------------------
    //文件关闭
    //--------------------------------------------------------------------------
    fstool_if->fclose(hFile);
    hFile = NULL;

    fclose(fp);
    fp = NULL;
    //--------------------------------------------------------------------------
    //文件校验和
    //--------------------------------------------------------------------------
#ifdef HAS_DATA_CHECK
    fs_sum = FSFileAddSum(fstool_if, fs_file, filelen);
    if (pc_sum != 0)
    {
        if (pc_sum != fs_sum)
        {
            Msg("Err:%s 校验和不匹配!", fs_file);
        }
        else
        {
            Msg("%s\n", fs_file);
        }
    }
    else
    {
        Msg("Err: %s failed !\n", fs_file);
    }
#endif //HAS_DATA_CHECK

    return ret;

    //--------------------------------------------------------------------------
    //出错处理
    //--------------------------------------------------------------------------
Err_End:
    if (NULL != hFile)
    {
        fstool_if->fclose(hFile);
        hFile = NULL;
    }

    if (NULL != fp)
    {
        fclose(fp);
        fp = NULL;
    }
    Msg("Err: %s failed %d !\n", fs_file, ret);
    return ret;
}

#ifdef HAS_DATA_CHECK
u32 AddSum(u8 *buffer, u32 buflen, u32 init_sum)
{
    u32 i   = 0;
    u32 sum = init_sum;

    for (i = 0; i < buflen; i++)
    {
        sum += buffer[i];
    }

    return sum;
}
#endif //HAS_DATA_CHECK

#ifdef HAS_DATA_CHECK
u32 PCFileAddSum(char *pc_file, u32 length)
{
    u8      buffer[BUF_LEN];
    u32     buflen      = BUF_LEN;
    u32     ret         = OK;
    u32     filelen     = 0;
    FILE  *fp          = NULL;
    u32     total_read  = 0;
    u32     sum         = 0 ;
    //--------------------------------------------------------------------------
    //打开pc上的文件
    //--------------------------------------------------------------------------
    fp = fopen(pc_file, "rb");
    if (NULL == fp)
    {
        return 0;
    }

    filelen = length;
    sum     = 0;
    //--------------------------------------------------------------------------
    //文件数据复制
    //--------------------------------------------------------------------------
    while (total_read < filelen)
    {
        //u32 this_read = min(BUF_LEN, filelen - total_read);

        u32 this_read = BUF_LEN > (filelen - total_read) ? (filelen - total_read) : BUF_LEN;
        fread(buffer, this_read, 1, fp);

        sum = AddSum(buffer, this_read, sum);

        total_read += this_read;
    }
    //--------------------------------------------------------------------------
    //文件关闭
    //--------------------------------------------------------------------------
    fclose(fp);
    fp = NULL;

    return sum;
}
#endif //HAS_DATA_CHECK

#ifdef HAS_DATA_CHECK
u32 FSFileAddSum(FSYS_TOOL_IF *fstool_if, char *fs_file, u32 length)
{
    u32 ret = OK;
    __hdle hFile = NULL;
    u8  buffer[BUF_LEN];
    u32 filelen = 0;
    u32 total_read = 0;
    u32 sum = 0;
    //--------------------------------------------------------------------------
    //创建文件
    //--------------------------------------------------------------------------
    hFile = fstool_if->fopen(fs_file, "rb");
    if (NULL == hFile)
    {
        printf("FS_fopen %s failed\n", fs_file);
        return 0;
    }
    filelen = length;
    sum = 0;
    //--------------------------------------------------------------------------
    //文件数据复制
    //--------------------------------------------------------------------------
    total_read = 0;
    while (total_read < filelen)
    {
        //u32 this_read = min(BUF_LEN, filelen - total_read);
        u32 this_read = BUF_LEN > (filelen - total_read) ? (filelen - total_read) : BUF_LEN;

        fstool_if->fread(buffer, this_read, 1, hFile);

        sum = AddSum(buffer, this_read, sum);

        total_read += this_read;
    }
    //--------------------------------------------------------------------------
    //文件关闭
    //--------------------------------------------------------------------------
    fstool_if->fclose(hFile);
    hFile = NULL;

    return sum;
}
#endif //HAS_DATA_CHECK


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//  mode=0; 在分割文件后在擦除magic.c文件为0
//  mode=1; 在分割文件后不用擦除magic.c文件
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//
//
//------------------------------------------------------------------------------------------------------------
/*
u32 __CutSplitData(const char *filepath, u8 * SplitBuf, u32 SplitLen, bool bCutSplit)
{
    FILE *fd, *fsplit;
    char *buff;
    u32 len, i, j, outputlen;
    char destfile[MAX_PATH];

    memset(destfile, 0, MAX_PATH);
    memcpy(destfile, filepath, strlen(filepath));
    strcat(destfile, "_tmp");

    fd      = fopen(filepath, "rb");
    fsplit  = fopen(destfile, "wb");
    outputlen = 0;

    buff = (char *)calloc(sizeof(char), SIZE_1M);
    while(len= fread(buff, sizeof(char), SIZE_1M, fd))
    {
        i=4;
        do{ //查找maigc最开始5个字节
            if( (buff[i]== SplitBuf[4])&&
                (buff[i-1] == SplitBuf[3])&& (buff[i-2] == SplitBuf[2])&&
                (buff[i-3] == SplitBuf[1])&& (buff[i-4] == SplitBuf[0]))
            {
                for(j = 5; j < SplitLen; j++)
                {
                    if(SplitBuf[j] != buff[i+j-4])
                    {
                        break;
                    }
                }

                if(j == SplitLen)   //找到magic
                {
                    if(bCutSplit)
                    {
                        memset(buff+i-4, 0, j);
                        fwrite(buff, sizeof(char), i+j-4, fsplit);
                    }
                    else
                    {
                        fwrite(buff, sizeof(char), i+j-4, fsplit);
                    }
                    goto findfile;
                }
            }
            i++;
        }while(i<len);

        fwrite(buff, sizeof(char), len, fsplit);
        memset(buff, 0, len);
    }

findfile:
    free(buff);
    fclose(fd);
    fclose(fsplit);
    remove(filepath);
    rename(destfile, filepath);

    return OK;
}
*/

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//  bCutSplit=1; 在分割文件后在擦除magic.c文件为0
//  bCutSplit=0; 在分割文件后不用擦除magic.c文件
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//
//
//------------------------------------------------------------------------------------------------------------
u32 SaveRAMDataToFile(const char *filename, void *buffer, u32 Len, u8 *SplitBuf, u32 SplitLen, bool bCutSplit)
{
    FILE  *fp      = NULL;
    u32     i       = 0;
    u8     *pbuf    = (u8 *) buffer;
    u32     RealLen = 0;
    u32     Left_Len = 0;
    u32     this_len = 0;
    u32     offset  = 0;

    if (NULL == buffer || NULL == filename)
        return __LINE__;

    if (NULL != SplitBuf)
    {
        for (i = 0 ; i < Len ; i++)
        {
            //查找maigc最开始4个字节
            if (0 == memcmp(&pbuf[i], SplitBuf, 4))
            {
                if (0 == memcmp(&pbuf[i], SplitBuf, SplitLen))  //找到magic
                {
                    Msg("find magic !! \n");
                    if (bCutSplit)
                    {
                        memset(&pbuf[i], 0, SplitLen);          //split数据清零
                    }
                    RealLen = i + SplitLen;                     //实际长度
                }
            }
        }
    }
    else
    {
        RealLen = Len;
    }
    Msg("RealLen=0x%X\n", RealLen);

    fp = fopen(filename, "wb+");
    if (NULL == fp)
        return __LINE__;

    offset  = 0;
    Left_Len = RealLen;
    while (Left_Len > 0)
    {
#define SIZE_32K 0x8000
        //this_len = min(SIZE_32K, Left_Len);
        this_len = (SIZE_32K > Left_Len) ? Left_Len : SIZE_32K;
        if (1 != fwrite(pbuf + offset, this_len, 1, fp))
        {
            fclose(fp);
            return __LINE__;
        }

        Left_Len -= this_len;
        offset += this_len;
    }
    fclose(fp);
    fp = NULL;

    return OK;
}


u32 CalcRootFileSize(char *pDir)
{
    u32 ret = 0;
    u32 nRetLen = 0;

    char path[512];
    struct dirent *filename;//readdir 的返回类型
    DIR *dir;//血的教训阿，不要随便把变量就设成全局变量。。。。
    dir = opendir(pDir);
    if (dir == NULL)
    {
        printf("open dir %s error!\n", pDir);
        exit(1);
    }

    while ((filename = readdir(dir)) != NULL)
    {
        //目录结构下面问什么会有两个.和..的目录？ 跳过着两个目录
        if (!strcmp(filename->d_name, ".") || !strcmp(filename->d_name, ".."))
            continue;

        //非常好用的一个函数，比什么字符串拼接什么的来的快的多
        sprintf(path, "%s/%s", pDir, filename->d_name);

        struct stat s;
        lstat(path, &s);
        if (S_ISDIR(s.st_mode))
        {
            if (0 == strcmp(filename->d_name, ".svn"))      //不加入文件系统
            {
                continue;
            }
            u32 nTmp = CalcRootFileSize(path);
            nRetLen += nTmp;
            if (0 == nTmp)
            {
                Msg("失败:%s %d \n", path, ret);
                break;
            }
        }
        else if (S_ISREG(s.st_mode))
        {
            if (0 == strcmp("Thumbs.db", filename->d_name)) //不加入文件系统
            {
                continue;
            }
            __int64 nTmp = GetFileLength(path);
            nTmp = (nTmp + 511) / 512 ;
            nRetLen += nTmp;
            if (0 == nTmp)
            {
                Msg("失败:%s %d \n", path, ret);
                break;
            }
        }
    }
    closedir(dir);

    return nRetLen ;
}

//////////////////////////////////////////////////////////////////////////
// Add by Sam Liu On 2010-8-23
//////////////////////////////////////////////////////////////////////////
void *FilePartConstruct(FS_INFO *fsinfo,
                        FSYS_TOOL_IF *fstool_if,
                        PARTITION_IF *partition_if,
                        __fsys_part_t *filepart)
{
    FILE_PartInfo   filePartInfo;
    char       *part_cname      = NULL;

    //--------------------------------------------------------------------------
    //  partition_if必须是RAM分区操作接口
    //--------------------------------------------------------------------------
    part_cname = partition_if->GetClassName();
    if (strcmp(FILE_PART_NAME, part_cname))
    {
        return NULL;
    }

    //--------------------------------------------------------------------------
    //  设置RAM分区信息
    //--------------------------------------------------------------------------
    filePartInfo.pFile = NULL;
    filePartInfo.nSectSize = RAM_PART_SCT_SIZE;
    filePartInfo.nSecCnt = fsinfo->size;//>>RAM_PART_SCT_BITS;
    strcpy(filePartInfo.szFileName, fsinfo->fsname);

    //--------------------------------------------------------------------------
    //  设置RAM分区操作、分区信息
    //--------------------------------------------------------------------------
    filepart->letter        = fsinfo->disc[0];
    filepart->Opts.open = partition_if->PartOpen;
    filepart->Opts.read = partition_if->PartRead;
    filepart->Opts.write    = partition_if->PartWrite;
    filepart->Opts.ioctl    = partition_if->PartIoctl;
    filepart->Opts.close    = partition_if->PartClose;
    filepart->hPDPrivate    = NULL;
    filepart->hFSPrivate    = NULL;
    filepart->next      = NULL;

    //--------------------------------------------------------------------------
    //  打开RAM分区，设置RAM分区私有句柄(暂时在这里打开)
    //--------------------------------------------------------------------------
    filepart->hPDPrivate = partition_if->PartOpen((__hdle)(filepart), (__hdle)(&filePartInfo));
    if (filepart->hPDPrivate == NULL)
    {
        return NULL;
    }
    //--------------------------------------------------------------------------
    //  注册File分区
    //--------------------------------------------------------------------------
    fstool_if->pdreg(filepart);

    return filepart->hPDPrivate;
}

u32 FilePartDestruct(FS_INFO *fsinfo,
                     FSYS_TOOL_IF *fstool_if,
                     PARTITION_IF *partition_if,
                     __fsys_part_t *filepart)
{
    //--------------------------------------------------------------------------
    //  注销FILE分区
    //--------------------------------------------------------------------------
    fstool_if->pdunreg(filepart);

    //--------------------------------------------------------------------------
    //  关闭FILE分区
    //--------------------------------------------------------------------------
    partition_if->PartClose((__hdle)(filepart), filepart->hPDPrivate);

    //--------------------------------------------------------------------------
    //  销毁FILE分区其他数据结构
    //--------------------------------------------------------------------------
    filepart->letter        = 0;
    filepart->Opts.open = NULL;
    filepart->Opts.read = NULL;
    filepart->Opts.write    = NULL;
    filepart->Opts.ioctl    = NULL;
    filepart->Opts.close    = NULL;
    filepart->hPDPrivate    = NULL;
    filepart->hFSPrivate    = NULL;
    filepart->next      = NULL;

    return 0;
}
#endif //__FSPROCESS_INTER___CPP____
