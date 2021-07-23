/*
 * ===========================================================================================
 *
 *       Filename:  hal_serial.c
 *
 *    Description:  hal impl. of usart.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-14 14:20:56
 *       Revision:  none
 *       Compiler:  GCC:version 9.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  bantao@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2020-06-04 09:13:07
 *
 * ===========================================================================================
 */

#include <sunxi_hal_usart.h>
#include <interrupt.h>
#include <hal_queue.h>
#include <hal_clk.h>
#include <sunxi_hal_gpio.h>
#include <sunxi_hal_common.h>
#include <init.h>
#include <log.h>

#if (0)
#define UART_LOG_DEBUG
#endif
#define UART_INIT(fmt, ...) printf("uart: "fmt, ##__VA_ARGS__)
#define UART_ERR(fmt, ...)  printf("uart: "fmt, ##__VA_ARGS__)

#ifdef UART_LOG_DEBUG
#define UART_INFO(fmt, ...) printf("[%s %d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define UART_INFO_IRQ(fmt, ...) __log("[%s %d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define UART_INFO(fmt, ...)
#define UART_INFO_IRQ(fmt, ...)
#endif

static uint32_t sunxi_uart_port[] =
{
    APB_USART0_BASE, APB_USART1_BASE, APB_USART2_BASE, APB_USART3_BASE
};
static const uint32_t g_uart_irqn[] = {SUNXI_IRQ_UART0, SUNXI_IRQ_UART1,
                                       SUNXI_IRQ_UART2, SUNXI_IRQ_UART3
                                      };
static sunxi_hal_version_t hal_uart_driver =
{
    SUNXI_HAL_USART_API_VERSION,
    SUNXI_HAL_USART_DRV_VERSION
};
static uart_priv_t g_uart_priv[UART_MAX];

static hal_mailbox_t uart_mailbox[UART_MAX];

static const uint32_t g_uart_baudrate_map[] =
{
    300,
    600,
    1200,
    2400,
    4800,
    9600,
    19200,
    38400,
    57600,
    115200,
    230400,
    576000,
    921600,
    1000000,
    1500000,
    3000000,
    4000000,
};

//driver capabilities, support uart function only.
static const sunxi_hal_usart_capabilities_t driver_capabilities =
{
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator available */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    0, /* Transmit completed event: \ref ARM_USARTx_EVENT_TX_COMPLETE */
    0, /* Signal receive character timeout event: \ref ARM_USARTx_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USARTx_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USARTx_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USARTx_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USARTx_EVENT_RI */
    0  /* Reserved */
};

#define CONFIG_SUNXI_UART_SUPPORT_POLL

#ifdef CONFIG_SUNXI_UART_SUPPORT_POLL
#include <hal_poll.h>

static poll_wakeup_func uart_drv_poll_wakeup = NULL;

static int32_t uart_check_poll_state(int32_t dev_id, short key)
{
    int ret = -1;
    int32_t entry = 0;
    int32_t mask = 0;

    if (key & POLLIN)
    {
        ret = hal_mailbox_control((hal_mailbox_t)uart_mailbox[dev_id],
                                  IPC_MAILBOX_CMD_GET_STATE,
                                  &entry);
        if (!ret && entry != 0)
        {
            mask |= POLLIN;
        }
        else
        {
            mask = 0;
        }
    }
    else if (key & POLLOUT)
    {
        mask |= POLLOUT;
    }
    else
    {
        mask = 0;
    }
    return mask;
}

static int32_t hal_uart_poll_wakeup(int32_t dev_id, short key)
{
    int ret = -1;
    if (uart_drv_poll_wakeup)
    {
        ret = uart_drv_poll_wakeup(dev_id, key);
    }
    return ret;
}

static int32_t uart_register_poll_wakeup(poll_wakeup_func poll_wakeup)
{
    uart_drv_poll_wakeup = poll_wakeup;
    return 0;
}

static sunxi_hal_poll_ops uart_poll_ops =
{
    .check_poll_state = uart_check_poll_state,
    .hal_poll_wakeup = hal_uart_poll_wakeup,
    .register_poll_wakeup = uart_register_poll_wakeup,
};
#endif

static bool uart_port_is_valid(uart_port_t uart_port)
{
    return (uart_port < UART_MAX);
}

static bool uart_baudrate_is_valid(uart_baudrate_t baudrate)
{
    return (baudrate < UART_BAUDRATE_MAX);
}

