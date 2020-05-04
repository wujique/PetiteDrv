
#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"


/*
	板级初始化
*/
s32 board_init(void)
{
	wjq_log(LOG_DEBUG, "[   board] Albatross other dev init!\r\n");

	petite_dev_register();

	albatross_750test_init();
	
	return 0;
}

/*
	低优先级轮训任务
*/
void board_low_task(void)
{
	wjq_log(LOG_DEBUG, "[   board] low task!\r\n");
	
}

