//------------------------------------------------------------------------------------------------------------
//
//  GenScript.h
//
//  2009-11-9 17:49:44
//
//  sunny
//
//------------------------------------------------------------------------------------------------------------

#ifndef     __GEN_SCRIPT_H__
#define     __GEN_SCRIPT_H__

#include "script_type.h"

class GenScriptCore
{
public:
    int  CreateScriptFile(const char *filename);
    int  CloseScriptFile(void);
    int  GenScriptFile(const char *scandir, u32 BaseID, u32 MaxID);

    GenScriptCore();
    virtual ~GenScriptCore();

protected:

    FILE *ScriptFile;
    int   WriteOneResItem(FACE_RES_ITEM *ResItem);
    int   GenOneResItem(const char *ResPath, u32 ResID);
    int   MatchResTypeFile(char *filename);
    int   IsResTypeFile(struct dirent *finddata);
};

#endif  // __GEN_SCRIPT_H__
