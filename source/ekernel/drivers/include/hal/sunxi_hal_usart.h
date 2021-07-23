/*
 * ===========================================================================================
 *
 *       Filename:  sunxi_usart.h
 *
 *    Description:  USART HAL definition.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-14 11:11:56
 *       Revision:  none
 *       Compiler:  GCC:version 9.2.1 20170904 (release),SUNXI_HAL/embedded-7-branch revision 255204
 *
 *         Author:  bantao@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2020-04-02 19:39:41
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_USART_H
#define SUNXI_HAL_USART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sunxi_hal_common.h"

#define SUNXI_IRQ_UART0                (81)  /* 108 uart0 interrupt */
#define SUNXI_IRQ_UART1                (82)  /* 109 uart1 interrupt */
#define SUNXI_IRQ_UART2                (83)  /* 110 uart2 interrupt */
#define SUNXI_IRQ_UART3                (84)  /* 111 uart3 interrupt */
//base register infomation
#define APB_USART0_BASE                           (0x05000000)
#define APB_USART1_BASE                           (0x05000400)
#define APB_USART2_BASE                           (0x05000800)
#define APB_USART3_BASE                           (0x05000c00)

#define UART_FIFO_SIZE                            (256)
#define UART_GPIO_FUNCTION                        (5)

#define UART0_TX    GPIOH(9)
#define UART0_RX   GPIOH(10)

#define UART1_TX    GPIOG(6)
#define UART1_RX    GPIOG(7)
#define UART1_CTS    GPIOG(5)
#define UART1_RTS    GPIOG(4)

#define UART2_TX    GPIOH(5)
#define UART2_RX   GPIOH(6)
#define UART2_CTS    GPIOH(8)
#define UART2_RTS   GPIOH(7)

#define UART3_TX    GPIOH(0)
#define UART3_RX   GPIOH(1)

#define UART3_RTS   GPIOH(3)
#define UART3_CTS   GPIOH(2)
/*
 * This enum defines return status of the UART HAL public API.
 * User should check return value after calling these APIs.
 */
typedef enum
{
    HAL_UART_STATUS_ERROR_PARAMETER = -4,      /**< Invalid user input parameter. */
    HAL_UART_STATUS_ERROR_BUSY = -3,           /**< UART port is currently in use. */
    HAL_UART_STATUS_ERROR_UNINITIALIZED = -2,  /**< UART port has not been initialized. */
    HAL_UART_STATUS_ERROR = -1,                /**< UART driver detected a common error. */
    HAL_UART_STATUS_OK = 0                     /**< UART function executed successfully. */
} hal_uart_status_t;

typedef enum
{
    UART_0 = 0,
    UART_1,
    UART_2,
    UART_3,
    UART_MAX,
} uart_port_t;

/* This enum defines baud rate of the UART frame. */
typedef enum
{
    UART_BAUDRATE_300 = 0,
    UART_BAUDRATE_600,
    UART_BAUDRATE_1200,
    UART_BAUDRATE_2400,
    UART_BAUDRATE_4800,
    UART_BAUDRATE_9600,
    UART_BAUDRATE_19200,
    UART_BAUDRATE_38400,
    UART_BAUDRATE_57600,
    UART_BAUDRATE_115200,
    UART_BAUDRATE_230400,
    UART_BAUDRATE_576000,
    UART_BAUDRATE_921600,
    UART_BAUDRATE_1000000,
    UART_BAUDRATE_1500000,
    UART_BAUDRATE_3000000,
    UART_BAUDRATE_4000000,
    UART_BAUDRATE_MAX,
} uart_baudrate_t;

/* This enum defines word length of the UART frame. */
typedef enum
{
    UART_WORD_LENGTH_5 = 0,
    UART_WORD_LENGTH_6,
    UART_WORD_LENGTH_7,
    UART_WORD_LENGTH_8,
} uart_word_length_t;

/* This enum defines stop bit of the UART frame. */
typedef enum
{
    UART_STOP_BIT_1 = 0,
    UART_STOP_BIT_2,
} uart_stop_bit_t;

/* This enum defines parity of the UART frame. */
typedef enum
{
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD,
    UART_PARITY_EVEN
} uart_parity_t;

/* This struct defines UART configure parameters. */
typedef struct
{
    uart_baudrate_t baudrate;
    uart_word_length_t word_length;
    uart_stop_bit_t stop_bit;
    uart_parity_t parity;
} _uart_config_t;


