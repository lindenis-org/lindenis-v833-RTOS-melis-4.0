#include <stdio.h>
#include "basetypes.h"
#include "osdvectorman_if.h"
#include "basefun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int main(int argc, TCHAR *argv[], TCHAR *envp[])
{
    int nRet = 0;
    char csXmlName[MAX_PATH];

    if (argc != 2)
    {
        printf("error!\n");
        return -1;
    }

    get_full_path(csXmlName, argv[1]);
    printf("%s\n", csXmlName);

    nRet = BuildOsd(csXmlName);
    return nRet;
}
