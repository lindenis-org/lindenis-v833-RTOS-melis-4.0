#ifndef CONSOLE_H
#define CONSOLE_H

#include <pthread.h>
#ifdef __cplusplus
extern "C" {
#endif

#define SECTION(x)  __attribute__((section(x)))
#define RT_USED     __attribute__((used))

#define SH_MAX_CMD_ARGS 16
#define SH_MAX_CMD_LEN 256

#if 0
typedef int (*syscall_func)(int argc, char **argv);

/* system call table */
struct finsh_syscall
{
    const char*     name;       /* the name of system call */
    const char*     desc;       /* description of system call */
    syscall_func func;      /* the function address of system call */
};
extern struct finsh_syscall *_syscall_table_begin, *_syscall_table_end;
#define FINSH_FUNCTION_EXPORT_CMD(name, cmd, desc)                      \
                const char __fsym_##cmd##_name[] SECTION(".rodata") = #cmd;    \
                const char __fsym_##cmd##_desc[] SECTION(".rodata") = #desc;   \
                RT_USED const struct finsh_syscall __fsym_##cmd SECTION("FSymTab")= \
                {                           \
                    __fsym_##cmd##_name,    \
                    __fsym_##cmd##_desc,    \
                    (syscall_func)&name     \
                };

#define FINSH_VAR_EXPORT(name, type, desc)                              \
                const char __vsym_##name##_name[] SECTION(".rodata") = #name;  \
                const char __vsym_##name##_desc[] SECTION(".rodata") = #desc;  \
                RT_USED const struct finsh_sysvar __vsym_##name SECTION("VSymTab")= \
                {                           \
                    __vsym_##name##_name,   \
                    __vsym_##name##_desc,   \
                    type,                   \
                    (void*)&name            \
                };

extern struct finsh_syscall* finsh_syscall_lookup(const char* name);

extern void finsh_syscall_show(void);

#endif
/*
 * receive data from console
 * @buf: receive buf
 * @len: receive len, now just support receive one character
 * */
int read_from_console(void * buf, size_t len);

/* create command-line console
 * @usStakSize: stack size of command-line task
 * @uxPriority: priority of command-line task
 * @console: the command line console
 * */
//void vCommandConsoleStart( uint16_t usStackSize, unsigned portBASE_TYPE uxPriority, void * console );
void vCommandConsoleStart( uint16_t usStackSize, unsigned uxPriority, void * console );

#ifdef __cplusplus
}
#endif
#endif  /*CONSOLE_H*/
