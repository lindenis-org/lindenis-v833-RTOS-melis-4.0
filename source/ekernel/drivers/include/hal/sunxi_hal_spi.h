/*
 * ===========================================================================================
 *
 *       Filename:  sunxi_hal_spi.h
 *
 *    Description:  SPI HAL definition.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-18 11:11:56
 *       Revision:  none
 *       Compiler:  GCC:version 9.2.1
 *
 *         Author:  bantao@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2019-12-03 16:02:11
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_SPI_H
#define SUNXI_HAL_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sunxi_hal_common.h"
#include <sunxi_hal_gpio.h>
#include <sunxi_hal_dma.h>
/* #include <sunxi_drv_spi.h> */
#include <hal_sem.h>
#include <hal_mutex.h>
#include <hal_clk.h>


#define SUNXI_SPI_REG_SIZE    0x1000 /* controler reg sized */


#define SUNXI_SPI0_PBASE         0X05010000 /* 4K */
#define SUNXI_SPI1_PBASE         0X05011000 /* 4K */
#define SUNXI_SPI2_PBASE         0X05012000 /* 4K */
#define SUNXI_IRQ_SPI0                 86

#define SPI_MUXSEL 4
#define SPI_DRVSEL 2

#define SPI0_NUM 6
#define SPI0_CLK    GPIO_PC0
#define SPI0_MOSI   GPIO_PC2
#define SPI0_MISO   GPIO_PC3
#define SPI0_CS0    GPIO_PC1
#define SPI0_WP     GPIO_PC4
#define SPI0_HOLD   GPIO_PC5

#define SPI1_NUM 4
#define SPI1_CLK    GPIO_PH0
#define SPI1_MOSI   GPIO_PH1
#define SPI1_MISO   GPIO_PH2
#define SPI1_CS0    GPIO_PH3

#define SPI2_NUM 4
#define SPI2_CLK    GPIO_PE18
#define SPI2_MOSI   GPIO_PE19
#define SPI2_MISO   GPIO_PE20
#define SPI2_CS0    GPIO_PE21

#define HEXADECIMAL (0x10)
#define REG_INTERVAL (0x04)
#define REG_CL (0x0c)

#define SPI_FIFO_DEPTH (128)
#define MAX_FIFU 64
#define BULK_DATA_BOUNDARY 64       /* can modify to adapt the application */
#define SPI_MAX_FREQUENCY 100000000 /* spi controller just support 100Mhz */
#define SPI_HIGH_FREQUENCY 60000000 /* sample mode threshold frequency  */
#define SPI_LOW_FREQUENCY 24000000  /* sample mode threshold frequency  */
#define SPI_MOD_CLK 100000000    /* sample mode frequency  */

/* SPI Registers offsets from peripheral base address */
#define SPI_VER_REG (0x00)      /* version number register */
#define SPI_GC_REG (0x04)       /* global control register */
#define SPI_TC_REG (0x08)       /* transfer control register */
#define SPI_INT_CTL_REG (0x10)  /* interrupt control register */
#define SPI_INT_STA_REG (0x14)  /* interrupt status register */
#define SPI_FIFO_CTL_REG (0x18) /* fifo control register */
#define SPI_FIFO_STA_REG (0x1C) /* fifo status register */
#define SPI_WAIT_CNT_REG (0x20) /* wait clock counter register */
#define SPI_CLK_CTL_REG                                                        \
    (0x24) /* clock rate control register. better not to use it */
#define SPI_BURST_CNT_REG (0x30)    /* burst counter register */
#define SPI_TRANSMIT_CNT_REG (0x34) /* transmit counter register */
#define SPI_BCC_REG (0x38)    /* burst control counter register */
#define SPI_DMA_CTL_REG (0x88)      /* DMA control register, only for 1639 */
#define SPI_TXDATA_REG (0x200)      /* tx data register */
#define SPI_RXDATA_REG (0x300)      /* rx data register */

/* SPI Global Control Register Bit Fields & Masks,default value:0x0000_0080 */
#define SPI_GC_EN                                                              \
    (0x1                                                                   \
     << 0) /* SPI module enable control 1:enable; 0:disable; default:0 */
