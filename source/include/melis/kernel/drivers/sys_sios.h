#ifndef _SYS_SIOS_H_
#define _SYS_SIOS_H_
#include <typedef.h>

#define PHY_ADDR_BASE_PIOC          0x01c20800
#define PHY_ADDR_BASE_CCM           0X01C20000

__s32 SIOS_Init(void);
__s32 SIOS_Exit(void);
__s32 SIOS_ClockChange(void);
__s32 SIOS_ClockDone(void);
__s32 SIOS_RegHw(void);
__s32 SIOS_UnregHw(void);
__s32 UART_DbgInit(unsigned pioMemBase, unsigned ccmMemBase, void *gpio_set, __s32 uart_port);

__s32 UART_DbgExit(void);

/*********************************************************************
* Method     :          UART_DbgLock
* Description: Lock uart for source clock changed, before adjust the source clock,
            this method should be called to lock the uart port.
* Parameters :
    @void
* Returns    :   __s32
* Note       :
*********************************************************************/
__s32 UART_DbgLock(void);

__s32 UART_DbgSetBaudRate(unsigned dbgBaudRate);

/*********************************************************************
* Method     :          UART_DbgUnlock
* Description: change baudrate to @dbgBaudRate and unlock the uart port.
* Parameters :
    @unsigned dbgBaudRate : baud rate for debug.
* Returns    :   __s32
* Note       :
*********************************************************************/
__s32 UART_DbgUnlock(void);

/*********************************************************************
* Method     :          LIB_DBG_UART_put_data
* Description: request the uart to transmit data
* Parameters :
    @const char * data
    @__u32 dataLen
* Returns    :   data size successful transmitted
* Note       :
*********************************************************************/
__s32 UART_DBGPutStr(const char *data);

/*********************************************************************
* Method     :          LIB_DBG_UART_get_data
* Description: receive data from the uart and copied to capacity
* Parameters :
    @char * container :
    @__u32 capacity
* Returns    :   how many data in byte copied successful to the @container,
                  always not larger than capacity
* Note       :
*********************************************************************/
__s32 UART_DBGGetStr(char *container, __u32 capacity);


/*
*********************************************************************************************************
*                                   UART DEBUG PUT ARGUMENT
*
* Description: uart debug put argument.
*
* Arguments  : arg      argument for output.
*              format   format for output.
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void UART_DbgPutArg(unsigned arg, char format);


/*
*********************************************************************************************************
*                                   UART DEBUG GET CHAR
*
* Description: uart debug get char;
*
* Arguments  : none;
*
* Returns    : charactor got from uart;
*
* Note       :
*********************************************************************************************************
*/
char UART_DbgGetChar(void);
char UART_DbgPutChar(char data);

extern __s32 esSIOS_putchar(char data);
// SIOS
extern __u8 esSIOS_getchar(void);
extern void esSIOS_gets(char *str);
extern void esSIOS_putarg(__u32 arg, char format);
extern void esSIOS_putstr(const char *str);
extern void esSIOS_setbaud(__u32 baud);
#endif  /* _SYS_SIOS_H_ */
