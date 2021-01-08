/**
 * @file            cmd_sys.c
 * @brief           系统命令定义
 * @author          wujique
 * @date            2018年1月8日 星期一
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年1月8日 星期一
 *   作    者:         wujique
 *   修改内容:   创建文件
*/
#include <command.h>
#include "console.h"

#define DAY_SECOND		(24*60*60)
#define HOUR_SECOND		(60*60)
#define MINI_SECOND		(60)

#define SYSTEM_TICK_HZ 100


int do_system_info( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint32 ticks = Stime_get_localtime();
	uint32 temp;

	unsigned int day=0,hour=0,min = 0,tmp=0;
	cmd_printf("COMPILE DATE: %s - %s \r\n",__DATE__,__TIME__);
	cmd_printf("TICKS FREQ: %d \r\n",SYSTEM_TICK_HZ);
	cmd_printf("SYSTEM TCKS: %d  \r\n",ticks);
	ticks = ticks/1000;
	cmd_printf("SYSTEM UPTIME: %d seconds \r\n",ticks);

	day = (ticks/DAY_SECOND);
	ticks =(ticks%DAY_SECOND);

	tmp = (unsigned int)ticks;
	hour = tmp/HOUR_SECOND;
	tmp = tmp%HOUR_SECOND;

	min = tmp/MINI_SECOND;
	tmp = tmp%MINI_SECOND;

	cmd_printf("SYSTEM UPTIME:%d:DAYS %dH:%dM:%dS\r\n",(unsigned int)day,
			hour,min, tmp);

	cmd_printf("IMAGE VERSION: %s \r\n", "0.1.1");

	temp = mcu_tempreate_get_tempreate();
	cmd_printf("CPU TEMPREATE: %d.%d \r\n", temp/100,temp%100);
	
	return 0;
}



