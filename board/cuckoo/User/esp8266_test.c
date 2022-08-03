

#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"
#include "bus_uart.h"

/*
	PE9 IO2
	PE10 IO0
	PA2 EN
	PE13 RST
	*/

void esp8266_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	GPIO_InitStructure.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);


	GPIO_InitStructure.Pin = GPIO_PIN_2;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	HAL_Delay(10);
	
}

char ESP8266_buf[10];

const static BusUartPra Uart7Pra = {
	.BaudRate = 115200, //波特率			 

	.bufsize = 512,//接收缓冲长度
};

void esp8266_uart_test(void)
{
	int rlen;
	BusUartNode *uartnode;
	uint8_t buf[16];
	uartnode = bus_uart_open("uart7", &Uart7Pra);

	while(1) {
		//uart_printf("s ");
		bus_uart_write(uartnode, "AT\r\n", 4);
		HAL_Delay(1000);
		memset(buf, 0, sizeof(buf));
		rlen = bus_uart_read(uartnode, buf, 10);
		if (rlen != 0) {
			uart_printf("%s", buf);
		}
	}
}