// UART HAL Layer API Version.
#define SUNXI_HAL_USART_API_VERSION               SUNXI_HAL_VERSION_MAJOR_MINOR(1, 0)

/* Driver version */
#define SUNXI_HAL_USART_DRV_VERSION               SUNXI_HAL_VERSION_MAJOR_MINOR(1, 0)

//======================================reg==========================================================//
#define UART_INVAL_DATA_IND     (0xffffffff)

#define BIT(nr)     (1UL << (nr))

/*
 * Register definitions for UART
 */
#define UART_RHB (0x00)
#define UART_RBR (0x00)         /* receive buffer register */
#define UART_THR (0x00)         /* transmit holding register */
#define UART_DLL (0x00)         /* divisor latch low register */
#define UART_DLH (0x04)         /* diviso latch high register */
#define UART_IER (0x04)         /* interrupt enable register */
#define UART_IIR (0x08)         /* interrupt identity register */
#define UART_FCR (0x08)         /* FIFO control register */
#define UART_LCR (0x0c)         /* line control register */
#define UART_MCR (0x10)         /* modem control register */
#define UART_LSR (0x14)         /* line status register */
#define UART_MSR (0x18)         /* modem status register */
#define UART_SCH (0x1c)         /* scratch register */
#define UART_USR (0x7c)         /* status register */
#define UART_TFL (0x80)         /* transmit FIFO level */
#define UART_RFL (0x84)         /* RFL */
#define UART_HALT (0xa4)        /* halt tx register */
#define UART_RS485 (0xc0)       /* RS485 control and status register */

/*
 * register bit field define
 */

/* Interrupt Enable Register */
#define UART_IER_MASK       (0xff)
#define UART_IER_PTIME      (BIT(7))
#define UART_IER_RS485      (BIT(4))
#define UART_IER_MSI        (BIT(3))
#define UART_IER_RLSI       (BIT(2))
#define UART_IER_THRI       (BIT(1))
#define UART_IER_RDI        (BIT(0))
/* Interrupt ID Register */
#define UART_IIR_FEFLAG_MASK    (BIT(6)|BIT(7))
#define UART_IIR_IID_MASK   (BIT(0)|BIT(1)|BIT(2)|BIT(3))
#define UART_IIR_IID_MSTA   (0)
#define UART_IIR_IID_NOIRQ  (1)
#define UART_IIR_IID_THREMP (2)
#define UART_IIR_IID_RXDVAL (4)
#define UART_IIR_IID_LINESTA    (6)
#define UART_IIR_IID_BUSBSY (7)
#define UART_IIR_IID_CHARTO (12)
/* FIFO Control Register */
#define UART_FCR_RXTRG_MASK (BIT(6)|BIT(7))
#define UART_FCR_RXTRG_1CH  (0 << 6)
#define UART_FCR_RXTRG_1_4  (1 << 6)
#define UART_FCR_RXTRG_1_2  (2 << 6)
#define UART_FCR_RXTRG_FULL     (3 << 6)
#define UART_FCR_TXTRG_MASK (BIT(4)|BIT(5))
#define UART_FCR_TXTRG_EMP  (0 << 4)
#define UART_FCR_TXTRG_2CH  (1 << 4)
#define UART_FCR_TXTRG_1_4  (2 << 4)
#define UART_FCR_TXTRG_1_2  (3 << 4)
#define UART_FCR_TXFIFO_RST (BIT(2))
#define UART_FCR_RXFIFO_RST (BIT(1))
#define UART_FCR_FIFO_EN    (BIT(0))
/* Line Control Register */
#define UART_LCR_DLAB       (BIT(7))
#define UART_LCR_SBC        (BIT(6))
#define UART_LCR_PARITY_MASK    (BIT(5)|BIT(4))
#define UART_LCR_EPAR       (1 << 4)
#define UART_LCR_OPAR       (0 << 4)
#define UART_LCR_PARITY     (BIT(3))
#define UART_LCR_STOP       (BIT(2))
#define UART_LCR_DLEN_MASK  (BIT(1)|BIT(0))
#define UART_LCR_WLEN5      (0)
#define UART_LCR_WLEN6      (1)
#define UART_LCR_WLEN7      (2)
#define UART_LCR_WLEN8      (3)
/* Modem Control Register */
#define UART_MCR_MODE_MASK  (BIT(7)|BIT(6))
#define UART_MCR_MODE_RS485 (2 << 6)
#define UART_MCR_MODE_SIRE  (1 << 6)
#define UART_MCR_MODE_UART  (0 << 6)
#define UART_MCR_AFE        (BIT(5))
#define UART_MCR_LOOP       (BIT(4))
#define UART_MCR_RTS        (BIT(1))
#define UART_MCR_DTR        (BIT(0))
/* Line Status Rigster */
#define UART_LSR_RXFIFOE    (BIT(7))
#define UART_LSR_TEMT       (BIT(6))
#define UART_LSR_THRE       (BIT(5))
#define UART_LSR_BI     (BIT(4))
#define UART_LSR_FE     (BIT(3))
#define UART_LSR_PE     (BIT(2))
#define UART_LSR_OE     (BIT(1))
#define UART_LSR_DR     (BIT(0))
#define UART_LSR_BRK_ERROR_BITS (0x1E) /* BI, FE, PE, OE bits */
/* Modem Status Register */
#define UART_MSR_DCD        (BIT(7))
#define UART_MSR_RI     (BIT(6))
#define UART_MSR_DSR        (BIT(5))
#define UART_MSR_CTS        (BIT(4))
#define UART_MSR_DDCD       (BIT(3))
#define UART_MSR_TERI       (BIT(2))
#define UART_MSR_DDSR       (BIT(1))
#define UART_MSR_DCTS       (BIT(0))
#define UART_MSR_ANY_DELTA  (0x0F)
#define MSR_SAVE_FLAGS      (UART_MSR_ANY_DELTA)
/* Status Register */
#define UART_USR_RFF        (BIT(4))
#define UART_USR_RFNE       (BIT(3))
#define UART_USR_TFE        (BIT(2))
#define UART_USR_TFNF       (BIT(1))
#define UART_USR_BUSY       (BIT(0))
/* Halt Register */
#define UART_HALT_LCRUP     (BIT(2))
#define UART_HALT_FORCECFG  (BIT(1))
#define UART_HALT_HTX       (BIT(0))
/* RS485 Control and Status Register */
#define UART_RS485_RXBFA    (BIT(3))
#define UART_RS485_RXAFA    (BIT(2))

