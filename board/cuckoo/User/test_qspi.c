#include <stdarg.h>
#include <stdio.h>

#include "log.h"

extern OSPI_HandleTypeDef hospi1;

const u8 test_qspi_tab[33] = {
	0x99, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

	0x98,
};

/* 本函数放到 外部SPI FLASH 进行测试 */
u8 test_qpi_run_fun(u8 num)
{
	num++;
	return num;
}
/* 把图片放到QSPI FALSH，map之后直接读数据 */
//#include "RGB565_480x272.h"
/*
	测试在QSPI上执行程序时是否能写QSPI Flash
	实测，好像可以。
	*/
void test_iap(void)
{
	uint32_t tickstart, tmp;
	
	uart_printf("run test_iap\r\n");
	
	tickstart = HAL_GetTick();
	w25qxx_test_2();
	tmp = HAL_GetTick();
	
	test_qpi_run_fun(2);
	uart_printf("run test_iap finish :%d\r\n", tmp-tickstart);
	
}

