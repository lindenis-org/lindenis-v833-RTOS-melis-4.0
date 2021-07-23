#include <typedef.h>

void __SLIB_int2str_dec(int input, char *str);
void __SLIB_uint2str_dec(unsigned int input, char *str);
void __SLIB_int2str_hex(int input, char *str, int hex_flag);
int __SLIB_atoi(const char *s);
int __SLIB_toupper(int c);
int  __SLIB_isspace(int ch);
long __SLIB_strtol(const char *str, const char **err, int base);
__size  __SLIB_strlen(const char  *pstr);
char *__SLIB_strcpy(char *pdest, const char *psrc);
char *__SLIB_strncpy(char *pdest, const char *psrc, __size len_max);
char *__SLIB_strcat(char *pdest, const char *pstr_cat);
char *__SLIB_strncat(char *pdest, char *pstr_cat, size_t len_max);
int __SLIB_strcmp(const char *p1_str, const char *p2_str);
int __SLIB_stricmp(const char *p1_str, const char *p2_str);
int __SLIB_strncmp(const char *p1_str, const char *p2_str, size_t len_max);
int __SLIB_strnicmp(const char *p1_str, const char *p2_str, size_t len_max);
char *__SLIB_strchr(char *pstr, char srch_char);
char *__SLIB_strchrlast(char *pstr, char srch_char);
char *__SLIB_strstr(char *pstr, char *psrch_str);
void  __SLIB_memclr(void *pmem, size_t size);
void  __SLIB_memset(void *pmem, __u8 data_val, size_t size);
void *__SLIB_memcpy(void *pdest, const void *psrc, size_t size);
int __SLIB_memcmp(const void *p1_mem, const void *p2_mem, size_t size);
int __SLIB_vsnprintf(char *buf, size_t size, const char *format, va_list arg);
char *__SLIB_strnchr(char *pstr, size_t len_max, char srch_char);

double __SLIB_atof(const char *s);
__s8 __SLIB_sscanf(const char *buf, const char *fmt, ...);

const uint32_t slib_api_tbl[] __attribute__((section(".api_table")))  =
{
    (uint32_t)__SLIB_int2str_dec,
    (uint32_t)__SLIB_uint2str_dec,
    (uint32_t)__SLIB_int2str_hex,
    (uint32_t)__SLIB_atoi,
    (uint32_t)__SLIB_toupper,
    (uint32_t)__SLIB_isspace,
    (uint32_t)__SLIB_strtol,
    (uint32_t)__SLIB_strlen,
    (uint32_t)__SLIB_strcpy,
    (uint32_t)__SLIB_strncpy,
    (uint32_t)__SLIB_strcat,
    (uint32_t)__SLIB_strncat,
    (uint32_t)__SLIB_strcmp,
    (uint32_t)__SLIB_stricmp,
    (uint32_t)__SLIB_strncmp,
    (uint32_t)__SLIB_strnicmp,
    (uint32_t)__SLIB_strchr,
    (uint32_t)__SLIB_strnchr,
    (uint32_t)__SLIB_strchrlast,
    (uint32_t)__SLIB_strstr,
    (uint32_t)__SLIB_memclr,
    (uint32_t)__SLIB_memset,
    (uint32_t)__SLIB_memcpy,
    (uint32_t)__SLIB_memcmp,
    (uint32_t)__SLIB_vsnprintf,
    (uint32_t)__SLIB_atof,
    (uint32_t)__SLIB_sscanf,
};