static bool uart_config_is_valid(const _uart_config_t *config)
{
    return ((config->baudrate < UART_BAUDRATE_MAX) &&
            (config->word_length <= UART_WORD_LENGTH_8) &&
            (config->stop_bit <= UART_STOP_BIT_2) &&
            (config->parity <= UART_PARITY_EVEN));
}

static sunxi_hal_version_t get_version(int32_t dev)
{
    HAL_ARG_UNUSED(dev);
    return hal_uart_driver;
}

static sunxi_hal_usart_capabilities_t get_capabilities(int32_t dev)
{
    HAL_ARG_UNUSED(dev);
    return driver_capabilities;
}

static void uart_set_format(uart_port_t uart_port, uart_word_length_t word_length,
                            uart_stop_bit_t stop_bit, uart_parity_t parity)
{
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uart_priv_t *uart_priv = &g_uart_priv[uart_port];
    uint32_t value;

    value = hal_readb(uart_base + UART_LCR);

    /* set word length */
    value &= ~(UART_LCR_DLEN_MASK);
    switch (word_length)
    {
        case UART_WORD_LENGTH_5:
            value |= UART_LCR_WLEN5;
            break;
        case UART_WORD_LENGTH_6:
            value |= UART_LCR_WLEN6;
            break;
        case UART_WORD_LENGTH_7:
            value |= UART_LCR_WLEN7;
            break;
        case UART_WORD_LENGTH_8:
        default:
            value |= UART_LCR_WLEN8;
            break;
    }

    /* set stop bit */
    switch (stop_bit)
    {
        case UART_STOP_BIT_1:
        default:
            value &= ~(UART_LCR_STOP);
            break;
        case UART_STOP_BIT_2:
            value |= UART_LCR_STOP;
            break;
    }

    /* set parity bit */
    value &= ~(UART_LCR_PARITY_MASK);
    switch (parity)
    {
        case UART_PARITY_NONE:
            value &= ~(UART_LCR_PARITY);
            break;
        case UART_PARITY_ODD:
            value |= UART_LCR_PARITY;
            break;
        case UART_PARITY_EVEN:
            value |= UART_LCR_PARITY;
            value |= UART_LCR_EPAR;
            break;
    }

    uart_priv->lcr = value;
    hal_writeb(uart_priv->lcr, uart_base + UART_LCR);
}

static void uart_set_baudrate(uart_port_t uart_port, uart_baudrate_t baudrate)
{
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uart_priv_t *uart_priv = &g_uart_priv[uart_port];
    uint32_t actual_baudrate = g_uart_baudrate_map[baudrate];
    uint32_t value, quot, uart_clk;

    uart_clk = 24000000; /* FIXME: fixed to 24MHz */

    quot = (uart_clk + 8 * actual_baudrate) / (16 * actual_baudrate);

    UART_INFO("baudrate: %d, quot = %d\r\n", actual_baudrate, quot);

    uart_priv->dlh = quot >> 8;
    uart_priv->dll = quot & 0xff;

    /* hold tx so that uart will update lcr and baud in the gap of tx */
    hal_writeb(UART_HALT_HTX | UART_HALT_FORCECFG, uart_base + UART_HALT);
    hal_writeb(uart_priv->lcr | UART_LCR_DLAB, uart_base + UART_LCR);
    hal_writeb(uart_priv->dlh, uart_base + UART_DLH);
    hal_writeb(uart_priv->dll, uart_base + UART_DLL);
    hal_writeb(UART_HALT_HTX | UART_HALT_FORCECFG | UART_HALT_LCRUP, uart_base + UART_HALT);
    /* FIXME: implement timeout */
    while (hal_readb(uart_base + UART_HALT) & UART_HALT_LCRUP)
        ;

    /* In fact there are two DLABs(DLAB and DLAB_BAK) in the hardware implementation.
     * The DLAB_BAK is sellected only when SW_UART_HALT_FORCECFG is set to 1,
     * and this bit can be access no matter uart is busy or not.
     * So we select the DLAB_BAK always by leaving SW_UART_HALT_FORCECFG to be 1. */
    hal_writeb(uart_priv->lcr, uart_base + UART_LCR);
    hal_writeb(UART_HALT_FORCECFG, uart_base + UART_HALT);
}

static void uart_set_fifo(uart_port_t uart_port, uint32_t value)
{
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uart_priv_t *uart_priv = &g_uart_priv[uart_port];

    uart_priv->fcr = value;
    hal_writeb(uart_priv->fcr, uart_base + UART_FCR);
}