#define SPI_GC_MODE                                                            \
    (0x1 << 1) /* SPI function mode select 1:master; 0:slave; default:0 */
#define SPI_GC_TP_EN                                                           \
    (0x1 << 7) /* SPI transmit stop enable 1:stop transmit data when       \
RXFIFO is full; 0:ignore RXFIFO status; default:1 */
#define SPI_GC_SRST                                                            \
    (0x1 << 31) /* soft reset, write 1 will clear SPI control, auto     \
clear to 0 */
/* SPI Transfer Control Register Bit Fields & Masks,default value:0x0000_0087 */
#define SPI_TC_PHA                                                             \
    (0x1 << 0) /* SPI Clock/Data phase control,0: phase0,1:                \
phase1;default:1 */
#define SPI_TC_POL                                                             \
    (0x1 << 1) /* SPI Clock polarity control,0:low level idle,1:high    \
level idle;default:1 */
#define SPI_TC_SPOL                                                            \
    (0x1 << 2) /* SPI Chip select signal polarity control,default: 1,low   \
effective like this:~~|_____~~ */
#define SPI_TC_SSCTL                                                           \
    (0x1                                                                   \
     << 3) /* SPI chip select control,default 0:SPI_SSx remains asserted   \
between SPI bursts,1:negate SPI_SSx between SPI bursts */
#define SPI_TC_SS_MASK                                                         \
    (0x3 << 4) /* SPI chip                                                 \
select:00-SPI_SS0;01-SPI_SS1;10-SPI_SS2;11-SPI_SS3*/
#define SPI_TC_SS_OWNER                                                        \
    (0x1 << 6) /* SS output mode select default is 0:automatic output      \
SS;1:manual output SS */
#define SPI_TC_SS_LEVEL                                                        \
    (0x1 << 7) /* defautl is 1:set SS to high;0:set SS to low */
#define SPI_TC_DHB                                                             \
    (0x1                                                                   \
     << 8) /* Discard Hash Burst,default 0:receiving all spi burst in BC   \
period 1:discard unused,fectch WTC bursts */
#define SPI_TC_DDB                                                             \
    (0x1 << 9) /* Dummy burst Type,default 0: dummy spi burst is           \
zero;1:dummy spi burst is one */
#define SPI_TC_RPSM                                                            \
    (0x1 << 10) /* select mode for high speed write,0:normal write         \
mode,1:rapids write mode,default 0 */
#define SPI_TC_SDM                                                             \
    (0x1 << 13) /* master sample data mode, 1: normal sample            \
mode;0:delay sample mode. */
#define SPI_TC_SDC                                                             \
    (0x1 << 11) /* master sample data control, 1: delay--high speed        \
operation;0:no delay. */
#define SPI_TC_FBS                                                             \
    (0x1 << 12) /* LSB/MSB transfer first select 0:MSB,1:LSB,default    \
0:MSB first */
#define SPI_TC_SDDM                                                             \
    (0x1 << 14) /* master sending data delay mode, 1: delay sending    \
		   mode;0:normal sending  mode. */
#define SPI_TC_XCH                                                             \
    (0x1                                                                   \
     << 31) /* Exchange burst default 0:idle,1:start exchange;when BC is   \
zero,this bit cleared by SPI controller*/
#define SPI_TC_SS_BIT_POS (4)

/* SPI Interrupt Control Register Bit Fields & Masks,default value:0x0000_0000
*/
#define SPI_INTEN_RX_RDY                                                       \
    (0x1 << 0) /* rxFIFO Ready Interrupt Enable,---used for immediately    \
received,0:disable;1:enable */
#define SPI_INTEN_RX_EMP                                                       \
    (0x1 << 1) /* rxFIFO Empty Interrupt Enable ---used for IRQ received   \
*/
#define SPI_INTEN_RX_FULL                                                      \
    (0x1 << 2) /* rxFIFO Full Interrupt Enable ---seldom used */
