#include <stdio.h>

void t(int *p)
{

}

int main()
{
    long long h = 0x7ffff7fd2000;
    unsigned int i = (unsigned int)h;
    printf("i:%d\n", i);
    return 0;
}


