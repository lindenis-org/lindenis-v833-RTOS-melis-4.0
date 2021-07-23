#include <cli_console.h>
#include <stdint.h>
#include <libc.h>

int uart_console_write(const void * buf, size_t len, void * privata_data)
{
    char * output_buf = (char *)buf;
    int i = 0;

    while(i < len)
    {
        /*
         * to be polite with serial console add a line feed
         * to the carriage return character
         */
        if (*(output_buf + i) == '\n')
        {
            esSIOS_putchar('\r');
        }

        esSIOS_putchar(*(output_buf + i));
        i++;
    }
    return len;
}

int uart_console_read(void * buf, size_t len, void * privata_data)
{
    char *inbuf = (char *)buf;
    char ch = esSIOS_getchar();
    *inbuf = ch;
    return 1;
}

static int uart_console_init(void * private_data)
{
    return 1;
}

static int uart_console_deinit(void * private_data)
{
    return 1;
}

static device_console uart_console =
{
    .name = "uart-console",
    .write = uart_console_write,
    .read = uart_console_read,
    .init = uart_console_init,
    .deinit = uart_console_deinit
};

cli_console cli_uart_console =
{
    .i_list = {0},
    .name = "cli-uart",
    .dev_console = &uart_console,
    .init_flag = 0,
    .exit_flag = 0,
    .alive = 1,
    .private_data = NULL,
    .task_list = {0},
    .finsh_callback = NULL,
    .start_callback =NULL,
};