#define SPI_INTEN_TX_ERQ                                                       \
    (0x1 << 4) /* txFIFO Empty Request Interrupt Enable ---seldom used */
#define SPI_INTEN_TX_EMP                                                       \
    (0x1 << 5) /* txFIFO Empty Interrupt Enable ---used  for IRQ tx */
#define SPI_INTEN_TX_FULL                                                      \
    (0x1 << 6) /* txFIFO Full Interrupt Enable ---seldom used */
#define SPI_INTEN_RX_OVF                                                       \
    (0x1                                                                   \
     << 8) /* rxFIFO Overflow Interrupt Enable ---used for error detect */
#define SPI_INTEN_RX_UDR                                                       \
    (0x1                                                                   \
     << 9) /* rxFIFO Underrun Interrupt Enable ---used for error detect */
#define SPI_INTEN_TX_OVF                                                       \
    (0x1 << 10) /* txFIFO Overflow Interrupt Enable ---used for error      \
detect */
#define SPI_INTEN_TX_UDR                                                       \
    (0x1 << 11) /* txFIFO Underrun Interrupt Enable ---not happened */
#define SPI_INTEN_TC                                                           \
    (0x1 << 12) /* Transfer Completed Interrupt Enable  ---used */
#define SPI_INTEN_SSI                                                          \
    (0x1 << 13) /* SSI interrupt Enable,chip select from valid state to    \
invalid state,for slave used only */
#define SPI_INTEN_ERR                                                          \
    (SPI_INTEN_TX_OVF | SPI_INTEN_RX_UDR |                                 \
     SPI_INTEN_RX_OVF) /* NO txFIFO underrun */
#define SPI_INTEN_MASK (0x77 | (0x3f << 8))

/* SPI Interrupt Status Register Bit Fields & Masks,default value:0x0000_0022 */
#define SPI_INT_STA_RX_RDY                                                     \
    (0x1 << 0) /* rxFIFO ready, 0:RX_WL < RX_TRIG_LEVEL,1:RX_WL >=         \
RX_TRIG_LEVEL */
#define SPI_INT_STA_RX_EMP                                                     \
    (0x1 << 1) /* rxFIFO empty, this bit is set when rxFIFO is empty */
#define SPI_INT_STA_RX_FULL                                                    \
    (0x1 << 2) /* rxFIFO full, this bit is set when rxFIFO is full */
#define SPI_INT_STA_TX_RDY                                                     \
    (0x1 << 4) /* txFIFO ready, 0:TX_WL > TX_TRIG_LEVEL,1:TX_WL <=         \
TX_TRIG_LEVEL */
#define SPI_INT_STA_TX_EMP                                                     \
    (0x1 << 5) /* txFIFO empty, this bit is set when txFIFO is empty */
#define SPI_INT_STA_TX_FULL                                                    \
    (0x1 << 6) /* txFIFO full, this bit is set when txFIFO is full */
#define SPI_INT_STA_RX_OVF                                                     \
    (0x1 << 8) /* rxFIFO overflow, when set rxFIFO has overflowed */
#define SPI_INT_STA_RX_UDR                                                     \
    (0x1 << 9) /* rxFIFO underrun, when set rxFIFO has underrun */
#define SPI_INT_STA_TX_OVF                                                     \
    (0x1 << 10) /* txFIFO overflow, when set txFIFO has overflowed */
#define SPI_INT_STA_TX_UDR                                                     \
    (0x1 << 11) /* fxFIFO underrun, when set txFIFO has underrun */
#define SPI_INT_STA_TC (0x1 << 12) /* Transfer Completed */
#define SPI_INT_STA_SSI                                                        \
    (0x1 << 13) /* SS invalid interrupt, when set SS has changed from      \
valid to invalid */
#define SPI_INT_STA_ERR                                                        \
    (SPI_INT_STA_TX_OVF | SPI_INT_STA_RX_UDR |                             \
     SPI_INT_STA_RX_OVF) /* NO txFIFO underrun */
#define SPI_INT_STA_MASK (0x77 | (0x3f << 8))

