// FaceDocument.cpp: implementation of the FaceDocument class.
//
//////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FaceDocument.h"
#include "error.h"
#include "basetypes.h"
#include <dlfcn.h>
#include "PlugInCenter/plugin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "face_document_config.h"                   //

#define WORK_FACE_SCRIPT    "face_script.work"      //当前script
#define WORD_FACE_BIN       "face_bin.work"         //当前模版资源bin

#define FACE_DIR            "face"                  //
#define FACE_DOC_CFG        "face.document"         //

extern void *GetScriptParserInterface();
extern void *GetConfigInterface();

int IsDirExist(const char *pathname)
{
    if (pathname == NULL)
        return -1;
    if (opendir(pathname) == NULL)
        return -1;
    return 0;
}


int removedir(const char *dir)
{
    char cur_dir[] = ".";
    char up_dir[] = "..";
    char dir_name[128];
    DIR *dirp;
    struct dirent *dp;
    struct stat dir_stat;

    // 参数传递进来的目录不存在，直接返回
    if (0 != access(dir, F_OK))
    {
        return 0;
    }

    // 获取目录属性失败，返回错误
    if (0 > stat(dir, &dir_stat))
    {
        perror("get directory stat error");
        return -1;
    }

    if (S_ISREG(dir_stat.st_mode))      // 普通文件直接删除
    {
        remove(dir);
    }
    else if (S_ISDIR(dir_stat.st_mode))         // 目录文件，递归删除目录中内容
    {
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != NULL)
        {
            // 忽略 . 和 ..
            if ((0 == strcmp(cur_dir, dp->d_name)) || (0 == strcmp(up_dir, dp->d_name)))
            {
                continue;
            }

            sprintf(dir_name, "%s/%s", dir, dp->d_name);
            removedir(dir_name);   // 递归调用
        }
        closedir(dirp);

        rmdir(dir);     // 删除空目录
    }
    else
    {
        perror("unknow file type!");
    }

    return 0;
}

int IsFileExist(char *pathname)
{
    if (pathname == NULL)
        return -1;
    if (access(pathname, F_OK) == 0)
        return 0;
    return -1;
}

