#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "basetypes.h"

using namespace std;

int is_full_path(const char *filepath)
{
    if (filepath[0] && '/' == filepath[0])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void get_full_path(char *dname, const char *sname)
{
    char buffer[MAX_PATH];

    if (is_full_path(sname))
    {
        strcpy(dname, sname);
        return ;
    }

    if (getcwd(buffer, MAX_PATH) == 0)
    {
        perror("getcwd error");
        return ;
    }
    sprintf(dname, "%s/%s", buffer, sname);
}
