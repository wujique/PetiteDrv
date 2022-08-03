#ifndef __BUS_UART_H__
#define __BUS_UART_H__

#include "mem/p_kfifo.h"


typedef enum {
  MCU_UART_1 =0,
  MCU_UART_2,
  MCU_UART_3,
  MCU_UART_4,
  MCU_UART_5,
  MCU_UART_6,
  MCU_UART_7,
  MCU_UART_8,
  MCU_UART_MAX,
}
McuUartNum; 


/* */
typedef struct {
	McuUartNum comport;
	
	/* mcu 层会直接操作下面变量 */
	char *buf;
	struct _pkfifo Kfifo;//接收缓冲
	u8  OverFg;//溢出标志	
}BusUartNode;


typedef struct
{
  uint32_t BaudRate; //波特率           
  uint16_t WordLength;  //        
  uint16_t StopBits;            
  uint16_t Parity;              
  uint16_t Mode;               
  uint16_t HardwareFlowControl; 
  
  uint32_t bufsize;//接收缓冲长度
} BusUartPra;


extern BusUartNode *bus_uart_open(char *name, const BusUartPra *Pra);


#endif