int makedir(char *path)
{
    return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
FaceDocument::FaceDocument()
{
    memset(m_szModuleDir,       0, MAX_PATH);

    memset(m_FaceDocumentFile,  0, MAX_PATH);

    memset(m_FaceDir,           0, MAX_PATH);

    memset(m_FaceDocumentCfg,   0, MAX_PATH);

    memset(m_FaceScriptFile,    0, MAX_PATH);

    memset(m_FaceTemplate,      0, MAX_PATH);

    m_FaceScriptData = NULL;
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
FaceDocument::~FaceDocument()
{
    DeleteFaceTempFile();

    removedir(m_FaceDir);

    m_FaceScriptData = NULL;
}


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------

void FaceDocument::Initial()
{
    u32 ret = OK;

    //GetModuleDirectory(m_szModuleDir);
    getcwd(m_szModuleDir, MAX_PATH);

    sprintf(m_FaceDir, "%s/%s", m_szModuleDir, FACE_DIR);

    if (OK != IsDirExist(m_FaceDir))
    {
        u32 ret = OK;

        ret = makedir(m_FaceDir);

        if (OK != ret)
        {
            Err("FaceDocument", __FILE__, __LINE__, "makedir ");
        }
    }


    //--------------------------------------------------------------------------
    //删除m_FaceDir下的所有文件
    //--------------------------------------------------------------------------
    DeleteFaceTempFile();

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    sprintf(m_FaceDocumentCfg, "%s/%s", m_szModuleDir, FACE_DOC_CFG);
    //sprintf(m_FaceDocumentCfg, "%s\\%s", m_FaceDir, FACE_DOC_CFG);

    memset(m_FaceDocumentFile, 0, MAX_PATH);

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    memset(m_FaceScriptFile, 0, MAX_PATH);
    sprintf(m_FaceScriptFile, "%s/%s", m_szModuleDir, WORK_FACE_SCRIPT);

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    memset(m_FaceTemplate, 0, MAX_PATH);
    sprintf(m_FaceTemplate, "%s/%s", m_szModuleDir, WORD_FACE_BIN);

    m_FaceScriptData = m_FaceScript.GetScriptData();
}


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
u32 FaceDocument::ParserFaceScript(char *script_path)
{
    u32 ret = OK;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_FaceScript.SetInterface(GetInterface(SCRIPTPARSER_PLUGIN_TYPE), SCRIPTPARSER_PLUGIN_TYPE);
    if (OK != ret)
    {
        return ret;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_FaceScript.ScriptParser(script_path, GetInterface(CONFIG_PLUGIN_TYPE));
    if (OK != ret)
    {
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
void *FaceDocument::GetInterface(const char *plug_guid)
{
    return m_PlugInCenter.GetInterface(plug_guid);
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
u32 FaceDocument::BuildFace(char *resFile,
                            char *resHeadFile,
                            char *script_path,
                            char *res_template,
                            u32 isClone)
{
    u32 ret = OK;


    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = ParserFaceScript(script_path);
    if (OK != ret)
    {
        Msg("Err:%s %d", __FILE__, __LINE__);
        return ret;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_FaceBuilder.SetInterface(GetInterface(FACEMAKER_PLUGIN_TYPE), FACEMAKER_PLUGIN_TYPE);
    if (OK != ret)
    {
        Msg("Err:%s %d", __FILE__, __LINE__);
        return ret;
    }


    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_FaceBuilder.Build(m_FaceScript.GetScriptData(),
                              resFile,
                              resHeadFile,
                              res_template,
                              GetInterface(RESACCESS_PLUGIN_TYPE),
                              isClone);

    if (OK != ret)
    {
        Msg("Err:%s %d", __FILE__, __LINE__);
        return ret;
    }

    return ret;
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//      face_document face文件路径
//      bTemplate     true face文件是模版，否则是用户‘方案
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
u32 FaceDocument::LoadFaceDocumet(char *face_document, BOOL bTemplate)
{
    u32             ret             = OK;
    HIMAGE          hImage          = NULL;
    IMGDECODE_IF   *imgdecode_if    = NULL;
    CString str;
    u16 ID = 0;

    if (NULL == face_document)
    {
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = IsFileExist(face_document);
    if (OK != ret)
    {
        str.Format("%s NOT exist ! Please Check the file !", face_document);
        printf("%s\n", str.GetBuffer(0));
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    imgdecode_if = (IMGDECODE_IF *)GetInterface(IMGDECODE_PLUGIN_TYPE);
    if (NULL == imgdecode_if)
    {
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    hImage = imgdecode_if->Img_Open(face_document);
    if (NULL == hImage)
    {
        str.Format("%s is not valid face document", face_document);
        //::AfxMessageBox(str);
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    // face.script
    //--------------------------------------------------------------------------
    ret = GetFaceDocumentItem(imgdecode_if, hImage, MAIN_FACE_SCRIPT, SUB_FACE_SCRIPT, m_FaceScriptFile);
    if (OK != ret)
    {
        Msg("GetFaceDocumentItem %s %s %s failed %d", MAIN_FACE_SCRIPT, SUB_FACE_SCRIPT, m_FaceScriptFile, ret);
        goto Error_End;
    }


    //--------------------------------------------------------------------------
    // face.bin
    //--------------------------------------------------------------------------
    ret = GetFaceDocumentItem(imgdecode_if, hImage, MAIN_FACE_TEMPLATE, SUB_FACE_TEMPLATE, m_FaceTemplate);
    if (OK != ret)
    {
        Msg("GetFaceDocumentItem %s %s %s failed %d", MAIN_FACE_TEMPLATE, SUB_FACE_TEMPLATE, m_FaceScriptFile, ret);
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    imgdecode_if->Img_Close(hImage);
    hImage = NULL;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    if (bTemplate)
    {
        ret = ParserFaceScript(m_FaceScriptFile);
        if (OK != ret)
        {
            goto Error_End;
        }
    }
    else
    {
        ret = ParserFaceScript(m_FaceScriptFile);
        if (OK != ret)
        {
            goto Error_End;
        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    m_FaceScriptData = m_FaceScript.GetScriptData();
    if (NULL != m_FaceScriptData)
    {
        //m_FaceScriptData->Dump();
    }
    //Msg("LoadFaceDocumet ResCount=%d", m_FaceScriptData->GetResCount());

    ret = m_FaceResource.SetInterface(GetInterface(RESACCESS_PLUGIN_TYPE), RESACCESS_PLUGIN_TYPE);
    if (OK != ret)
    {
        Msg("Err: %s %d", __FILE__, __LINE__);
        goto Error_End;
    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_FaceResource.OpenResource(m_FaceTemplate);
    if (OK != ret)
    {
        Msg("Err: %s %d", __FILE__, __LINE__);
        goto Error_End;
    }


    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    strcpy(m_FaceDocumentFile, face_document);

    //Msg("LoadFaceDocumet(%s) OK", face_document);

    return ret;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
Error_End:

    if (NULL != hImage)
    {
        imgdecode_if->Img_Close(hImage);
        hImage = NULL;
    }

    Msg("LoadFaceDocumet error");

    return ret;
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------

u32 FaceDocument::GetFaceDocumentItem(IMGDECODE_IF *imgdecode_if, HIMAGE hImage,
                                      char *main, char *sub, char *filepath)
{

    u32             ret         = OK;
    HIMAGEITEM      hItem       = NULL;
    u8             *buffer      = NULL;
    u32             filesize    = 0;
    u32             datasize    = 0;
    FILE           *fp          = NULL;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    //Msg("GetFaceDocumentItem : %s %s %s", main, sub, filepath);
    if (NULL == imgdecode_if)
    {
        return __LINE__;
    }
    if (NULL == hImage)
    {
        return __LINE__;
    }

    if (NULL == main || NULL == sub)
    {
        return __LINE__;
    }

    if (NULL == filepath)
    {
        return __LINE__;
    }

    if (strlen(filepath) >= MAX_PATH)
    {
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    if (OK == IsFileExist(filepath))
    {
        remove(filepath);
    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    hItem = imgdecode_if->Img_OpenItem(hImage, main, sub);
    if (NULL == hItem)
    {
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    filesize = imgdecode_if->Img_GetItemSize(hImage, hItem);
    if (0 == filesize)
    {
        ret =  __LINE__;
        goto Error_End;
    }

    datasize = 16 * ((filesize + 15) / 16);
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    buffer = (u8 *)malloc(datasize);
    if (NULL == buffer)
    {
        ret = __LINE__;
        goto Error_End;
    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    if (datasize != imgdecode_if->Img_ReadItemData(hImage, hItem, buffer, datasize))
    {
        ret = __LINE__;
        Msg("Img_ReadItemData failed ");
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    imgdecode_if->Img_CloseItem(hImage, hItem);
    hItem = NULL;

    //--------------------------------------------------------------------------
    // save buffer to file
    //--------------------------------------------------------------------------
    fp = fopen(filepath, "wb+");
    if (NULL == fp)
    {
        Msg("fopen failed ");
        ret = __LINE__;
        goto Error_End;
    }

    fwrite(buffer, filesize, 1, fp);

    fclose(fp);
    fp = NULL;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    free(buffer);
    buffer = NULL;

    return ret;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
Error_End:
    if (NULL != buffer)
    {
        free(buffer);
        buffer = NULL;
    }

    if (NULL != hItem)
    {
        imgdecode_if->Img_CloseItem(hImage, hItem);
        hItem = NULL;
    }

    return ret;
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
u32 FaceDocument::StoreFaceDocumet(char *face_document)
{
    u32 ret     = OK;
    u32 Count   = 0;
    u32 i       = 0;
    CString str;

    m_FaceDocumentBuilder.SetInterface(GetConfigInterface(), CONFIG_PLUGIN_TYPE);
    //m_FaceDocumentBuilder.SetInterface(GetInterface(CONFIG_PLUGIN_TYPE), CONFIG_PLUGIN_TYPE);

    char szModuleDir[MAX_PATH];
    char szPlugPath[MAX_PATH];
    getcwd(szModuleDir, MAX_PATH);

    sprintf(szPlugPath, "%s/%s", szModuleDir, IMGBUILDER_PLUG);

    typedef void*(*pGetInterface)(const char * plug_guid);

    void *hDynamicLib = dlopen(szPlugPath, RTLD_LAZY);
    pGetInterface funHandle = (pGetInterface) dlsym(hDynamicLib, "GetInterface");


    m_FaceDocumentBuilder.SetInterface(funHandle(IMAGEBUILDER_PLUGIN_TYPE), IMAGEBUILDER_PLUGIN_TYPE);
    //m_FaceDocumentBuilder.SetInterface(GetInterface(IMAGEBUILDER_PLUGIN_TYPE), IMAGEBUILDER_PLUGIN_TYPE);

    //--------------------------------------------------------------------------
    //填写m_FaceDocumentCfg的信息
    //--------------------------------------------------------------------------
    m_FaceDocumentData.Release();

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_FaceDocumentBuilder.ParserConfig(m_FaceDocumentCfg, &m_FaceDocumentData);
    if (OK != ret)
    {
        Msg("ParserConfig failed %d", ret);
        goto Error_End;
    }
    //m_FaceDocumentData.DumpSystemData();
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    //strcpy((char *)m_FaceDocumentData.GetSystemData()->image, face_document);
    sprintf((char *)m_FaceDocumentData.GetSystemData()->image, "%s", face_document);

    //m_FaceDocumentData.DumpSystemData();
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    Count = m_FaceDocumentData.GetSystemData()->filecnt;
    for (i = 0 ; i < Count; i++)
    {
        ITEM_INFO *pItemInfo = m_FaceDocumentData.GetItemData(i);
        if (0 == strncmp(MAIN_FACE_SCRIPT, (const char *)pItemInfo->main, 8))
        {
            strcpy((char *)pItemInfo->filepath, m_FaceScriptFile);  //face.script
        }
        else if (0 == strncmp(MAIN_FACE_TEMPLATE, (const char *)pItemInfo->main, 8))
        {
            strcpy((char *)pItemInfo->filepath, m_FaceTemplate);    //face.bin
        }
        else
        {
            Msg("unuse file");
        }
        //m_FaceDocumentData.DumpItem(i);
    }


    //--------------------------------------------------------------------------
    //将当前资源信息数据写入m_FaceScriptFile文件
    //--------------------------------------------------------------------------
    //m_FaceScript.SetInterface(GetInterface(SCRIPTPARSER_PLUGIN_TYPE), SCRIPTPARSER_PLUGIN_TYPE);
    m_FaceScript.SetInterface(GetScriptParserInterface(), SCRIPTPARSER_PLUGIN_TYPE);

    //ret = m_FaceScript.SaveDataToScript(m_FaceScriptFile, m_FaceScriptData, GetInterface(CONFIG_PLUGIN_TYPE));
    ret = m_FaceScript.SaveDataToScript(m_FaceScriptFile, m_FaceScriptData, GetConfigInterface());
    if (OK != ret)
    {
        Msg("SaveDataToScript failed %d", ret);
        goto Error_End;
    }


    //--------------------------------------------------------------------------
    //m_FaceTemplate文件
    //--------------------------------------------------------------------------


    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = BuildFaceDocument();
    if (OK == ret)
    {
        str.Format("StoreFaceDocumet OK");
        Msg("StoreFaceDocumet OK\n");
    }
    else
    {
        str.Format("StoreFaceDocumet failed %d", ret);
        Msg("StoreFaceDocumet failed %d", ret);
    }
    //::AfxMessageBox(str);

    return ret;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
Error_End:

    Msg("StoreFaceDocumet_error");

    return ret;
}



//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
u32 FaceDocument::BuildFaceDocument()
{
    return m_FaceDocumentBuilder.Build(&m_FaceDocumentData);

}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
FaceScriptData *FaceDocument::GetScriptData()
{
    return m_FaceScript.GetScriptData();
}


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
char *FaceDocument::GetModuleDir()
{
    return m_szModuleDir;
}


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------

FaceResource *FaceDocument::GetFaceResource()
{
    return & m_FaceResource;
}


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------

FaceScriptData *FaceDocument::GetFaceScriptData()
{
    return m_FaceScriptData;
}



//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
u32 FaceDocument::BuildFace(char *resFile, u32 IsClone)
{
    char resHeadFile[MAX_PATH];
    CString str, csDir, csName, csTmp;
    int pos = 0;

    str = resFile;
    pos = str.ReverseFind('/');
    csDir = str.Left(pos);

    csTmp = str.Right(str.GetLength() - pos - 1);

    pos = csTmp.ReverseFind('.');
    csName = csTmp.Left(pos);

    sprintf(resHeadFile, "%s/%s.h", csDir.GetBuffer(0), csName.GetBuffer(0));

    Msg("BuildFace:%s ", resHeadFile);

    return BuildFace(resFile,
                     resHeadFile,
                     m_FaceScriptFile,
                     m_FaceTemplate,
                     IsClone);
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------

void FaceDocument::DeleteFaceTempFile()
{
    u32 ret = OK;

    //WIN32_FIND_DATA finddata; //pc查找数据结构
    HANDLE hFind = NULL;        //pc查找句柄

    char szFilePath[MAX_PATH];
    char findmode[MAX_PATH];        //当前查找的pc目录的匹配模式

    //Msg("DeleteFaceTempFile : %s", m_FaceDir);

    if (OK != IsDirExist(m_FaceDir))
    {
        Msg("dir %s not exist !\n", m_FaceDir);
        return ;
    }

    //----------------------------------------------------------
    //搜索的资源目录
    //----------------------------------------------------------
    memset(findmode, 0, MAX_PATH);
    sprintf((char *)findmode, "%s", m_FaceDir); //查找的匹配模式


    char path[512];
    struct dirent *filename;//readdir 的返回类型
    DIR *dir;//血的教训阿，不要随便把变量就设成全局变量。。。。
    dir = opendir(findmode);
    if (dir == NULL)
    {
        printf("open dir %s error!\n", findmode);
        exit(1);
    }

    while ((filename = readdir(dir)) != NULL)
    {
        //目录结构下面问什么会有两个.和..的目录？ 跳过着两个目录
        if (!!strcmp(filename->d_name, ".svn") || strcmp(filename->d_name, ".") || !strcmp(filename->d_name, ".."))
            continue;

        //非常好用的一个函数，比什么字符串拼接什么的来的快的多
        sprintf(path, "%s/%s", findmode, filename->d_name);

        struct stat s;
        lstat(path, &s);
        if (S_ISDIR(s.st_mode))
        {

        }
        else if (S_ISREG(s.st_mode))
        {
            memset(szFilePath, 0, MAX_PATH);
            sprintf(szFilePath, "%s/%s", m_FaceDir, (const char *)filename->d_name);
            ret = remove((const char *)szFilePath);
            if (OK != ret)
            {
                Msg("RemoveFile(%s) failed %d", filename->d_name, ret);
            }
        }
    }
    closedir(dir);

    return ;
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
char *FaceDocument::GetFaceDocumentFile()
{
    return m_FaceDocumentFile;
}


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------