//=================================reg===================================================//
/*
 * brief USART Status
 */
typedef struct sunxi_hal_usart_status
{
    uint32_t tx_busy          : 1;        ///< Transmitter busy flag
    uint32_t rx_busy          : 1;        ///< Receiver busy flag
    uint32_t tx_underflow     : 1;        ///< Transmit data underflow detected (cleared on start of next send operation)
    uint32_t rx_overflow      : 1;        ///< Receive data overflow detected (cleared on start of next receive operation)
    uint32_t rx_break         : 1;        ///< Break detected on receive (cleared on start of next receive operation)
    uint32_t rx_framing_error : 1;        ///< Framing error detected on receive (cleared on start of next receive operation)
    uint32_t rx_parity_error  : 1;        ///< Parity error detected on receive (cleared on start of next receive operation)
    uint32_t reserved         : 25;
} sunxi_hal_usart_status_t;

/*
 *brief USART Modem Control
 */
typedef enum sunxi_hal_usart_modem_control
{
    SUNXI_HAL_USART_RTS_CLEAR,            ///< Deactivate RTS
    SUNXI_HAL_USART_RTS_SET,              ///< Activate RTS
    SUNXI_HAL_USART_DTR_CLEAR,            ///< Deactivate DTR
    SUNXI_HAL_USART_DTR_SET               ///< Activate DTR
} sunxi_hal_usart_modem_control_e;

/*
 *brief USART Modem Status
 */
typedef struct sunxi_hal_usart_modem_status
{
    uint32_t cts      : 1;                ///< CTS state: 1=Active, 0=Inactive
    uint32_t dsr      : 1;                ///< DSR state: 1=Active, 0=Inactive
    uint32_t dcd      : 1;                ///< DCD state: 1=Active, 0=Inactive
    uint32_t ri       : 1;                ///< RI  state: 1=Active, 0=Inactive
    uint32_t reserved : 28;
} sunxi_hal_usart_modem_status_t;