/* SPI FIFO Control Register Bit Fields & Masks,default value:0x0040_0001 */
#define SPI_FIFO_CTL_RX_LEVEL                                                  \
    (0xFF << 0) /* rxFIFO reday request trigger level,default 0x1 */
#define SPI_FIFO_CTL_RX_DRQEN                                                  \
    (0x1 << 8) /* rxFIFO DMA request enable,1:enable,0:disable */
#define SPI_FIFO_CTL_RX_TESTEN                                                 \
    (0x1 << 14) /* rxFIFO test mode enable,1:enable,0:disable */
#define SPI_FIFO_CTL_RX_RST                                                    \
    (0x1 << 15) /* rxFIFO reset, write 1, auto clear to 0 */
#define SPI_FIFO_CTL_TX_LEVEL                                                  \
    (0xFF << 16) /* txFIFO empty request trigger level,default 0x40 */
#define SPI_FIFO_CTL_TX_DRQEN                                                  \
    (0x1 << 24) /* txFIFO DMA request enable,1:enable,0:disable */
#define SPI_FIFO_CTL_TX_TESTEN                                                 \
    (0x1 << 30) /* txFIFO test mode enable,1:enable,0:disable */
#define SPI_FIFO_CTL_TX_RST                                                    \
    (0x1 << 31) /* txFIFO reset, write 1, auto clear to 0 */
#define SPI_FIFO_CTL_DRQEN_MASK (SPI_FIFO_CTL_TX_DRQEN | SPI_FIFO_CTL_RX_DRQEN)

/* SPI FIFO Status Register Bit Fields & Masks,default value:0x0000_0000 */
#define SPI_FIFO_STA_RX_CNT                                                    \
    (0xFF << 0) /* rxFIFO counter,how many bytes in rxFIFO */
#define SPI_FIFO_STA_RB_CNT                                                    \
    (0x7 << 12) /* rxFIFO read buffer counter,how many bytes in rxFIFO     \
read buffer */
#define SPI_FIFO_STA_RB_WR (0x1 << 15) /* rxFIFO read buffer write enable */
#define SPI_FIFO_STA_TX_CNT                                                    \
    (0xFF << 16) /* txFIFO counter,how many bytes in txFIFO */
#define SPI_FIFO_STA_TB_CNT                                                    \
    (0x7 << 28) /* txFIFO write buffer counter,how many bytes in txFIFO    \
write buffer */
#define SPI_FIFO_STA_TB_WR (0x1 << 31) /* txFIFO write buffer write enable */
#define SPI_RXCNT_BIT_POS (0)
#define SPI_TXCNT_BIT_POS (16)

/* SPI Wait Clock Register Bit Fields & Masks,default value:0x0000_0000 */
#define SPI_WAIT_WCC_MASK                                                      \
    (0xFFFF << 0) /* used only in master mode: Wait Between Transactions   \
*/
#define SPI_WAIT_SWC_MASK                                                      \
    (0xF << 16) /* used only in master mode: Wait before start dual data   \
transfer in dual SPI mode */

/* SPI Clock Control Register Bit Fields & Masks,default:0x0000_0002 */
#define SPI_CLK_CTL_CDR2                                                       \
    (0xFF << 0) /* Clock Divide Rate 2,master mode only : SPI_CLK =        \
AHB_CLK/(2*(n+1)) */
#define SPI_CLK_CTL_CDR1                                                       \
    (0xF << 8) /* Clock Divide Rate 1,master mode only : SPI_CLK =         \
AHB_CLK/2^n */
#define SPI_CLK_CTL_DRS                                                        \
    (0x1 << 12) /* Divide rate select,default,0:rate 1;1:rate 2 */
#define SPI_CLK_SCOPE (SPI_CLK_CTL_CDR2 + 1)

/* SPI Master Burst Counter Register Bit Fields & Masks,default:0x0000_0000 */
/* master mode: when SMC = 1,BC specifies total burst number, Max length is
 * 16Mbytes */
