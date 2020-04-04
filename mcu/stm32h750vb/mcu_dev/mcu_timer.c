/**
 * @file            mcu_timer.c
 * @brief           CPU片上定时器驱动
 * @author          test
 * @date            2017年10月25日 星期三
 * @version         初稿
 * @par             
 * @par History:
 * 1.日    期:      2017年10月25日 星期三
 *   作    者:      屋脊雀工作室
 *   修改内容:      创建文件
 		1 源码归屋脊雀工作室所有。
		2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/

#include "mcu.h"
#include "log.h"
#include "mcu_timer.h"

/*

	对定时器进行抽象，抽象的接口提供给上层驱动使用。
	1. 定时器分软定时器和硬定时器。
	2. 硬定时器分3种功能：PWN，捕获，定时。
	3. 定时器提供接口

*/

#define MCU_TIME_DEBUG

#ifdef MCU_TIME_DEBUG
#define TIME_DEBUG	wjq_log 
#else
#define TIME_DEBUG(a, ...)
#endif

typedef struct _strTimerCtrl
{
	void (*Callback)(void);	
	u8 retype;
}McuTimerCtrl;

McuTimerCtrl McuTimerCtrlList[MCU_TIMER_MAX];
/*STM32相关操作定义*/
const TIM_TypeDef *STM32TimerList[MCU_TIMER_MAX]={
	0,
		TIM1,TIM2,TIM3,TIM4,
		TIM5,TIM6,TIM7,TIM8

	};
const uint32_t STM32TimerRccList[MCU_TIMER_MAX]={
	0,
		1,2,3,4,
		5,6,7,8
	};
/*
	从这个表格可以看出，23457这5个定时器中断入口没有复用，
	简单的定时功能最好用这4个
*/	
const uint8_t STM32TimerIRQList[MCU_TIMER_MAX]={
	0,
		0, 1, 2, 3,
		4, 5, 6, 7
	};
/*
    定时器时钟为84M,
    Tout=((SYSTEM_CLK_PERIOD)*(SYSTEM_CLK_PRESCALER))/Ft us.

	预分频,8400个时钟才触发一次定时器计数 
	那么一个定时器计数的时间就是(1/84M)*8400 = 100us	  
*/	
const u16  STM32TimerTickSet[MCU_TIMER_DEF_MAX] =
	{
		0,
		84,//1us
		840,//840预分频，一个tick=10us
		8400,//8400预分频，一个tick=100us
		42000,//预分频，一个tick=500us
		8,//约等于100ns，通常用于捕获计数而不是定时
	}; 
/**
 *@brief:	   mcu_tim3_IRQhandler
 *@details:    定时器中断处理函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:	   
 */
void mcu_tim3_IRQhandler(void)
{

}

/**
 *@brief:	   mcu_tim5_IRQhandler
 *@details:    定时器中断处理函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:	   
 */
void mcu_tim5_IRQhandler(void)
{
}

/**
 *@brief:	   mcu_tim7_IRQhandler
 *@details:    定时器中断处理函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:	   
 */
void mcu_tim7_IRQhandler(void)
{

}

/**
 *@brief:	   
 *@details:    
 *@param[in]	 
 *@param[out]  
 *@retval:	   
 */
s32 mcu_timer_init(McuTimerNum timer)
{

	return 0;
}  
/**
 *@brief:      
 *@details:    
 *@param[in]     
 *@param[out]  
 *@retval:     
 */

s32 mcu_timer_config(McuTimerNum timer, McuTimerTickDef tickdef, u32 tick, void (*callback)(void), u8 type)
{

	
	return 0;
}

s32 mcu_timer_start(McuTimerNum timer)
{
	
	return 0;
}

/**
 *@brief:      
 *@details:    
 *@param[in]     
 *@param[out]  
 *@retval:     
 */
s32 mcu_timer_stop(McuTimerNum timer)
{

	return 0;
}

/**
 *@brief:      mcu_timer_pwm_init
 *@details:    
 *@param[in]   
 *@param[out]  无
 *@retval:     
 */
s32 mcu_timer_pwm_init(McuTimerNum timer, McuTimerTickDef tickdef, u32 tick, u8 duty, McuTimerCh ch)
{



	return 0;
}
/*
	定时器捕获
*/
/**
 *@brief:      mcu_timer_cap_init
 *@details:    初始化定时器捕获，不使用中断
 *@param[in]     
               
 *@param[out]  无
 *@retval:     
 */
s32 mcu_timer_cap_init(McuTimerNum timer, McuTimerTickDef tickdef, McuTimerCh ch, McuTimerCapEdge edge)
{


	return 0;

}
/**
 *@brief:      mcu_timer_get_cap
 *@details:    查询获取定时去捕获值
 *@param[in]   void  
 *@param[out]  无
 *@retval:     捕获值，超时则返回最大值	
 */
u32 mcu_timer_get_cap(McuTimerNum timer, McuTimerCh ch)
{ 


	return 0;
}

/*


*/
/**
 *@brief:	   mcu_tim5_test
 *@details:    定时器测试
 *@param[in]   void  
 *@param[out]  无
 *@retval:	   
 */
void mcu_tim5_test(void)
{
	static u8 i = 0;

	i++;
	if(1 == i)
	{
		TIME_DEBUG(LOG_DEBUG, "tim int 1\r\n");    
	}
	else if(2 == i)
	{
		TIME_DEBUG(LOG_DEBUG, "tim int 2\r\n");
	}
	else if(3 == i)
	{
		TIME_DEBUG(LOG_DEBUG, "tim int 3\r\n");
	}
	else
	{
		TIME_DEBUG(LOG_DEBUG, "tim int 4\r\n");
		i = 0;	 
	}
}
void mcu_tim7_test(void)
{

	TIME_DEBUG(LOG_DEBUG, "tim7\r\n");    

}

s32 mcu_timer_test(void)
{
	mcu_timer_init(MCU_TIMER_5);
	mcu_timer_config(MCU_TIMER_5, MCU_TIMER_500US, 2000, mcu_tim5_test, MCU_TIMER_RE);

	//mcu_timer_init(MCU_TIMER_7);
	//mcu_timer_config(MCU_TIMER_7, MCU_TIMER_10US, 50000, mcu_tim7_test, MCU_TIMER_RE);
	
	mcu_timer_start(MCU_TIMER_5);
	//mcu_timer_start(MCU_TIMER_7);
	
	while(1)
	{
		
	}
}


