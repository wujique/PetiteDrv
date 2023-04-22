/*
	修改本文件实现自己的命令行功能

	本文件功能：
		1. 定义静态命令列表。
		2. 创建命令行任务线程（TASK）。
		3. 实现串口收发接口。

	
	
*/
#include "board_sysconf.h"
#include "FreeRtos.h"


#include <command.h>
#include "console.h"

extern int do_system_info( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int cmd_spiffs_ls( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_version (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_help (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
extern int cmd_spiffs_format( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int cmd_display( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

/*
	命令列表 
	使用静态列表，不再使用注册方式。
	注册方式依赖于编译器平台，较麻烦。
	*/
const cmd_tbl_t static_cmd_table[CONFIG_CMD_TABLE_SIZE]={
		{"sysinfo", 1, 1, do_system_info, "systeminfo", "\t display system info "},

		{"spiffs_ls", 2 , 1, cmd_spiffs_ls,	"spiffs ls",	"\t ls spiffs file "},
		{"spiffs_format", 2 , 1, cmd_spiffs_format,	"spiffs format",	"\t ls spiffs file "},

		{"display", 4 , 1, cmd_display,	"lcd display",	"\t lcd display test "},
		
		{"version",	1,		1,	do_version,	"print monitor version",	""},
		{"help",	1,	1,	do_help,	"print online help",
				"[command ...]\r\n"
				"    - show help information (for 'command')\r\n"
				"'help' prints online help for the monitor commands.\r\n\r\n"
				"Without arguments, it prints a short usage message for all commands.\r\n\r\n"
				"To get detailed help information for specific commands you can type\r\n"
				"'help' with one or more command names as arguments."},
		{"\0", NULL},//don't remove
};

extern void cli_main_loop (void* p);

TaskHandle_t  FunCmdTaskHandle;
/**
 *@brief:      fun_cmd_init
 *@details:    初始化命令行，建立命令行任务
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 fun_cmd_init(void)
{
	xTaskCreate(	(TaskFunction_t) cli_main_loop,
					(const char *)"cmd task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
					(const configSTACK_DEPTH_TYPE) UBOOTCMD_TASK_STK_SIZE,
					(void *) NULL,
					(UBaseType_t) UBOOTCMD_TASK_PRIO,
					(TaskHandle_t *) &FunCmdTaskHandle );	
}

/*-----------------------串口交互接口--------------------------*/
#include "bus/bus_uart.h"

extern BusUartNode *LogUartNode;

int serial_getc (void)
{
	u8 ch,ret;

	while(1) {
		ret = bus_uart_read(LogUartNode, &ch, 1);
		if(0 == ret) {
			Delay(1);
		}else return (int)ch;
	}
	//return getchar();
}

/*
 * Output a single byte to the serial port.
 */
void serial_putc (const char c)
{
	bus_uart_write(LogUartNode, (u8 *)&c, 1);
	/* If \n, also do \r */
	if (c == '\n')
		serial_putc ('\r');
}
/*
	检测串口是否有数据
*/
int serial_tstc(void)
{
	return 1;//public_soc_uart_get_present(TERM_PORT);
}