void hal_uart_set_hardware_flowcontrol(uart_port_t uart_port)
{
	const unsigned long uart_base = sunxi_uart_port[uart_port];
	uart_priv_t *uart_priv = &g_uart_priv[uart_port];
	uint32_t value;

	value = hal_readb(uart_base + UART_MCR);
	value |= UART_MCR_DTR | UART_MCR_RTS | UART_MCR_AFE;
	uart_priv->mcr = value;
	hal_writeb(uart_priv->mcr, uart_base + UART_MCR);

	/* enable with modem status interrupts */
	value = hal_readb(uart_base + UART_IER);
	value |= UART_IER_MSI;
	uart_priv->ier = value;
	hal_writeb(uart_priv->ier, uart_base + UART_IER);
}

void hal_uart_disable_flowcontrol(uart_port_t uart_port)
{
	const unsigned long uart_base = sunxi_uart_port[uart_port];
	uart_priv_t *uart_priv = &g_uart_priv[uart_port];
	uint32_t value;

	value = readb(uart_base + UART_MCR);
	value &= ~(UART_MCR_DTR | UART_MCR_RTS | UART_MCR_AFE);
	uart_priv->mcr = value;
	hal_writeb(uart_priv->mcr, uart_base + UART_MCR);

	/* disable with modem status interrupts */
	value = readb(uart_base + UART_IER);
	value &= ~(UART_IER_MSI);
	uart_priv->ier = value;
	hal_writeb(uart_priv->ier, uart_base + UART_IER);
}

static void uart_force_idle(uart_port_t uart_port)
{
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uart_priv_t *uart_priv = &g_uart_priv[uart_port];
    uint32_t value;

    if (uart_priv->fcr & UART_FCR_FIFO_EN)
    {
        hal_writeb(UART_FCR_FIFO_EN, uart_base + UART_FCR);
        hal_writeb(UART_FCR_TXFIFO_RST
                   | UART_FCR_RXFIFO_RST
                   | UART_FCR_FIFO_EN, uart_base + UART_FCR);
        hal_writeb(0, uart_base + UART_FCR);
    }

    hal_writeb(uart_priv->fcr, uart_base + UART_FCR);
    (void)hal_readb(uart_base + UART_FCR);
}

static void uart_handle_busy(uart_port_t uart_port)
{
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uart_priv_t *uart_priv = &g_uart_priv[uart_port];
    uint32_t value;

    (void)hal_readb(uart_base + UART_USR);

    /*
     * Before reseting lcr, we should ensure than uart is not in busy
     * state. Otherwise, a new busy interrupt will be introduced.
     * It is wise to set uart into loopback mode, since it can cut down the
     * serial in, then we should reset fifo(in my test, busy state
     * (UART_USR_BUSY) can't be cleard until the fifo is empty).
     */
    hal_writeb(uart_priv->mcr | UART_MCR_LOOP, uart_base + UART_MCR);
    uart_force_idle(uart_port);
    hal_writeb(uart_priv->lcr, uart_base + UART_LCR);
    hal_writeb(uart_priv->mcr, uart_base + UART_MCR);
}

static uint32_t uart_handle_rx(uart_port_t uart_port, uint32_t lsr)
{
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uint8_t ch = 0;

    UART_INFO("IRQ uart%d handle rx \n", uart_port);
    do
    {
        if (lsr & UART_LSR_DR)
        {
            ch = hal_readb(uart_base + UART_RBR);
            hal_mailbox_send((hal_mailbox_t)uart_mailbox[uart_port], ch);
#ifdef CONFIG_SUNXI_UART_SUPPORT_POLL
            if (uart_poll_ops.hal_poll_wakeup)
            {
                uart_poll_ops.hal_poll_wakeup(uart_port, POLLIN);
            }
#endif
        }
        lsr = readb(uart_base + UART_LSR);
    } while ((lsr & (UART_LSR_DR | UART_LSR_BI)));

    return lsr;
}