#define SPI_BC_CNT_MASK                                                        \
    (0xFFFFFF << 0) /* Total Burst Counter, tx length + rx length ,SMC=1   \
*/

/* SPI Master Transmit Counter reigster default:0x0000_0000 */
#define SPI_TC_CNT_MASK                                                        \
    (0xFFFFFF                                                              \
     << 0) /* Write Transmit Counter, tx length, NOT rx length!!! */

/* SPI Master Burst Control Counter reigster Bit Fields &
 * Masks,default:0x0000_0000 */
#define SPI_BCC_STC_MASK                                                       \
    (0xFFFFFF << 0)           /* master single mode transmit counter */
#define SPI_BCC_DBC_MASK (0xF << 24)  /* master dummy burst counter */
#define SPI_BCC_DUAL_MODE (0x1 << 28) /* master dual mode RX enable */
#define SPI_BCC_QUAD_MODE (0x1 << 29) /* master quad mode RX enable */

#define SPI_PHA_ACTIVE_ (0x01)
#define SPI_POL_ACTIVE_ (0x02)

#define SPI_MODE_0_ACTIVE_ (0 | 0)
#define SPI_MODE_1_ACTIVE_ (0 | SPI_PHA_ACTIVE_)
#define SPI_MODE_2_ACTIVE_ (SPI_POL_ACTIVE_ | 0)
#define SPI_MODE_3_ACTIVE_ (SPI_POL_ACTIVE_ | SPI_PHA_ACTIVE_)
#define SPI_CS_HIGH_ACTIVE_ (0x04)
#define SPI_LSB_FIRST_ACTIVE_ (0x08)
#define SPI_DUMMY_ONE_ACTIVE_ (0x10)
#define SPI_RECEIVE_ALL_ACTIVE_ (0x20)

#define SUNXI_SPI_DRQ_RX(ch) (DRQSRC_SPI0_RX + ch)
#define SUNXI_SPI_DRQ_TX(ch) (DRQDST_SPI0_TX + ch)

#define SPIM_BUSY (1)
#define SPIM_IDLE (0)

#define spim_set_idle(master_port)                                             \
    do {                                                                   \
        g_spi_master_status[master_port] = SPIM_IDLE;                  \
    } while (0)

#define SPI_MASTER_MB_LSB_FIRST (0x1UL << 3)
#define SPI_MASTER_MB_MSB_FIRST (0x0UL << 3)

#define SPI_MASTER_CPOL_0 (0x0UL << 4)
#define SPI_MASTER_CPOL_1 (0x1UL << 4)

#define SPI_MASTER_CPHA_0 (0x0UL << 5)
#define SPI_MASTER_CPHA_1 (0x1UL << 5)

#define SPI_MASTER_INT_DISABLE (0x0UL << 9)
#define SPI_MASTER_INT_ENABLE (0x1UL << 9)

#define SPI_MASTER_HALF_DUPLEX (0x0UL << 10)
#define SPI_MASTER_FULL_DUPLEX (0x1UL << 10)

#define SPI_MASTER_SLAVE_SEL_0 (0x0UL << 29)
#define SPI_MASTER_SLAVE_SEL_1 (0x1UL << 29)


/*****************************************************************************
 * spi master
 *****************************************************************************/
/** @brief This enum defines the SPI master port.
 *  This chip total has 2 SPI master port
 */
typedef enum
{
    HAL_SPI_MASTER_0 = 0, /**< spi master port 0 */
    HAL_SPI_MASTER_1 = 1, /**< spi master port 1 */
    HAL_SPI_MASTER_2 = 2, /**< spi master port 1 */
    HAL_SPI_MASTER_MAX    /**< spi master max port number\<invalid\> */
} hal_spi_master_port_t;

