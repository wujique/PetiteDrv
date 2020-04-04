/**
 * @file            mcu_rtc.c
 * @brief           stm32片上RTC驱动
 * @author          wujique
 * @date            2018年1月24日 星期三
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年1月24日 星期三
 *   作    者:         wujique
 *   修改内容:   创建文件
*/
#include <stdarg.h>
#include <stdio.h>
#include "mcu.h"
#include "log.h"
#include "mcu_rtc.h"

/**
 *@brief:      mcu_rtc_init
 *@details:    复位时初始化RTC
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_rtc_init(void)
{

	wjq_log(LOG_INFO, " init rtc finish\r\n");	
	return 0;	
}

u8 RTC_Set(u32 sec)
{

	return 0;	    
}	  
/**
 *@brief:      mcu_rtc_set_time
 *@details:    设置时间
 *@param[in]   u8 hours    
               u8 minutes  
               u8 seconds  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_rtc_set_time(u8 hours, u8 minutes, u8 seconds)
{

	return 0;
}		
/**
 *@brief:      mcu_rtc_set_date
 *@details:    设置日期
 *@param[in]   u8 year     
               u8 weekday  
               u8 month    
               u8 date     
 *@param[out]  无
 *@retval:     
 */
s32 mcu_rtc_set_date(u8 year, u8 weekday, u8 month, u8 date)
{

	return 0;
}

/**
 *@brief:      mcu_rtc_get_date
 *@details:    读取日期
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_rtc_get_date(void)
{

	
	return 0;
}
/**
 *@brief:      mcu_rtc_get_time
 *@details:    读取时间
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_rtc_get_time(void)
{

	return 0;
}
/*

	获取UTC时间

*/
time_t mcu_rtc_get_utc_time()
{
	time_t time;

	wjq_log(LOG_DEBUG, "%lu\n",time);

	return time;
}

struct tm* gmtime (const time_t *timep)
{
	return 0;
}
/*

	将UTC时间转化为当地时间

*/
struct tm* localtime (const time_t *timep)
{
	return 0;
}

s32 mcu_rtc_test(void)
{
		return 0;
}

