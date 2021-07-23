
#include <stdarg.h>
#include "fsys_libs.h"

/* ePDkK system services */
__u32 CPU_SR_Save(void)
{
    return 0;
}
void CPU_SR_Restore(__u32 x) {}
__krnl_event_t  *esKRNL_QCreate(__u16 size)
{
    return NULL;
}
__u8 esKRNL_TCreate(void (*thread)(void *p_arg), void *p_arg, __u32 stksize, __u16 id_priolevel)
{
    return 0;
}
void  esKRNL_TimeDly(__u16 ticks) {};
void  *esKRNL_QPend(__krnl_event_t *pevent, __u16 timeout, __u8 *err)
{
    return NULL;
}
__u32 esKRNL_Time(void)
{
    return 0;
}
__s8 esKRNL_TDel(__u8 prio)
{
    return 0;
}
__s8 esKRNL_TDelReq(__u8 prio)
{
    return 0;
}
__krnl_event_t  *esKRNL_SemDel(__krnl_event_t *pevent, __u8 opt, __u8 *err)
{
    return NULL;
}
void  esKRNL_SemPend(__krnl_event_t *pevent, __u16 timeout, __u8 *err)
{
    *err = 0;
}
__u8  esKRNL_SemPost(__krnl_event_t *pevent)
{
    return 0;
}
__krnl_event_t  *esKRNL_SemCreate(__u16 cnt)
{
    return (__krnl_event_t *)1;
}
__u32 esKSRV_Random(__u32 max)
{
    return 0;
}

void eLIBs_int2str_dec(int input , char *str)
{
    char stack[12];
    char sign_flag = 0 ;      // 'sign_flag indicates wheater 'input' is positive or negative, default
    int i ;                           // value is 'POSITIVE'.
    int j ;

    if (input == 0)
    {
        str[0] = '0';
        str[1] = '\0';                   // 'str' must end with '\0'
        return ;
    }

    if (input < 0)                       // If 'input' is negative, 'input' is assigned to its absolute value.
    {
        sign_flag = 1 ;
        input = -input ;
    }

    for (i = 0; input > 0; ++i)
    {
        stack[i] = input % 10 + '0';    // characters in reverse order are put in 'stack' .
        input /= 10;
    }                                   // at the end of 'for' loop, 'i' is the number of characters.


    j = 0;
    if (sign_flag == 1)
        str[j++] = '-';                 // If 'input' is negative, minus sign '-' is placed in the head.
    for (--i  ; i >= 0; --i, ++j)
        str[j] = stack[i];
    str[j] = '\0';                      // 'str' must end with '\0'
}

/*
******************************************************************************************************************
*
*Function Name : Uint_to_string_dec
*
*Description : This function is to convert an 'unsigned int' data 'input' to a string in decimalism, and the
*              string converted is in 'str'.
*
*Input : int input : 'unsigned int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void eLIBs_uint2str_dec(unsigned int input, char *str)
{
    char stack[11] ;
    int i ;
    int j ;


    if (input == 0)
    {
        str[0] = '0';
        str[1] = '\0';                   // 'str' must end with '\0'
        return ;
    }

    for (i = 0; input > 0; ++i)
    {
        stack[i] = input % 10 + '0';     // characters in reverse order are put in 'stack' .
        input /= 10;
    }                                    // at the end of 'for' loop, 'i' is the number of characters.

    for (--i, j = 0; i >= 0; --i, ++j)
        str[j] = stack[i];
    str[j] = '\0';                       // 'str' must end with '\0'
}

/*
******************************************************************************************************************
*
*Function Name : int_to_string_hex
*
*Description : This function is to convert an 'int' data 'input' to a string in hex, and the string
*              converted is in 'str'.
*
*Input : int input : 'int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*        int hex_flag : hex_flag is just a option to distinguish whether hex format is '0f' or '0F'.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void eLIBs_int2str_hex(int input, char *str, int hex_flag)
{
    int i;
    int j;
    char stack[8];


    for (i = 0; i < 8; ++i)
    {
        if (hex_flag == HEX_X)
            stack[i] = "0123456789ABCDEF"[input & 0xf];
        else
            stack[i] = "0123456789abcdef"[input & 0xf];

        input >>= 4;
    }

    str[0] = '0';
    str[1] = hex_flag;                         // Hex format shoud start with "0x" or "0X".
    for (i = 7, j = 2; i >= 0; --i, ++j)
        str[j] = stack[i];
    str[j] = '\0';                             // 'str' must end with '\0'
}

int fsyslib_ffs(int x)
{
    int r = 1;

    if (!x)
        return 0;
    if (!(x & 0xffff))
    {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff))
    {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf))
    {
        x >>= 4;
        r += 4;
    }
    if (!(x & 3))
    {
        x >>= 2;
        r += 2;
    }
    if (!(x & 1))
    {
        x >>= 1;
        r += 1;
    }
    return r;
}


