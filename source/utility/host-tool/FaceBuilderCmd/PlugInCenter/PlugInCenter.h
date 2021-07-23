// PlugInCenter.h: interface for the CPlugInCenter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGINCENTER_H__E3D3EF83_F5FF_4C0A_97AF_8C513458BF5D__INCLUDED_)
#define AFX_PLUGINCENTER_H__E3D3EF83_F5FF_4C0A_97AF_8C513458BF5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#include "PlugInWrapper/PlugInWrapper.h"    //

#define GUID_LEN    36
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef struct tag_PLUG_IF_ITEM PLUG_IF_ITEM_t;

struct tag_PLUG_IF_ITEM
{
    void            *_if;               //
    CPlugInWrapper *Wrapper;            //
    char             GUID[GUID_LEN];    //
    PLUG_IF_ITEM_t *next;               //
};

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
class CPlugInCenter
{
public:
    void Release(void);

    void *GetInterface(const char *plug_guid);

    CPlugInCenter();
    virtual ~CPlugInCenter();

protected:
    void *LoadInterface(const char *plug_guid);

    void *GetInterfaceFromList(const char *plug_guid);

    u32 AddPlugItem(PLUG_IF_ITEM_t *item);

    void DeleteList();


    PLUG_IF_ITEM_t *m_PlugList;         //

};

#endif // !defined(AFX_PLUGINCENTER_H__E3D3EF83_F5FF_4C0A_97AF_8C513458BF5D__INCLUDED_)
