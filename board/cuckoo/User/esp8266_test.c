

#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"


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

/*
HAL_UART_Transmit_IT()
HAL_UART_Receive_IT
HAL_UART_TxCpltCallback
HAL_UART_RxCpltCallback
*/
extern UART_HandleTypeDef McuHuart2;
extern UART_HandleTypeDef huart7;

char ESP8266_buf[10];
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	//HAL_UART_Transmit(&McuHuart2, ESP8266_buf, 1, 100);	
	uart_printf("%c", ESP8266_buf[0]);
	HAL_UART_Receive_IT(&huart7, ESP8266_buf, 1);
}

void esp8266_uart_test(void)
{
	HAL_UART_Receive_IT(&huart7, ESP8266_buf, 1);
	
	while(1) {
		//uart_printf("s ");
		HAL_UART_Transmit(&huart7, "AT\r\n", 4, 1000);
		HAL_Delay(1000);
	}
}



