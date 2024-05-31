#ifndef __BUS_UART_H__
#define __BUS_UART_H__

#include "mem/p_kfifo.h"

extern void *bus_uart_open(char *name, const BusUartPra *Pra);
extern s32 bus_uart_read (void *vnode, u8 *buf, s32 len);
extern int bus_uart_write(void *vnode, u8 *buf, s32 len);

#endif