/****** USART Event *****/
#define SUNXI_HAL_USART_EVENT_SEND_COMPLETE       (1UL << 0)  ///< Send completed; however USART may still transmit data
#define SUNXI_HAL_USART_EVENT_RECEIVE_COMPLETE    (1UL << 1)  ///< Receive completed
#define SUNXI_HAL_USART_EVENT_TRANSFER_COMPLETE   (1UL << 2)  ///< Transfer completed
#define SUNXI_HAL_USART_EVENT_TX_COMPLETE         (1UL << 3)  ///< Transmit completed (optional)
#define SUNXI_HAL_USART_EVENT_TX_UNDERFLOW        (1UL << 4)  ///< Transmit data not available (Synchronous Slave)
#define SUNXI_HAL_USART_EVENT_RX_OVERFLOW         (1UL << 5)  ///< Receive data overflow
#define SUNXI_HAL_USART_EVENT_RX_TIMEOUT          (1UL << 6)  ///< Receive character timeout (optional)
#define SUNXI_HAL_USART_EVENT_RX_BREAK            (1UL << 7)  ///< Break detected on receive
#define SUNXI_HAL_USART_EVENT_RX_FRAMING_ERROR    (1UL << 8)  ///< Framing error detected on receive
#define SUNXI_HAL_USART_EVENT_RX_PARITY_ERROR     (1UL << 9)  ///< Parity error detected on receive
#define SUNXI_HAL_USART_EVENT_CTS                 (1UL << 10) ///< CTS state changed (optional)
#define SUNXI_HAL_USART_EVENT_DSR                 (1UL << 11) ///< DSR state changed (optional)
#define SUNXI_HAL_USART_EVENT_DCD                 (1UL << 12) ///< DCD state changed (optional)
#define SUNXI_HAL_USART_EVENT_RI                  (1UL << 13) ///< RI  state changed (optional)




/* This enum defines the UART event when an interrupt occurs. */
typedef enum
{
    UART_EVENT_TRANSACTION_ERROR = -1,
    UART_EVENT_RX_BUFFER_ERROR = -2,
    UART_EVENT_TX_COMPLETE = 1,
    UART_EVENT_RX_COMPLETE = 2,
} uart_callback_event_t;

/** @brief This typedef defines user's callback function prototype.
 *             This callback function will be called in UART interrupt handler when UART interrupt is raised.
 *             User should call uart_register_callback() to register callbacks to UART driver explicitly.
 *             Note, that the callback function is not appropriate for time-consuming operations. \n
 *             parameter "event" : for more information, please refer to description of #uart_callback_event_t.
 *             parameter "user_data" : a user defined data used in the callback function.
 */
typedef void (*uart_callback_t)(uart_callback_event_t event, void *user_data);
typedef struct
{
    uint8_t *buf;
    uint32_t len;
    uint32_t head;
    uint32_t tail;
    int32_t cnt;
} uart_ring_buf_t;
/* This struct defines UART private data */
typedef struct
{
    /* basic info */
    uart_port_t uart_port;
    uint32_t irqn;

    /* uart register value */
    unsigned char ier;
    unsigned char lcr;
    unsigned char mcr;
    unsigned char fcr;
    unsigned char dll;
    unsigned char dlh;

    /* tx & rx buf */
    const char *tx_buf;
    uint32_t tx_buf_size;
    /* rx ring buf */
    uart_ring_buf_t ring_buf;

    /* user callback */
    uart_callback_t func;
    void *arg;
} uart_priv_t;

