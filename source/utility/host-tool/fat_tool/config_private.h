//------------------------------------------------------------------------------------------------------------
//
//2008-10-16 8:58:24
//
//scott
//
//config_private.h
//
//------------------------------------------------------------------------------------------------------------

#ifndef __CONFIG___PRIVATE____H____
#define __CONFIG___PRIVATE____H____

#include    "config.h"

typedef struct tagListItem
{
    s8 *String;
    struct tagListItem *Next;
} ListItem;

typedef struct tagPOS
{
    u32         count;
    ListItem   *list;
    ListItem   *current;
    u32         index;
} POS_t;

void AddItem(POS_t *pos, ListItem *item);

void Delete(POS_t *pos);


#endif //__CONFIG___PRIVATE____H____
