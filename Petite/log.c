/**
 * @file                wujique_log.c
 * @brief           调试信息管理
 * @author          wujique
 * @date            2018年4月12日 星期四
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年4月12日 星期四
 *   作    者:         wujique
 *   修改内容:   创建文件
 版权说明：
 					1 源码归屋脊雀工作室所有。
 					2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
 					3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
 					4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
 					5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
 					6 使用本源码则相当于认同本版权说明。
 					7 如侵犯你的权利，请联系：code@wujique.com
					8 一切解释权归屋脊雀工作室所有。
*/
#include <stdarg.h>
#include <stdio.h>

#include "mcu.h"
#include "board_sysconf.h"
#include "bus/bus_uart.h"
#include "log.h"

LOG_L LogLevel = LOG_DEBUG;//系统调试信息等级

//osMutexId_t LogMutex = NULL;

volatile static int LogInit = -1;


/*
使用串口输出调试信息
*/
s8 string[256];//调试信息缓冲，输出调试信息一次不可以大于256

#if 1
#ifdef __CC_ARM//优先配置MDK，因为MDK也可能使用GNU扩展
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#warning "__CC_ARM redef printf putchar in fputc"
#elif __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#warning "__GNUC__ redef printf putchar in __io_putchar"
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#warning "default redef printf putchar in fputc"
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
	#if 0
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART3, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	#else
    if (LogInit == -1) return ch;
	/* 用hal库实现putc，效率很低*/
	char tmp[2];
	tmp[0] = ch;
	mcu_uart_write(PC_PORT, (u8*)&tmp[0], 1);
	#endif
	
    return ch;
}

#endif 


extern int vsprintf(char * s, const char * format, __va_list arg);
/**
 *@brief:      uart_printf
 *@details:    从串口格式化输出调试信息
 *@param[in]   s8 *fmt  
               ...      
 *@param[out]  无
 *@retval:     
 */
void uart_printf(s8 *fmt,...)
{
    s32 length = 0;
    va_list ap;

    s8 *pt;
    
    if (LogInit == -1) return;

    va_start(ap,fmt);
    vsprintf((char *)string,(const char *)fmt,ap);
    pt = &string[0];
    while(*pt!='\0') {
        length++;
        pt++;
    }
    
    mcu_uart_write(PC_PORT, (u8*)&string[0], length);  //写串口
    
    va_end(ap);
}
/*
	字体颜色+背景色+方式
	*/
const char *log_color_tab[]={
	"",
	"\033[31;40;1m",//red
	"\033[33;40;1m",//orange
	"\033[32;40;1m",//green
	"\033[34;40;1m",//blue
    "\033[39;40;1m",//default,white

};

void wjq_log(LOG_L l, s8 *fmt,...)
{
	if(l > LogLevel) return;
    if (LogInit == -1) return;

	s32 length = 0;
    va_list ap;

    s8 *pt;

	
    va_start(ap,fmt);
    vsprintf((char *)&string[0],(const char *)fmt,ap);
	
    pt = &string[0];
    while(*pt!='\0') {
        length++;
        pt++;
    }

	mcu_uart_write(PC_PORT, (u8*)log_color_tab[l], 10);
    mcu_uart_write(PC_PORT, (u8*)&string[0], length);  //写串口
    
    va_end(ap);
}

static char logbuf[128];

void petite_log(LOG_L l, char *tag, const char *file, const char *fun, int line, s8 *fmt,...)
{
    uint32_t systime;

	if(l > LogLevel) return;
    if (LogInit == -1) return;

	mcu_uart_write(PC_PORT, (u8*)log_color_tab[l], 10);


    systime = Stime_get_localtime();

    memset(logbuf, 0, sizeof(logbuf));

    sprintf(logbuf, "[%010d]", systime);
    if (tag != NULL) {
        strcat(logbuf, "[");
        strcat(logbuf, tag);
        strcat(logbuf, "]");
    }

    if (file != NULL) {
        strcat(logbuf, "[");
        strcat(logbuf, file);
        strcat(logbuf, "]");
    }

    strcat(logbuf, "[");

    if (fun != NULL) {
        
        strcat(logbuf, fun);
        strcat(logbuf, ":");
    }

    sprintf(logbuf+strlen(logbuf), "%d]: ", line);
    mcu_uart_write(PC_PORT, (u8*)logbuf, strlen(logbuf));

    mcu_uart_write(PC_PORT, (u8*)log_color_tab[5], 10);

    s32 length = 0;
    va_list ap;
    s8 *pt;
    va_start(ap,fmt);
    vsprintf((char *)&string[0],(const char *)fmt,ap);
    pt = &string[0];
    while(*pt!='\0') {
        length++;
        pt++;
    }
    mcu_uart_write(PC_PORT, (u8*)&string[0], length);  //写串口
    va_end(ap);
}

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')

/**
 * dump_hex
 * 
 * @brief dump data in hex format
 * 
 * @param buf: User buffer
 * @param size: Dump data size
 * @param number: The number of outputs per line
 * 
 * @return void
*/
void dump_hex(const uint8_t *buf, uint32_t size, uint32_t number)
{
    int i, j;

    if (LogInit == -1) return;

    ///@todo 一个字符调一次printf太慢了，有空要优化，组织一行字符在printf
    for (i = 0; i < size; i += number) {
        uart_printf("%08X: ", i);

        for (j = 0; j < number; j++) {
            if (j % 8 == 0) {
                uart_printf(" ");
            }
            if (i + j < size)
                uart_printf("%02X ", buf[i + j]);
            else
                uart_printf("   ");
        }
        uart_printf(" ");

        for (j = 0; j < number; j++)
        {
            if (i + j < size)
            {
                uart_printf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
            }
        }
        uart_printf("\r\n");
    }
}

void cmd_uart_printf(s8 *fmt,...)
{
    s32 length = 0;
    va_list ap;

    s8 *pt;

    if (LogInit == -1) return;

    va_start(ap,fmt);
    vsprintf((char *)string,(const char *)fmt,ap);
    pt = &string[0];
    while(*pt!='\0')
    {
        length++;
        pt++;
    }
    
    mcu_uart_write(PC_PORT, (u8*)&string[0], length);  //写串口
    
    va_end(ap);
}

#if 0
const static BusUartPra PcPortPra={
	.BaudRate = 115200,
	.bufsize = 256,
	};
	
BusUartNode *LogUartNode;
#endif

void log_init(void)
{
    //LogMutex = osMutexNew(NULL);
    LogInit = 0;
}