static irqreturn_t uart_irq_handler(int irq, void *dev_id)
{
    uart_priv_t *uart_priv = dev_id;
    uart_port_t uart_port = uart_priv->uart_port;
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uint32_t iir, lsr;

    iir = hal_readb(uart_base + UART_IIR) & UART_IIR_IID_MASK;
    lsr = hal_readb(uart_base + UART_LSR);

    UART_INFO_IRQ("IRQ uart%d lsr is %08x \n", uart_port, lsr);
    if (iir == UART_IIR_IID_BUSBSY)
    {
        uart_handle_busy(uart_port);
    }
    else
    {
        if (lsr & (UART_LSR_DR | UART_LSR_BI))
        {
            lsr = uart_handle_rx(uart_port, lsr);
        }
        else if (iir & UART_IIR_IID_CHARTO)
            /* has charto irq but no dr lsr? just read and ignore */
        {
            hal_readb(uart_base + UART_RBR);
        }

        /* if (lsr & UART_LSR_THRE)
	{
	    uart_handle_tx(uart_port);
	}*/
    }
    return 0;
}

static void uart_enable_irq(uart_port_t uart_port, uint32_t irq_type)
{
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uint32_t value;

    value = hal_readb(uart_base + UART_IER);
    value |= irq_type;
    hal_writeb(value, uart_base + UART_IER);

}

static void uart_enable_busy_cfg(uart_port_t uart_port)
{
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uint32_t value;

    value = hal_readb(uart_base + UART_HALT);
    value |= UART_HALT_FORCECFG;
    hal_writeb(value, uart_base + UART_HALT);
}

static int uart_clk_init(int bus, bool enable)
{
    hal_clk_id_t clk;
    int ret;

    switch (bus)
    {
        case 0:
            bus = HAL_CLK_PERIPH_UART0;
            break;
        case 1:
            bus = HAL_CLK_PERIPH_UART1;
            break;
        case 2:
            bus = HAL_CLK_PERIPH_UART2;
            break;
        case 3:
            bus = HAL_CLK_PERIPH_UART3;
            break;
        default:
            UART_ERR("uart%d is invalid\n", bus);
            return -1;
    }
    if (enable)
    {
        ret = hal_clock_enable(bus);
        if (ret)
        {
            UART_ERR("[uart%d] couldn't enable clk!\n", bus);
            return -1;
        }
    }
    else
    {
        ret = hal_clock_disable(bus);
        if (ret)
        {
            UART_ERR("[uart%d] couldn't disable clk!\n", bus);
            return -1;
        }
    }
    return 0;
}

static void uart_pinctrl_init(uart_port_t uart_port)
{
    switch (uart_port)
    {
        case UART_0:
            hal_gpio_pinmux_set_function(UART0_TX, UART_GPIO_FUNCTION);//TX
            hal_gpio_pinmux_set_function(UART0_RX, UART_GPIO_FUNCTION);//RX
            break;
        case UART_1:
            hal_gpio_pinmux_set_function(UART1_TX, UART_GPIO_FUNCTION);//TX
            hal_gpio_pinmux_set_function(UART1_RX, UART_GPIO_FUNCTION);//RX
            hal_gpio_pinmux_set_function(UART1_RTS, UART_GPIO_FUNCTION);//RTS
            hal_gpio_pinmux_set_function(UART1_CTS, UART_GPIO_FUNCTION);//CTS
            break;
        case UART_2:
            hal_gpio_pinmux_set_function(UART2_TX, UART_GPIO_FUNCTION);//TX
            hal_gpio_pinmux_set_function(UART2_RX, UART_GPIO_FUNCTION);//RX
            hal_gpio_pinmux_set_function(UART2_RTS, UART_GPIO_FUNCTION);//RTS
            hal_gpio_pinmux_set_function(UART2_CTS, UART_GPIO_FUNCTION);//CTS
            break;
        case UART_3:
            hal_gpio_pinmux_set_function(UART3_TX, UART_GPIO_FUNCTION);//TX
            hal_gpio_pinmux_set_function(UART3_RX, UART_GPIO_FUNCTION);//RX
            hal_gpio_pinmux_set_function(UART3_RTS, UART_GPIO_FUNCTION);//RTS
            hal_gpio_pinmux_set_function(UART3_CTS, UART_GPIO_FUNCTION);//CTS
            break;
        default:
            UART_ERR("[uart%d] not support \n", uart_port);
            break;
    }
}

