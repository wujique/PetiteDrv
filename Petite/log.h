/**
 * @file                wujique_log.h
 * @brief           调试信息头文件
 * @author          wujique
 * @date            2018年4月12日 星期四
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年4月12日 星期四
 *   作    者:         wujique
 *   修改内容:   创建文件
*/
#ifndef _WUJIQUE_LOG_H_
#define _WUJIQUE_LOG_H_

#include "mcu.h"

typedef enum
{
	LOG_DISABLE = 0,
	LOG_ERR,	//错误
	LOG_WAR,	//警告
	LOG_INFO,	//信息，例如设备初始化等信息
	LOG_DEBUG,	//调试，正式程序通常屏蔽
}LOG_L;

extern void uart_printf(s8 *fmt,...);
extern void wjq_log(LOG_L l, s8 *fmt,...);
extern void petite_log(LOG_L l, char *tag, const char *file, const char *fun, int line, s8 *fmt,...);

void dump_hex(const uint8_t *buf, uint32_t size, uint32_t number);

#define DUMP_HEX_16BYTE(buf,size) dump_hex(buf,size,16);

#define Uprintf uart_printf

#endif