/**
  \fn          sunxi_hal_version_t SUNXI_HAL_USART_GetVersion (void)
  \brief       Get driver version.
  \return      \ref sunxi_hal_version_t

  \fn          SUNXI_HAL_USART_CAPABILITIES SUNXI_HAL_USART_GetCapabilities (void)
  \brief       Get driver capabilities
  \return      \ref SUNXI_HAL_USART_CAPABILITIES

  \fn          int32_t SUNXI_HAL_USART_Initialize (SUNXI_HAL_USART_SignalEvent_t cb_event)
  \brief       Initialize USART Interface.
  \param[in]   cb_event  Pointer to \ref SUNXI_HAL_USART_SignalEvent
  \return      \ref execution_status

  \fn          int32_t SUNXI_HAL_USART_Uninitialize (void)
  \brief       De-initialize USART Interface.
  \return      \ref execution_status

  \fn          int32_t SUNXI_HAL_USART_PowerControl (SUNXI_HAL_POWER_STATE state)
  \brief       Control USART Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status

  \fn          int32_t SUNXI_HAL_USART_Send (const void *data, uint32_t num)
  \brief       Start sending data to USART transmitter.
  \param[in]   data  Pointer to buffer with data to send to USART transmitter
  \param[in]   num   Number of data items to send
  \return      \ref execution_status

  \fn          int32_t SUNXI_HAL_USART_Receive (void *data, uint32_t num)
  \brief       Start receiving data from USART receiver.
  \param[out]  data  Pointer to buffer for data to receive from USART receiver
  \param[in]   num   Number of data items to receive
  \return      \ref execution_status

  \fn          int32_t SUNXI_HAL_USART_Transfer (const void *data_out,
                                                 void *data_in,
                                           uint32_t    num)
  \brief       Start sending/receiving data to/from USART transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
  \param[in]   num       Number of data items to transfer
  \return      \ref execution_status

  \fn          uint32_t SUNXI_HAL_USART_GetTxCount (void)
  \brief       Get transmitted data count.
  \return      number of data items transmitted

  \fn          uint32_t SUNXI_HAL_USART_GetRxCount (void)
  \brief       Get received data count.
  \return      number of data items received

  \fn          int32_t SUNXI_HAL_USART_Control (uint32_t control, uint32_t arg)
  \brief       Control USART Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      common \ref execution_status and driver specific \ref usart_execution_status

  \fn          SUNXI_HAL_USART_STATUS SUNXI_HAL_USART_GetStatus (void)
  \brief       Get USART status.
  \return      USART status \ref SUNXI_HAL_USART_STATUS

  \fn          int32_t SUNXI_HAL_USART_SetModemControl (SUNXI_HAL_USART_MODEM_CONTROL control)
  \brief       Set USART Modem Control line state.
  \param[in]   control  \ref SUNXI_HAL_USART_MODEM_CONTROL
  \return      \ref execution_status

  \fn          SUNXI_HAL_USART_MODEM_STATUS SUNXI_HAL_USART_GetModemStatus (void)
  \brief       Get USART Modem Status lines state.
  \return      modem status \ref SUNXI_HAL_USART_MODEM_STATUS

  \fn          void SUNXI_HAL_USART_SignalEvent (uint32_t event)
  \brief       Signal USART Events.
  \param[in]   event  \ref USART_events notification mask
  \return      none
*/

typedef void (*sunxi_hal_usart_signal_event_t)(uint32_t event);   ///< Pointer to \ref SUNXI_HAL_USART_SignalEvent : Signal USART Event.


/**
\brief USART Device Driver Capabilities.
*/
typedef struct sunxi_hal_usart_capabilities
{
    uint32_t asynchronous       : 1;      ///< supports UART (Asynchronous) mode
    uint32_t synchronous_master : 1;      ///< supports Synchronous Master mode
    uint32_t synchronous_slave  : 1;      ///< supports Synchronous Slave mode
    uint32_t single_wire        : 1;      ///< supports UART Single-wire mode
    uint32_t irda               : 1;      ///< supports UART IrDA mode
    uint32_t smart_card         : 1;      ///< supports UART Smart Card mode
    uint32_t smart_card_clock   : 1;      ///< Smart Card Clock generator available
    uint32_t flow_control_rts   : 1;      ///< RTS Flow Control available
    uint32_t flow_control_cts   : 1;      ///< CTS Flow Control available
    uint32_t event_tx_complete  : 1;      ///< Transmit completed event: \ref SUNXI_HAL_USART_EVENT_TX_COMPLETE
    uint32_t event_rx_timeout   : 1;      ///< Signal receive character timeout event: \ref SUNXI_HAL_USART_EVENT_RX_TIMEOUT
    uint32_t rts                : 1;      ///< RTS Line: 0=not available, 1=available
    uint32_t cts                : 1;      ///< CTS Line: 0=not available, 1=available
    uint32_t dtr                : 1;      ///< DTR Line: 0=not available, 1=available
    uint32_t dsr                : 1;      ///< DSR Line: 0=not available, 1=available
    uint32_t dcd                : 1;      ///< DCD Line: 0=not available, 1=available
    uint32_t ri                 : 1;      ///< RI Line: 0=not available, 1=available
    uint32_t event_cts          : 1;      ///< Signal CTS change event: \ref SUNXI_HAL_USART_EVENT_CTS
    uint32_t event_dsr          : 1;      ///< Signal DSR change event: \ref SUNXI_HAL_USART_EVENT_DSR
    uint32_t event_dcd          : 1;      ///< Signal DCD change event: \ref SUNXI_HAL_USART_EVENT_DCD
    uint32_t event_ri           : 1;      ///< Signal RI change event: \ref SUNXI_HAL_USART_EVENT_RI
    uint32_t reserved           : 11;     ///< Reserved (must be zero)
} sunxi_hal_usart_capabilities_t;


