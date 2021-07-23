#ifndef SUNXI_HAL_TWI_H
#define SUNXI_HAL_TWI_H

#include "hal_sem.h"
#include "hal_clk.h"
#include "sunxi_hal_common.h"
#include "sunxi_hal_gpio.h"
#include "sunxi_hal_regulator.h"

#ifdef __cplusplus
extern "C" {
#endif

//for debug
#define CONFIG_DRIVERS_TWI_DEBUG
#ifndef CONFIG_DRIVERS_TWI_DEBUG
#define TWI_INFO(fmt, arg...) __log(fmt, ##arg)
#else
#define TWI_INFO(fmt, arg...) do {}while(0)
#endif

#define TWI_ERR(fmt, arg...) __err(fmt, ##arg)
/** the base address of TWI*/
#define SUNXI_TWI0_PBASE 0x05002000
#define SUNXI_TWI1_PBASE 0x05002400
#define SUNXI_TWI2_PBASE 0x05002800
#define SUNXI_TWI3_PBASE 0x05002c00
#define SUNXI_S_TWI0_PBASE 0x07081400

/** the irq of each TWI **/
#define SUNXI_GIC_START 32
#define SUNXI_IRQ_TWI0 (SUNXI_GIC_START + 41)
#define SUNXI_IRQ_TWI1 (SUNXI_GIC_START + 42)
#define SUNXI_IRQ_TWI2 (SUNXI_GIC_START + 43)
#define SUNXI_IRQ_TWI3 (SUNXI_GIC_START + 44)
#define SUNXI_IRQ_S_TWI0 (SUNXI_GIC_START + 107)

/* TWI Register Offset */
#define TWI_ADDR_REG        (0x00)  /*  31:8bit reserved,7-1bit for slave addr,0 bit for GCE */
#define TWI_XADDR_REG       (0x04)  /*  31:8bit reserved,7-0bit for second addr in 10bit addr */
#define TWI_DATA_REG        (0x08)  /*  31:8bit reserved, 7-0bit send or receive data byte */
#define TWI_CTL_REG         (0x0C)  /*  INT_EN,BUS_EN,M_STA,INT_FLAG,A_ACK */
#define TWI_STAT_REG        (0x10)  /*  28 interrupt types + 0xF8 normal type = 29  */
#define TWI_CLK_REG         (0x14)  /*  31:7bit reserved,6-3bit,CLK_M,2-0bit CLK_N */
#define TWI_SRST_REG        (0x18)  /*  31:1bit reserved;0bit,write 1 to clear 0. */
#define TWI_EFR_REG         (0x1C)  /*  31:2bit reserved,1:0 bit data byte follow read command */
#define TWI_LCR_REG         (0x20)  /*  31:6bits reserved  5:0bit for sda&scl control*/
#define TWI_DVFS_REG        (0x24)  /*  31:3bits reserved  2:0bit for dvfs control. only A10 support. */
#define TWI_DRIVER_CTRL     (0x200)
#define TWI_DRIVER_CFG      (0x204)
#define TWI_DRIVER_SLV      (0x208)
#define TWI_DRIVER_FMT      (0x20C)
#define TWI_DRIVER_BUSC     (0x210)
#define TWI_DRIVER_INTC     (0x214)
#define TWI_DRIVER_DMAC     (0x218)
#define TWI_DRIVER_FIFOC    (0x21C)
#define TWI_DRIVER_SENDF    (0x300)
#define TWI_DRIVER_RECVF    (0x304)

/* TWI address register */
/* general call address enable for slave mode */
#define TWI_GCE_EN      (0x1<<0)
#define TWI_ADDR_MASK       (0x7f<<1)   /* 7:1bits */
/* 31:8bits reserved */


/* TWI extend address register */
/* 7:0bits for extend slave address */
#define TWI_XADDR_MASK      (0xff)
/* 31:8bits reserved */


/* TWI Data register default is 0x0000_0000 */
/* 7:0bits for send or received */
#define TWI_DATA_MASK       (0xff)

/* TWI Control Register Bit Fields & Masks, default value: 0x0000_0000*/
/* 1:0 bits reserved */
/* set 1 to send A_ACK,then low level on SDA */
#define TWI_CTL_ACK     (0x1<<2)
/* INT_FLAG,interrupt status flag: set '1' when interrupt coming */
#define TWI_CTL_INTFLG      (0x1<<3)
#define TWI_CTL_STP     (0x1<<4)    /* M_STP,Automatic clear 0 */
#define TWI_CTL_STA     (0x1<<5)    /* M_STA,atutomatic clear 0 */
/* BUS_EN, master mode should be set 1.*/
#define TWI_CTL_BUSEN       (0x1<<6)
#define TWI_CTL_INTEN       (0x1<<7)    /* INT_EN */
/* 31:8 bit reserved */

/* TWI Clock Register Bit Fields & Masks,default value:0x0000_0000 */
/*
 * Fin is APB CLOCK INPUT;
 * Fsample = F0 = Fin/2^CLK_N;
 *  F1 = F0/(CLK_M+1);
 *
 * Foscl = F1/10 = Fin/(2^CLK_N * (CLK_M+1)*10);
 * Foscl is clock SCL;standard mode:100KHz or fast mode:400KHz
 */
#define TWI_CLK_DUTY        (0x1<<7)    /* 7bit  */
#define TWI_CLK_DIV_M       (0xf<<3)    /* 6:3bit  */
#define TWI_CLK_DIV_N       (0x7<<0)    /* 2:0bit */

/* TWI Soft Reset Register Bit Fields & Masks  */
/* write 1 to clear 0, when complete soft reset clear 0 */
#define TWI_SRST_SRST       (0x1<<0)

/* TWI Enhance Feature Register Bit Fields & Masks  */
/* default -- 0x0 */
/* 00:no,01: 1byte, 10:2 bytes, 11: 3bytes */
#define TWI_EFR_MASK        (0x3<<0)
#define TWI_EFR_WARC_0      (0x0<<0)
#define TWI_EFR_WARC_1      (0x1<<0)
#define TWI_EFR_WARC_2      (0x2<<0)
#define TWI_EFR_WARC_3      (0x3<<0)

/* twi line control register -default value: 0x0000_003a */
/* SDA line state control enable ,1:enable;0:disable */
#define TWI_LCR_SDA_EN      (0x01<<0)
/* SDA line state control bit, 1:high level;0:low level */
#define TWI_LCR_SDA_CTL     (0x01<<1)
/* SCL line state control enable ,1:enable;0:disable */
#define TWI_LCR_SCL_EN      (0x01<<2)
/* SCL line state control bit, 1:high level;0:low level */
#define TWI_LCR_SCL_CTL     (0x01<<3)
/* current state of SDA,readonly bit */
#define TWI_LCR_SDA_STATE_MASK  (0x01<<4)
/* current state of SCL,readonly bit */
#define TWI_LCR_SCL_STATE_MASK  (0x01<<5)
/* 31:6bits reserved */
#define TWI_LCR_IDLE_STATUS (0x3a)
#define TWI_LCR_NORM_STATUS (0x30)      /* normal status */

/* TWI Status Register Bit Fields & Masks  */
#define TWI_STAT_MASK       (0xff)
/* 7:0 bits use only,default is 0xF8 */
#define TWI_STAT_BUS_ERR    (0x00)  /* BUS ERROR */
/* Master mode use only */
#define TWI_STAT_TX_STA     (0x08)  /* START condition transmitted */
/* Repeated START condition transmitted */
#define TWI_STAT_TX_RESTA   (0x10)
/* Address+Write bit transmitted, ACK received */
#define TWI_STAT_TX_AW_ACK  (0x18)
/* Address+Write bit transmitted, ACK not received */
#define TWI_STAT_TX_AW_NAK  (0x20)
/* data byte transmitted in master mode,ack received */
#define TWI_STAT_TXD_ACK    (0x28)
/* data byte transmitted in master mode ,ack not received */
#define TWI_STAT_TXD_NAK    (0x30)
/* arbitration lost in address or data byte */
#define TWI_STAT_ARBLOST    (0x38)
/* Address+Read bit transmitted, ACK received */
#define TWI_STAT_TX_AR_ACK  (0x40)
/* Address+Read bit transmitted, ACK not received */
#define TWI_STAT_TX_AR_NAK  (0x48)
/* data byte received in master mode ,ack transmitted */
#define TWI_STAT_RXD_ACK    (0x50)
/* date byte received in master mode,not ack transmitted */
#define TWI_STAT_RXD_NAK    (0x58)
/* Slave mode use only */
/* Slave address+Write bit received, ACK transmitted */
#define TWI_STAT_RXWS_ACK   (0x60)
#define TWI_STAT_ARBLOST_RXWS_ACK   (0x68)
/* General Call address received, ACK transmitted */
#define TWI_STAT_RXGCAS_ACK     (0x70)
#define TWI_STAT_ARBLOST_RXGCAS_ACK (0x78)
#define TWI_STAT_RXDS_ACK       (0x80)
#define TWI_STAT_RXDS_NAK       (0x88)
#define TWI_STAT_RXDGCAS_ACK        (0x90)
#define TWI_STAT_RXDGCAS_NAK        (0x98)
#define TWI_STAT_RXSTPS_RXRESTAS    (0xA0)
#define TWI_STAT_RXRS_ACK       (0xA8)
#define TWI_STAT_ARBLOST_SLAR_ACK   (0xB0)
/* 10bit Address, second part of address */
/* Second Address byte+Write bit transmitted,ACK received */
#define TWI_STAT_TX_SAW_ACK     (0xD0)
/* Second Address byte+Write bit transmitted,ACK not received */
#define TWI_STAT_TX_SAW_NAK     (0xD8)
/* No relevant status information,INT_FLAG = 0 */
#define TWI_STAT_IDLE           (0xF8)

/* Offset:0x0200. Twi driver control register(Default Value:0x00F8_0000) */
#define TWI_DRV_EN  (0x01<<0)
#define TWI_DRV_RST (0x01<<1)
#define TWI_DRV_STA (0xff<<16)
#define TRAN_RESULT (0x0f<<24)
#define READ_TRAN   (0x01<<28)
#define START_TRAN  (0x01<<31)
#define TRAN_OK     0x00
#define TRAN_FAIL   0x01

/*
 * Offset:0x0204.
 * Twi driver transmission configuration register(Default Value:0x1000_0001)
 */
#define PACKET_MASK (0xffff<<0)
#define INTERVAL_MASK   (0xff<<16)

/* Offset:0x0208. Twi driver slave id register(Default Value:0x0000_0000) */
#define SLV_ID_X    (0xff<<0)
#define SLV_RD_CMD  (0x01<<8)
#define SLV_ID      (0x7f<<9)

/*
 * Offset:0x020C.
 * Twi driver packet format register(Default Value:0x0001_0001)
 */
#define DATA_BYTE   0xffff
#define ADDR_BYTE   (0xff<<16)

/* Offset:0x0210. Twi driver bus control register(Default Value:0x0000_00C0) */
#define TWI_DRV_CLK_DUTY    (0x01<<16)
#define TWI_DRV_CLK_M       (0x0f<<8)
#define TWI_DRV_CLK_N       (0x07<<12)

/*
 * Offset:0x0214.
 * Twi driver interrupt control register(Default Value:0x0000_0000)
 */
#define TRAN_COM_PD (0x1<<0)
#define TRAN_ERR_PD (0x1<<1)
#define TX_REQ_PD   (0x1<<2)
#define RX_REQ_PD   (0x1<<3)
#define TRAN_COM_INT    (0x1<<16)
#define TRAN_ERR_INT    (0x1<<17)
#define TX_REQ_INT  (0x1<<18)
#define RX_REQ_INT  (0x1<<19)
#define TWI_DRV_INT_MASK    (0x0f<<16)
#define TWI_DRV_STAT_MASK   (0x0f<<0)

/*
 * Offset:0x0218.
 * Twi driver DMA configure register(Default Value:0x0010_0010)
 */
#define TRIG_DEFAULT    0x10
#define TRIG_MASK   0x3f
#define DMA_TX      (0x01<<8)
#define DMA_RX      (0x01<<24)
#define TWI_DRQEN_MASK  (DMA_TX | DMA_RX)

/* Offset:0x021C. Twi driver FIFO content register(Default Value:0x0000_0000) */
#define SEND_FIFO_CONT  (0x3f<<0)
#define SEND_FIFO_CLEAR (0x01<<6)
#define RECV_FIFO_CONT  (0x3f<<16)
#define RECV_FIFO_CLEAR (0x01<<22)

/*
 * Offset:0x0300.
 * Twi driver send data FIFO access register(Default Value:0x0000_0000)
 */
#define SEND_DATA_FIFO  (0xff<<0)


/*
 * Offset:0x0304.
 * Twi driver receive data FIFO access register(Default Value:0x0000_0000)
 */
#define RECV_DATA_FIFO  (0xff<<0)

/* TWI driver result */
#define RESULT_COMPLETE 1
#define RESULT_ERR  2


/* TWI mode select */
#define TWI_MASTER_MODE     (1)
#define TWI_SLAVE_MODE      (0) /* seldom use */


#define TWI_SEM_MAX_COUNT 0xFFFFFFFFUL

#define SUNXI_TWI_OK      0
#define SUNXI_TWI_FAIL   -1
#define SUNXI_TWI_RETRY  -2
#define SUNXI_TWI_SFAIL  -3  /* start fail */
#define SUNXI_TWI_TFAIL  -4  /* stop  fail */

#define DMA_THRESHOLD   32
#define MAX_FIFO    32
#define DMA_TIMEOUT 1000

#define TWI_PIN_NUM 2 /*pin num of twi*/
#define TWI0_PIN_MUXSEL 5
#define TWI1_PIN_MUXSEL 5
#define TWI2_PIN_MUXSEL 4
#define TWI3_PIN_MUXSEL 5
#define S_TWI0_PIN_MUXSEL 3
#define TWI_DISABLE_PIN_MUXSEL 7
#define TWI_PULL_STATE 1
#define TWI_DRIVE_STATE 0

#define TWI0_SCK GPIOI(3)
#define TWI0_SDA GPIOI(4)
#define TWI1_SCK GPIOI(1)
#define TWI1_SDA GPIOI(2)
#define TWI2_SCK GPIOH(5)
#define TWI2_SDA GPIOH(6)
#define TWI3_SCK GPIOH(13)
#define TWI3_SDA GPIOH(14)
#define S_TWI0_SCK GPIOL(0)
#define S_TWI0_SDA GPIOL(1)

typedef enum
{
    TWI_XFER_IDLE    = 0x1,
    TWI_XFER_START   = 0x2,
    TWI_XFER_RUNNING = 0x4,
} twi_xfer_status_t;

/** @brief This enum defines the HAL interface return value. */
typedef enum
{
    TWI_STATUS_ERROR = -4,                        /**<  An error occurred and the transaction has failed. */
    //TWI_STATUS_ERROR_TIMEOUT = -4,                /**<  The TWI bus xfer timeout, an error occurred. */
    TWI_STATUS_ERROR_BUSY = -3,                   /**<  The TWI bus is busy, an error occurred. */
    TWI_STATUS_INVALID_PORT_NUMBER = -2,          /**<  A wrong port number is given. */
    TWI_STATUS_INVALID_PARAMETER = -1,            /**<  A wrong parameter is given. */
    TWI_STATUS_OK = 0                             /**<  No error occurred during the function call. */
} twi_status_t;

typedef enum
{
    TWI_MASTER_0,           /**< TWI master 0. */
    TWI_MASTER_1,           /**< TWI master 1. */
    TWI_MASTER_2,           /**< TWI master 0. */
    TWI_MASTER_3,           /**< TWI master 1. */
    S_TWI_MASTER_0,           /**< S_TWI master 0. */
    TWI_MASTER_MAX              /**< max TWI master number, \<invalid\> */
} twi_port_t;

/** @brief This enum defines the TWI transaction speed.  */
typedef enum
{
    TWI_FREQUENCY_100K = 100000,          /**<  100kbps. */
    TWI_FREQUENCY_200K = 200000,          /**<  200kbps. */
    TWI_FREQUENCY_400K = 400000,          /**<  400kbps. */
} twi_frequency_t;

/** @brief This enum defines the TWI transaction speed.  */
typedef enum
{
    ENGINE_XFER = 0,
    TWI_DRV_XFER = 1,
} twi_mode_t;

typedef struct twi_msg
{
    uint16_t addr;          /* slave address */
    uint16_t flags;
#define TWI_M_RD        0x0001  /* read data, from slave to master
                     * TWI_M_RD is guaranteed to be 0x0001!
                     * */
#define TWI_M_TEN       0x0010  /* this is a ten bit chip address */
    uint16_t len;           /* msg length */
    uint8_t *buf;       /* pointer to msg data */
} twi_msg_t;

typedef struct sunxi_twi
{

    uint8_t port;
    uint8_t result;
    uint8_t already_init;
    uint8_t twi_drv_used;
    uint8_t pkt_interval;

    uint16_t slave_addr;
    uint16_t flags;

    uint32_t timeout;
    uint32_t msgs_num;
    uint32_t msgs_idx;
    uint32_t msgs_ptr;
    uint32_t base_addr;
    uint32_t irqnum;

    struct regulator_dev regulator;
    hal_clk_id_t pclk;
    hal_clk_id_t mclk;
    twi_frequency_t freq;

    uint32_t    pinmux;
    uint32_t    pin[TWI_PIN_NUM];
    twi_xfer_status_t   status;
    hal_sem_t   hal_complete_sem;
    hal_sem_t   hal_xfer_sem;
    twi_msg_t   *msgs;
#if 0
    struct sunxi_twi_dma    *dma_tx;
    struct sunxi_twi_dma    *dma_rx;
    struct sunxi_twi_dma    *dma_using;
#endif
} hal_twi_t;

typedef enum
{
    I2C_SLAVE = 0,
    I2C_SLAVE_FORCE = 1,
    I2C_TENBIT = 2,
    I2C_RDWR = 3
} hal_twi_transfer_cmd_t;

typedef struct sunxi_hal_driver_twi
{
    twi_status_t (*initialize)(twi_port_t port);
    twi_status_t (*uninitialize)(twi_port_t port);
    twi_status_t (*send)(twi_port_t port, unsigned long pos,
                         const void *buf, uint32_t size);
    twi_status_t (*receive)(twi_port_t port, unsigned long pos,
                            void *buf, uint32_t size);
    twi_status_t (*control)(twi_port_t port, hal_twi_transfer_cmd_t cmd,
                            void *args);
} const sunxi_hal_driver_twi_t;

//initialize twi port
twi_status_t hal_twi_init(twi_port_t port);
//uninitialize twi port
twi_status_t hal_twi_uninit(twi_port_t port);
//twi write
twi_status_t hal_twi_write(twi_port_t port, unsigned long pos, const void *buf, uint32_t size);
//twi read
twi_status_t hal_twi_read(twi_port_t port, unsigned long pos, void *buf, uint32_t size);
//twi control
twi_status_t hal_twi_control(twi_port_t port, hal_twi_transfer_cmd_t cmd, void *args);

#ifdef __cplusplus
}
#endif

#endif
