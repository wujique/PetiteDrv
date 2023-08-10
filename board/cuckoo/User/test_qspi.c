#include <stdarg.h>
#include <stdio.h>

#include "log.h"

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

#include "gb18030.h"
#include "gb2312.h"

/*
	测试在QSPI上执行程序时是否能写QSPI Flash
	实测，好像可以。
	test_iap 函数是放在 QSPI Flash
	但是uart_printf、w25qxx_test_wr、是放在内部的
	test_qpi_run_fun是放在外部的，
	本测试是：
	从内部Flash，调用外部Flash的test_iap函数，
	函数中调用内部uart_printf等函数，然后执行
	内部函数w25qxx_test_wr进行外部Flash读写操作，
	完成后回到外部Flash，执行外部Flash中的函数test_qpi_run_fun。


	本测试未通过
	*/
extern void OSPI_reinit(void);

void test_iap(void)
{
	uint32_t tickstart, tmp;
	u8 a;
	
	uart_printf("run test_iap\r\n");
	
	tickstart = HAL_GetTick();
	//OSPI_reinit();
	w25qxx_test_wr();
	//w25qxx_map();
	tmp = HAL_GetTick();
	
	a = test_qpi_run_fun(2);
	uart_printf("run test_iap finish :%d ms, add:%d\r\n", tmp-tickstart, a);
	
}

