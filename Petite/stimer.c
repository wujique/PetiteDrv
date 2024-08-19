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

#include "time.h"
#define CLOCKS_PER_SEC (1000)
volatile uint32_t SysTickCnt = 0;//系统上电后运行时间计数，如果是32位，1ms周期下最大计时49.7天
time_t time_dat;//设置localtime相对于公元1970年1月1日0时0分0秒算起至今的UTC时间所经过的秒数

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  本函数放在系统滴答中执行，通常滴答间隔1ms
  在board_mcu_preinit中，如果没有用RTOS，会初始化一个裸奔用的滴答定时器。
  */
void Time_Update(void)
{
    SysTickCnt+= SYSTEMTICK_PERIOD_MS;
    if ((SysTickCnt % CLOCKS_PER_SEC) == 0) {
        time_dat++;
    }

}

/* Private functions ---------------------------------------------------------*/

int sys_timer_test(void)
{
	printf("run sys timer test!\r\n");
  //设置时间，localtime
  printf("set localtime\r\n");
  {
      struct tm set_time;
      set_time.tm_sec = 10;
      set_time.tm_min = 0;
      set_time.tm_hour = 22;
      set_time.tm_mday = 19;
      set_time.tm_mon = 8-1;
      set_time.tm_year = 2024-1900;
      //set_time.tm_wday = 1;
      //set_time.tm_yday = 2;
      set_time.tm_isdst = -1;
      //C 库函数 time_t mktime(struct tm *timeptr) 把 timeptr 所指向的结构转换为一个依据本地时区的 time_t 值，根据当前时区设置。
      time_dat = mktime(&set_time);//mktime()用来将参数timeptr所指的tm结构数据转换成从公元1970年1月1日0时0分0 秒算起至今的UTC时间所经过的秒数。
  }
  
  //显示时间
	printf("show localtime\r\n");
  {
      int32_t now;
      struct tm *ts;
	  /* 得到一个秒时间 */
      now = time(NULL);
	  printf("now:%d\r\n", now);

	  /* 将秒转未tm格式的时间戳*/
      ts = localtime(&now);
      /* 将本地时间戳转为字符串 */
      //printf("localtime = %s\r\n", asctime(ts));
	  /** gmtime获取到的是GMT时间 */
      //printf("gmtime    = %s\r\n", asctime(gmtime(&now)));
      
      printf("ts->tm_yesr   = %d\r\n", 1990 + ts->tm_year);
      printf("ts->tm_mon    = %d\r\n", 1 + ts->tm_mon);
      printf("ts->tm_mday   = %d\r\n", 0 + ts->tm_mday);
      printf("ts->tm_hour   = %d\r\n", 0 + ts->tm_hour);
      printf("ts->tm_min    = %d\r\n", 0 + ts->tm_min);
      printf("ts->tm_sec    = %d\r\n", 0 + ts->tm_sec);
      printf("ts->tm_wday   = %d\r\n", 0 + ts->tm_wday);
      printf("ts->tm_yday   = %d\r\n", 1 + ts->tm_yday);
      
      printf("ctime = %s\r\n", ctime(&now));
      
      char buf[80] = {0};
      strftime (buf, sizeof (buf), "%a %Y-%m-%d %H:%M:%S%Z", ts);
      printf ("%s \n", buf);
  }
  
  {
      clock_t clock_start, clock_end;
      clock_start = clock();
      osDelay(5000);
      clock_end = clock();
      double total_t = (double)(clock_end - clock_start) / CLOCKS_PER_SEC;
      printf("sleep(5) use times %f seconds\r\n", total_t);
  }

  {
      time_t time_start, time_end;
      time(&time_start);
      osDelay(5000);
      time(&time_end);
      double diff_t = difftime(time_end, time_start);
      printf("sleep(5) use times %f seconds\r\n", diff_t);
  }
  
  /* Display time in infinite loop */
  while (1) {
      time_t now;
      now = time(NULL);
      printf("localtime = %s\r\n", asctime(localtime(&now)));
      printf("gmtime    = %s\r\n", asctime(gmtime(&now)));
      osDelay(5000); 
  }
}





/*---------------------------------------------------*/
/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
	uint32_t timingdelay = 0;
  	/* Capture the current local time */
  	timingdelay = SysTickCnt + nCount;  

  	/* wait until the desired delay finish */  
  	while(timingdelay > SysTickCnt){

  }
}


uint32_t Stime_get_localtime(void)
{
	return SysTickCnt;
}

uint32_t Stime_get_systime(void)
{
	return SysTickCnt;
}

uint32_t stime_get_passtime(uint32_t StrTime)
{
	uint32_t cur, pass;

	/* 当前时间*/
	cur = SysTickCnt;

	if(cur >= StrTime)
		pass = cur - StrTime;
	else
		pass = 0xffffffff - (StrTime - cur);	

	return pass;
}

/*---------------------------------------------------*/