static void uart_pinctrl_uninit(uart_port_t uart_port)
{
    switch (uart_port)
    {
        case UART_0:
            hal_gpio_pinmux_set_function(UART0_TX, GPIO_MUXSEL_DISABLED);//TX
            hal_gpio_pinmux_set_function(UART0_RX, GPIO_MUXSEL_DISABLED);//RX
            break;
        case UART_1:
            hal_gpio_pinmux_set_function(UART1_TX, GPIO_MUXSEL_DISABLED);//TX
            hal_gpio_pinmux_set_function(UART1_RX, GPIO_MUXSEL_DISABLED);//RX
            break;
        case UART_2:
            hal_gpio_pinmux_set_function(UART2_TX, GPIO_MUXSEL_DISABLED);//TX
            hal_gpio_pinmux_set_function(UART2_RX, GPIO_MUXSEL_DISABLED);//RX
            break;
        case UART_3:
            hal_gpio_pinmux_set_function(UART3_TX, GPIO_MUXSEL_DISABLED);//TX
            hal_gpio_pinmux_set_function(UART3_RX, GPIO_MUXSEL_DISABLED);//RX
            break;
        default:
            UART_ERR("[uart%d] not support \n", uart_port);
            break;
    }
}

_uart_config_t uart_defconfig =
{
    .baudrate    = UART_BAUDRATE_115200,
    .word_length = UART_WORD_LENGTH_8,
    .stop_bit    = UART_STOP_BIT_1,
    .parity      = UART_PARITY_NONE,
};//defult uart config