typedef struct
{
    const uint8_t *tx_buf; /**< Data buffer to send, */
    uint32_t tx_len; /**< The total number of bytes to send. */
    uint32_t
    tx_single_len; /**< The number of bytes to send in single mode. */
    uint8_t *rx_buf;   /**< Received data buffer, */
    uint32_t rx_len;   /**< The valid number of bytes received. */
    uint8_t tx_nbits : 3;  /**< Data buffer to send in nbits mode */
    uint8_t rx_nbits : 3;  /**< Data buffer to received in nbits mode */
    uint8_t dummy_byte;    /**< Flash send dummy byte, default 0*/
#define SPI_NBITS_SINGLE 0x01  /* 1bit transfer */
#define SPI_NBITS_DUAL 0x02    /* 2bit transfer */
#define SPI_NBITS_QUAD 0x04    /* 4bit transfer */
    uint8_t bits_per_word; /**< transfer bit_per_word */
} hal_spi_master_transfer_t;

typedef enum spi_mode_type
{
    SGLE_HALF_DUPLEX_RX, /* single mode, half duplex read */
    SGLE_HALF_DUPLEX_TX, /* single mode, half duplex write */
    DUAL_HALF_DUPLEX_RX, /* dual mode, half duplex read */
    DUAL_HALF_DUPLEX_TX, /* dual mode, half duplex write */
    QUAD_HALF_DUPLEX_RX, /* quad mode, half duplex read */
    QUAD_HALF_DUPLEX_TX, /* quad mode, half duplex write */
    FULL_DUPLEX_TX_RX,   /* full duplex read and write */
    MODE_TYPE_NULL,
} spi_mode_type_t;

typedef struct spi_dma
{
    struct dma_slave_config config;
    unsigned long *hdma;
} spi_dma_t;

typedef struct sunxi_spi
{
    int8_t result : 2;
#define SPI_XFER_READY 0
#define SPI_XFER_OK 1
#define SPI_XFER_FAILED -1

    bool sem;
    uint16_t irqnum;
    uint32_t base;
    spi_mode_type_t mode_type;

    hal_clk_id_t pclk; /* PLL clock */
    hal_clk_id_t mclk; /* spi module clock */
    uint32_t *pin;
    uint8_t pin_num : 3;
    uint8_t pin_mux : 3;
    uint8_t pin_drv : 2;

    spi_dma_t dma_rx;
    spi_dma_t dma_tx;

    char *align_dma_buf;
#define ALIGN_DMA_BUF_SIZE (4096 + 64)

    hal_sem_t xSemaphore_tx;
    hal_sem_t xSemaphore_rx;

    hal_mutex_t lock;
    char slave_mode;
    struct rt_ringbuffer *ringbuf;
    rt_thread_t slave_task;

    hal_spi_master_port_t port;
    hal_spi_master_transfer_t *transfer;
} sunxi_spi_t;

typedef enum
{
    SPI_MASTER_ERROR = -6,       /**< SPI master function error occurred. */
    SPI_MASTER_ERROR_NOMEM = -5, /**< SPI master request mem failed. */
    SPI_MASTER_ERROR_TIMEOUT = -4, /**< SPI master xfer timeout. */
    SPI_MASTER_ERROR_BUSY = -3,    /**< SPI master is busy. */
    SPI_MASTER_ERROR_PORT = -2,    /**< SPI master invalid port. */
    SPI_MASTER_INVALID_PARAMETER =
        -1,       /**< SPI master invalid input parameter. */
    SPI_MASTER_OK = 0 /**< SPI master operation completed successfully. */
} spi_master_status_t;





/** @brief selection of spi slave device connected to which cs pin of spi master
*/
typedef enum
{
    HAL_SPI_MASTER_SLAVE_0 =
        0, /**< spi slave device connect to spi master cs0 pin */
    HAL_SPI_MASTER_SLAVE_1 =
        1, /**< spi slave device connect to spi master cs1 pin */
    HAL_SPI_MASTER_SLAVE_MAX /**< spi master max cs pin number\<invalid\> */
} hal_spi_master_slave_port_t;

/** @brief SPI master clock polarity definition */
typedef enum
{
    HAL_SPI_MASTER_CLOCK_POLARITY0 = 0, /**< Clock polarity is 0 */
    HAL_SPI_MASTER_CLOCK_POLARITY1 = 2  /**< Clock polarity is 1 */
} hal_spi_master_clock_polarity_t;

