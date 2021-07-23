#include <rtthread.h>
#include <cli_console.h>

static int cmd_exit(int argc, char *argv[])
{
#ifdef CONFIG_COMPONENTS_MULTI_CONSOLE
    cli_console_current_task_destory();
#endif
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_exit, __cmd_exit, Exit current console);