int32_t hal_uart_initialize(int32_t uart_port)
{
    uart_priv_t *uart_priv = &g_uart_priv[uart_port];
    uint32_t irqn = g_uart_irqn[uart_port];
    uint32_t reg_val, value = 0;
    _uart_config_t *uart_config = &uart_defconfig;
    static char uart_name[12] = {0};

    UART_INIT("Initializing uart%ld \n", uart_port);
    if ((!uart_port_is_valid(uart_port)) ||
        (!uart_config_is_valid(uart_config)))
    {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    /* enable clk */
    uart_clk_init(uart_port, true);

    /* request gpio */
    uart_pinctrl_init(uart_port);

    /* config uart attributes */
    uart_set_format(uart_port, uart_config->word_length,
                    uart_config->stop_bit, uart_config->parity);
    uart_set_baudrate(uart_port, uart_config->baudrate);

    value |= UART_FCR_RXTRG_1_2 | UART_FCR_TXTRG_1_2 | UART_FCR_FIFO_EN;
    uart_set_fifo(uart_port, value);

    sprintf(uart_name, "uart%ld", uart_port);
    if (uart_priv->uart_port == uart_port && uart_priv->irqn == irqn)
    {
        UART_ERR("irq for uart%ld already enabled\n", uart_port);
    }
    else
    {
        uart_priv->uart_port = uart_port;
        uart_priv->irqn = irqn;
        if (request_irq(irqn, uart_irq_handler, 0, uart_name, uart_priv) < 0)
        {
            UART_ERR("request irq error\n");
            return -1;
        }

        enable_irq(irqn);
    }

    hal_mailbox_create(uart_name, UART_FIFO_SIZE, (hal_mailbox_t *)&uart_mailbox[uart_port]);
    /* set uart IER */
    uart_enable_irq(uart_port, UART_IER_RDI | UART_IER_RLSI);

    /* force config */
    uart_enable_busy_cfg(uart_port);

    return SUNXI_HAL_OK;
}

int32_t hal_uart_uninitialize(int32_t uart_port)
{
    /* disable clk */
    uart_clk_init(uart_port, false);
    uart_pinctrl_uninit(uart_port);
    return SUNXI_HAL_OK;
}

static int32_t power_control(int32_t dev, sunxi_hal_power_state_e state)
{
    return SUNXI_HAL_OK;
}

static int _uart_putc(int devid, char c)
{
    volatile uint32_t *sed_buf;
    volatile uint32_t *sta;

    sed_buf = (uint32_t *)(sunxi_uart_port[devid] + UART_THR);
    sta = (uint32_t *)(sunxi_uart_port[devid] + UART_USR);

    /* FIFO status, contain valid data */
    while (!(*sta & 0x02));
    *sed_buf = c;

    return 1;
}

int32_t hal_uart_send(int32_t dev, const char *data, uint32_t num)
{
    int size;

    hal_assert(data != NULL);

    size = num;
    while (num)
    {
        _uart_putc(dev, *data);

        ++ data;
        -- num;
    }

    return size - num;
}

static int _uart_getc(int devid)
{
    int ch = -1;
    volatile uint32_t *rec_buf;
    volatile uint32_t *sta;
    volatile uint32_t *fifo;

    rec_buf = (uint32_t *)(sunxi_uart_port[devid] + UART_RHB);
    sta = (uint32_t *)(sunxi_uart_port[devid] + UART_USR);
    fifo = (uint32_t *)(sunxi_uart_port[devid] + UART_RFL);

    while (!(*fifo & 0x1ff));

    /* Receive Data Available */
    if (*sta & 0x08)
    {
        ch = *rec_buf & 0xff;
    }

    return ch;
}

static int32_t receive_polling(int32_t dev, int *data, uint32_t num)
{
    int ch;
    int size;

    hal_assert(data != NULL);
    size = num;

    while (num)
    {
        ch = _uart_getc(dev);
        if (ch == -1)
        {
            break;
        }

        *data = ch;
        data ++;
        num --;

        if (ch == '\n')
        {
            break;
        }
    }

    return size - num;
}

int32_t hal_uart_receive(int32_t dev, int *data, uint32_t num)
{
    uint32_t *pdata = (uint32_t *)data;
    int i = 0;
    int32_t ret = -1, rev_count = 0;

    hal_assert(data != NULL);

    for (i = 0; i < num; i++)
    {
        ret = hal_mailbox_recv((hal_mailbox_t)uart_mailbox[dev], (unsigned int *)&pdata[i], -1);
        if (ret == 0)
        {
            rev_count++;
        }
        else
        {
            __err("receive error");
            break;
        }
    }

    return rev_count;
}

static int32_t transfer(int32_t dev, const void *data_out, void *data_in, uint32_t num)
{
    return SUNXI_HAL_OK;
}

static uint32_t get_tx_count(int32_t dev)
{
    //TODO: need verify
    return 0;
}

static uint32_t get_rx_count(int32_t dev)
{
    //TODO: need verify
    return 0;
}

static sunxi_hal_usart_modem_status_t get_modem_status(int32_t uart_port)
{
    sunxi_hal_usart_modem_status_t status = {0};
    const unsigned long uart_base = sunxi_uart_port[uart_port];
    uart_priv_t *uart_priv = &g_uart_priv[uart_port];
    uint32_t value;

    value = hal_readb(uart_base + UART_MSR);
    memcpy(&status, &value, sizeof(value));

    return status;
}

int32_t hal_uart_control(int32_t uart_port, int cmd, void *args)
{
    _uart_config_t *uart_config;
    int32_t ret = -1;

    switch(cmd)
    {
        case UART_CMD_SET_CONFIG:
            uart_config = (_uart_config_t *)args;

            /* config uart attributes */
            uart_set_format(uart_port, uart_config->word_length,
                uart_config->stop_bit, uart_config->parity);
            uart_set_baudrate(uart_port, uart_config->baudrate);
            ret = SUNXI_HAL_OK;
            break;
        case UART_CMD_ENABLE_HW_FLOW_CTL:
            hal_uart_set_hardware_flowcontrol(uart_port);
            ret = SUNXI_HAL_OK;
            break;
        case UART_CMD_DISABLE_HW_FLOW_CTL:
            hal_uart_disable_flowcontrol(uart_port);
            ret = SUNXI_HAL_OK;
            break;
        case UART_CMD_GET_MODEM_STATUS:
            *(sunxi_hal_usart_modem_status_t *)args = get_modem_status(uart_port);
            ret = SUNXI_HAL_OK;
            break;
        default:
            break;
    }

    return ret;
}

static sunxi_hal_usart_status_t get_status(int32_t dev)
{
    sunxi_hal_usart_status_t status = {1, 1, 0, 0, 0, 0, 0, 0};
    return status;
}

static int32_t set_modem_control(int32_t dev, sunxi_hal_usart_modem_control_e control)
{
    return SUNXI_HAL_OK;
}

const sunxi_hal_driver_usart_t sunxi_hal_usart_driver =
{
    .get_version  = get_version,
    .get_capabilities = get_capabilities,
    .initialize = hal_uart_initialize,
    .uninitialize = hal_uart_uninitialize,
    .power_control = power_control,
    .send = hal_uart_send,
    .receive = hal_uart_receive,
    .transfer = transfer,
    .get_tx_count = get_tx_count,
    .get_rx_count = get_rx_count,
    .control = hal_uart_control,
    .get_status = get_status,
    .set_modem_control = set_modem_control,
    .get_modem_status = get_modem_status,
    .receive_polling = receive_polling,
#ifdef CONFIG_SUNXI_UART_SUPPORT_POLL
    .poll_ops = &uart_poll_ops,
#endif
};

int serial_driver_init(void)
{
    __log("serial hal driver init");
    return 0;
}

//late_initcall(serial_driver_init);