typedef enum
{
    UART_CMD_SET_CONFIG = 0,
    UART_CMD_ENABLE_HW_FLOW_CTL,
    UART_CMD_DISABLE_HW_FLOW_CTL,
    UART_CMD_GET_MODEM_STATUS,
} UART_CTL_CMD;

/*
 *brief Access structure of the USART Driver.
 */
typedef struct sunxi_hal_driver_usart
{

    ///< Pointer to \ref SUNXI_HAL_USART_GetVersion : Get driver version.
    sunxi_hal_version_t (*get_version)(int32_t dev);

    ///< Pointer to \ref SUNXI_HAL_USART_GetCapabilities : Get driver capabilities.
    sunxi_hal_usart_capabilities_t (*get_capabilities)(int32_t dev);

    ///< Pointer to \ref SUNXI_HAL_USART_Initialize : Initialize USART Interface.
    int32_t (*initialize)(int32_t uart_port);

    ///< Pointer to \ref SUNXI_HAL_USART_Uninitialize : De-initialize USART Interface.
    int32_t (*uninitialize)(int32_t uart_port);

    ///< Pointer to \ref SUNXI_HAL_USART_PowerControl : Control USART Interface Power.
    int32_t (*power_control)(int32_t dev, sunxi_hal_power_state_e state);

    ///< Pointer to \ref SUNXI_HAL_USART_Send : Start sending data to USART transmitter.
    int32_t (*send)(int32_t dev, const char *data, uint32_t num);

    ///< Pointer to \ref SUNXI_HAL_USART_Receive : Start receiving data from USART receiver.
    int32_t (*receive)(int32_t dev, int *data, uint32_t num);

    ///< Pointer to \ref SUNXI_HAL_USART_Transfer : Start sending/receiving data to/from USART.
    int32_t (*transfer)(int32_t dev, const void *data_out, void *data_in, uint32_t    num);

    ///< Pointer to \ref SUNXI_HAL_USART_GetTxCount : Get transmitted data count.
    uint32_t (*get_tx_count)(int32_t dev);

    ///< Pointer to \ref SUNXI_HAL_USART_GetRxCount : Get received data count.
    uint32_t (*get_rx_count)(int32_t dev);

    ///< Pointer to \ref SUNXI_HAL_USART_Control : Control USART Interface.
    int32_t (*control)(int32_t uart_port, int cmd, void *args);

    ///< Pointer to \ref SUNXI_HAL_USART_GetStatus : Get USART status.
    sunxi_hal_usart_status_t (*get_status)(int32_t dev);
    ///< Pointer to \ref SUNXI_HAL_USART_SetModemControl : Set USART Modem Control line state.
    int32_t (*set_modem_control)(int32_t dev, sunxi_hal_usart_modem_control_e control);

    ///< Pointer to \ref SUNXI_HAL_USART_GetModemStatus : Get USART Modem Status lines state.
    sunxi_hal_usart_modem_status_t (*get_modem_status)(int32_t dev);

    int32_t (*receive_polling)(int32_t dev, int *data, uint32_t num);
    sunxi_hal_poll_ops *poll_ops;
} const sunxi_hal_driver_usart_t;

int32_t hal_uart_initialize(int32_t uart_port);
int32_t hal_uart_uninitialize(int32_t uart_port);
int32_t hal_uart_send(int32_t dev, const char *data, uint32_t num);
int32_t hal_uart_receive(int32_t dev, int *data, uint32_t num);
int32_t hal_uart_control(int32_t uart_port, int cmd, void *args);
void hal_uart_set_hardware_flowcontrol(uart_port_t uart_port);
void hal_uart_disable_flowcontrol(uart_port_t uart_port);

#ifdef __cplusplus
}
#endif

#endif
