#include <log.h>

#include "spinor_config.h"

#ifdef CONFIG_FEXCONFIG
#include <script.h>
#endif

#define NOR_DEFAULT_FREQUENCY 50

int get_clock_frequency(int *freq)
{
    int frequency = 100 * 1000 * 1000;
    int ret = 0;

#ifdef CONFIG_FEXCONFIG
    ret = script_parser_fetch(script_get_handle(), "spinor_para", "frequency", &frequency, 1);
    if (ret)
    {
        pr_debug("get frequecy from script failed!\n");
        frequency = 100 * 1000 * 1000;
    }
#endif
    *freq = frequency;
    return ret;
}

int get_read_mode(int *read)
{
    int read_mode = DEFAULT_READ_MODE;
    int ret = -1;

#ifdef CONFIG_FEXCONFIG
    ret = script_parser_fetch(script_get_handle(), "spinor_para", "read_mode", &read_mode, 1);
    if (ret)
    {
        pr_debug("get read_mode from script failed, ret = %d!\n", ret);
        read_mode = DEFAULT_READ_MODE;
    }
#endif
    *read = read_mode;
    return ret;
}

int get_write_mode(int *write)
{
    int write_mode = DEFAULT_WRITE_MODE;
    int ret = -1;

#ifdef CONFIG_FEXCONFIG
    ret = script_parser_fetch(script_get_handle(), "spinor_para", "write_mode", &write_mode, 1);
    if (ret)
    {
        pr_debug("get write_mode from script failed, ret = %d!\n", ret);
        write_mode = DEFAULT_WRITE_MODE;
    }
#endif
    *write = write_mode;

    return ret;
}

int get_spi_port(int *spi_port)
{
    int port = 0;
    int ret = -1;

#ifdef CONFIG_FEXCONFIG
    ret = script_parser_fetch(script_get_handle(), "spinor_para", "spi_port", &port, 1);
    if (ret)
    {
        pr_debug("get spi port from script failed, ret = %d!\n", ret);
        port = 0;
    }
#endif
    *spi_port = port;
    return ret;
}
