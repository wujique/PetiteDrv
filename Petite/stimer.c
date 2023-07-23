/**
 * @file            
 * @brief           
 * @author          wujique
 * @date            2018年1月6日 星期六
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年1月6日 星期六
 *   作    者:         wujique
 *   修改内容:   创建文件
        1 源码归屋脊雀工作室所有。
		2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/
/*

	本函数功能是，定时相关功能管理

*/
#include "mcu.h"
#include "board_sysconf.h"
#include "log.h"

#include "stimer.h"


volatile uint32_t LocalTime = 0; 
/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  本函数放在系统滴答中执行，通常滴答间隔1ms
  在board_mcu_preinit中，如果没有用RTOS，会初始化一个裸奔用的滴答定时器。
  */
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}


uint32_t timingdelay;

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;  

  /* wait until the desired delay finish */  
  while(timingdelay > LocalTime)
  {     
  }
}




uint32_t Stime_get_localtime(void)
{
	return LocalTime;
}

uint32_t stime_get_passtime(uint32_t StrTime)
{
	uint32_t cur, pass;

	/* 当前时间*/
	cur = LocalTime;

	if(cur >= StrTime)
		pass = cur - StrTime;
	else
		pass = 0xffffffff - (StrTime - cur);	

	return pass;
}


#include "time.h"
///@bug need fix
time_t time(time_t *t)
{

	return LocalTime;
}