/** @brief SPI master clock format definition */
typedef enum
{
    HAL_SPI_MASTER_CLOCK_PHASE0 = 0, /**< Clock format is 0 */
    HAL_SPI_MASTER_CLOCK_PHASE1 = 1  /**< Clock format is 1 */
} hal_spi_master_clock_phase_t;

/** @brief SPI master transaction bit order definition */
typedef enum
{
    HAL_SPI_MASTER_MSB_FIRST =
        0, /**< Both send and receive data transfer MSB first */
    HAL_SPI_MASTER_LSB_FIRST =
        1 /**< Both send and receive data transfer LSB first */
} hal_spi_master_bit_order_t;

/** @brief SPI master status. */
typedef enum
{
    HAL_SPI_MASTER_STATUS_ERROR =
        -6, /**< SPI master function error occurred. */
    HAL_SPI_MASTER_STATUS_ERROR_NOMEM =
        -5, /**< SPI master request mem failed. */
    HAL_SPI_MASTER_STATUS_ERROR_TIMEOUT =
        -4,                    /**< SPI master xfer timeout. */
    HAL_SPI_MASTER_STATUS_ERROR_BUSY = -3, /**< SPI master is busy. */
    HAL_SPI_MASTER_STATUS_ERROR_PORT = -2, /**< SPI master invalid port. */
    HAL_SPI_MASTER_STATUS_INVALID_PARAMETER =
        -1, /**< SPI master invalid input parameter. */
    HAL_SPI_MASTER_STATUS_OK =
        0 /**< SPI master operation completed successfully. */
} hal_spi_master_status_t;

/** @brief SPI master running status. */
typedef enum
{
    HAL_SPI_MASTER_BUSY = 0, /**< SPI master is busy. */
    HAL_SPI_MASTER_IDLE = 1  /**< SPI master is idle. */
} hal_spi_master_running_status_t;

typedef struct
{
    uint32_t clock_frequency; /**< SPI master clock frequency setting. */
    hal_spi_master_slave_port_t
    slave_port; /**< SPI slave device selection. */
    hal_spi_master_bit_order_t
    bit_order; /**< SPI master bit order setting. 0:MSB first 1:LSB
                 first*/
    hal_spi_master_clock_polarity_t
    cpol; /**< SPI master clock polarity setting. 0:Active high
            polarity(0 = Idle) 1:Active low polarity(1 = Idle) */
    hal_spi_master_clock_phase_t
    cpha; /**< SPI master clock phase setting. 0: Phase 0(Leading edge
            for sample data)  1: Phase 1(Leading edge for setup data)
            */
} hal_spi_master_config_t;

typedef enum
{
    SPI_WRITE_READ = 0, /**< SPI master is busy. */
    SPI_CONFIG = 1  /**< SPI master is idle. */
} hal_spi_transfer_cmd_t;

typedef struct sunxi_hal_driver_spi
{
    spi_master_status_t (*initialize)(hal_spi_master_port_t port, hal_spi_master_config_t *config);
    spi_master_status_t (*uninitialize)(hal_spi_master_port_t port);
    spi_master_status_t (*send)(hal_spi_master_port_t port,
                                const void *buf, uint32_t size);
    spi_master_status_t (*receive)(hal_spi_master_port_t port,
                                   void *buf, uint32_t size);
    spi_master_status_t (*control)(hal_spi_master_port_t port, hal_spi_transfer_cmd_t cmd,
                                   void *args);
    spi_master_status_t (*transfer)(hal_spi_master_port_t port, hal_spi_master_transfer_t *transfer);
} const sunxi_hal_driver_spi_t;



spi_master_status_t hal_spi_init(hal_spi_master_port_t port, hal_spi_master_config_t *config);
spi_master_status_t hal_spi_write(hal_spi_master_port_t port, const void *buf, uint32_t size);
spi_master_status_t hal_spi_uninit(hal_spi_master_port_t port);
spi_master_status_t hal_spi_read(hal_spi_master_port_t port, void *buf, uint32_t size);

#endif

