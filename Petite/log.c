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
#include <string.h>

#include "mcu.h"
#include "board_sysconf.h"
#include "bus/bus_uart.h"
#include "log.h"
#include "stimer.h"
#include "SEGGER_RTT.h"

LOG_L LogLevel = LOG_DEBUG;//系统调试信息等级

#define LOG_SEGGER_RTT 1
#define BUFFER_INDEX 0

#define LOG_WRITE_DATA(data, len)  mcu_uart_write(PC_PORT, (u8 *)data, len);
s32 log_redef_putc(u8 *buf, s32 len)
{
    mcu_uart_write(PC_PORT, buf, len);   
}
//#define LOG_WRITE_DATA(data, len)  SEGGER_RTT_Write(BUFFER_INDEX,  data, len);

//osMutexId_t LogMutex = NULL;

volatile static int LogInit = -1;


/*
使用串口输出调试信息
*/
s8 string[256];//调试信息缓冲，输出调试信息一次不可以大于256

//extern int vsprintf(char * s, const char * format, __va_list arg);
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
   
    va_list ap;

    if (LogInit == -1) return;

    va_start(ap,fmt);
    vsnprintf((char *)string, sizeof(string) -1, (const char *)fmt, ap);

    s32 length = 0;
    string[sizeof(string) -1] = 0;
    length = strlen((const char *)string);

    LOG_WRITE_DATA(string, length); 

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

    va_list ap;

    va_start(ap,fmt);
    memcpy(string, (u8*)log_color_tab[l], 10);
    vsnprintf((char *)&string[10], sizeof(string) -1, (const char *)fmt, ap);
 
    s32 length = 0;
    string[sizeof(string) -1] = 0;
    length = strlen((const char *)string);

    LOG_WRITE_DATA(string, length); 

    va_end(ap);
}

static char logbuf[128];

void petite_log(LOG_L l, char *tag, const char *file, const char *fun, int line, s8 *fmt,...)
{
    uint32_t systime;
    uint8_t hlen;

	if(l > LogLevel) return;
    if (LogInit == -1) return;

    memset(logbuf, 0, sizeof(logbuf));
    strcpy(logbuf, log_color_tab[l]);

    systime = Stime_get_localtime();
    sprintf(logbuf+strlen((const char *)logbuf), "[%010d]", systime);
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

    sprintf(logbuf+strlen((const char *)logbuf), "%d]: ", line);
    strcat(logbuf, log_color_tab[5]);

    strcpy((char *)string, logbuf);
    hlen = strlen((const char *)logbuf);

    va_list ap;
    va_start(ap,fmt);
    vsnprintf((char *)&string[hlen], sizeof(string)-hlen, (const char *)fmt, ap);

    s32 length = 0;
    string[sizeof(string) -1] = 0;
    length = strlen((const char *)string);
    
    LOG_WRITE_DATA(string, length); 
    va_end(ap);
}

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')

/**------------------------------------------------------------------------------------------*/
char dumphexbuf[80];
#define DUMP_HEX_PERLINE 16
/**
 * dump_hex
 * 
 * @brief dump data in hex format
 * 
 * @param buf: User buffer
 * @param size: Dump data size
 * 
 * @return void
*/
void dump_hex(const uint8_t *buf, uint32_t size)
{
    int i, j;
    int shift;
    if (LogInit == -1) return;

    ///@todo 一个字符调一次printf太慢了，有空要优化，组织一行字符在printf
    for (i = 0; i < size; i += DUMP_HEX_PERLINE) {
        shift = 0;

        sprintf(dumphexbuf, "%08X: ", i);//10 byte
        shift = 10;
        for (j = 0; j < DUMP_HEX_PERLINE; j++) {
            if (j % 8 == 0) {
                sprintf(dumphexbuf + shift, " ");//1byte
                shift += 1;
            }
            if (i + j < size)
                sprintf(dumphexbuf + shift, "%02X ", buf[i + j]);
            else
                sprintf(dumphexbuf + shift, "   ");

            shift += 3;
        } //16*3 byte
        uart_printf(" ");//1byte

        for (j = 0; j < DUMP_HEX_PERLINE; j++) {
            if (i + j < size) {
                char ch = buf[i + j];
                sprintf(dumphexbuf + shift, "%c", ( ch >= 0x20 && ch < 0x7f) ? ch : '.');
                shift++;
            }
        }//16byte
        sprintf(dumphexbuf + shift, "\r\n");//2 byte
        shift+=2;
        dumphexbuf[shift] = 0;
        uart_printf("%s", dumphexbuf);
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
    
    LOG_WRITE_DATA( (u8*)&string[0], length);  //写串口

    va_end(ap);
}

#if 0
const static BusUartPra PcPortPra={
	.BaudRate = 115200,
board_mcu_preinit	.bufsize = 256,
	};
	
void *LogUartNode;
#endif

void log_init(void)
{
    //LogMutex = osMutexNew(NULL);
    SEGGER_RTT_Init();
    LogInit = 0;
}

