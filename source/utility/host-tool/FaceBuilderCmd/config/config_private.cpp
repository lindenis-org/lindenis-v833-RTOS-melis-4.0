//------------------------------------------------------------------------------------------------------------
//
//2008-10-16 8:58:24
//
//scott
//
//config_private.cpp
//
//------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_private.h"


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
void AddItem(POS_t *pos, ListItem *item)
{
    if (!pos || !item)
        return;

    if (!pos->list)
    {
        pos->list = item;
        pos->count++;
        pos->current = pos->list;
        pos->index = 0;
    }
    else
    {
        ListItem *pcurrent = pos->list;
        while (pcurrent && pcurrent->Next)
        {
            pcurrent = pcurrent->Next;
        }
        pcurrent->Next = item;
        pos->count++;
        pos->current = pos->list;
        pos->index = 0;
    }
}
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
void Delete(POS_t *pos)
{
    if (!pos)
        return;

    if (!pos->list || pos->count == 0)
        return;

    ListItem *pcurrent = NULL;
    while (pos->list)
    {
        pcurrent = pos->list;
        pos->list = pcurrent->Next;
        pos->count--;
        free(pcurrent->String);
        free(pcurrent);
    }
    free(pos);
}

