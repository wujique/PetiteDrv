#ifndef __DEV_UART_H__
#define __DEV_UART_H__

#include "bus/bus_uart.h"

extern s32 mcu_uart_init(McuUartNum comport, BusUartNode *BusNode, const BusUartPra *Pra);
extern s32 mcu_uart_deinit (McuUartNum comport);
extern s32 mcu_uart_send (McuUartNum comport, u8 *buf, s32 len);
extern s32 mcu_uart_set_baud (McuUartNum comport, s32 baud);

#define mcu_uart_send mcu_uart_write


#endif

