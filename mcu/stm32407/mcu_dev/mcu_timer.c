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

#include "stm32f4xx.h"
#include "log.h"
#include "mcu.h"

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
		TIM5,TIM6,TIM7,TIM8,
		TIM9,TIM10,TIM11,TIM12,
		TIM13,TIM14
	};
const uint32_t STM32TimerRccList[MCU_TIMER_MAX]={
	0,
		RCC_APB2Periph_TIM1,RCC_APB1Periph_TIM2,RCC_APB1Periph_TIM3,RCC_APB1Periph_TIM4,
		RCC_APB1Periph_TIM5,RCC_APB1Periph_TIM6,RCC_APB1Periph_TIM7,RCC_APB2Periph_TIM8,
		RCC_APB2Periph_TIM9,RCC_APB2Periph_TIM10,RCC_APB2Periph_TIM11,RCC_APB1Periph_TIM12,
		RCC_APB1Periph_TIM13,RCC_APB1Periph_TIM14
	};
/*
	从这个表格可以看出，23457这5个定时器中断入口没有复用，
	简单的定时功能最好用这4个
*/	
const uint8_t STM32TimerIRQList[MCU_TIMER_MAX]={
	0,
		TIM1_UP_TIM10_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn,
		TIM5_IRQn, TIM6_DAC_IRQn, TIM7_IRQn, TIM8_UP_TIM13_IRQn,
		TIM1_BRK_TIM9_IRQn, TIM1_UP_TIM10_IRQn, TIM1_TRG_COM_TIM11_IRQn, TIM8_BRK_TIM12_IRQn,
		TIM8_UP_TIM13_IRQn, TIM8_TRG_COM_TIM14_IRQn
	};
/*
    定时器时钟为84M,
    Tout=((SYSTEM_CLK_PERIOD)*(SYSTEM_CLK_PRESCALER))/Ft us.

	预分频,8400个时钟才触发一次定时器计数 
	那么一个定时器计数的时间就是(1/84M)*8400 = 100us	  
*/	
const u16  STM32TimerTickSet[MCU_TIMER_DEF_MAX*2] =
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
	if(TIM_GetITStatus(TIM3, TIM_FLAG_Update) == SET)
	{										
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);

		if(McuTimerCtrlList[MCU_TIMER_3].retype == 1)
			TIM_Cmd(TIM3, DISABLE);//停止定时器
		if(McuTimerCtrlList[MCU_TIMER_3].Callback != NULL)	
			McuTimerCtrlList[MCU_TIMER_3].Callback();

	}
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
	if(TIM_GetITStatus(TIM5, TIM_FLAG_Update) == SET)
	{										
		TIM_ClearFlag(TIM5, TIM_FLAG_Update);

		if(McuTimerCtrlList[MCU_TIMER_5].retype == 1)
			TIM_Cmd(TIM5, DISABLE);//停止定时器
		if(McuTimerCtrlList[MCU_TIMER_5].Callback != NULL)	
			McuTimerCtrlList[MCU_TIMER_5].Callback();
 
	}
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
	if(TIM_GetITStatus(TIM7, TIM_FLAG_Update) == SET)
	{										
		TIM_ClearFlag(TIM7, TIM_FLAG_Update);
		
		if(McuTimerCtrlList[MCU_TIMER_7].retype == 1)
			TIM_Cmd(TIM7, DISABLE);//停止定时器
		if(McuTimerCtrlList[MCU_TIMER_7].Callback != NULL)	
			McuTimerCtrlList[MCU_TIMER_7].Callback();
		
	}
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
    NVIC_InitTypeDef NVIC_InitStructure;
    

	if(timer == MCU_TIMER_NULL
			|| timer >= MCU_TIMER_MAX)
		return -1;
	
    //打开定时器时钟
    if(timer == MCU_TIMER_1 || timer == MCU_TIMER_8
		|| timer == MCU_TIMER_10 || timer == MCU_TIMER_11
		||timer == MCU_TIMER_9)
		RCC_APB2PeriphClockCmd(STM32TimerRccList[timer], ENABLE);	
	else
    	RCC_APB1PeriphClockCmd(STM32TimerRccList[timer], ENABLE);
	
    //复位定时器
    TIM_Cmd((TIM_TypeDef *)STM32TimerList[timer], DISABLE);
	
    TIM_SetCounter((TIM_TypeDef *)STM32TimerList[timer], 0);
    
	/*中断优先级在底层固定*/
	NVIC_InitStructure.NVIC_IRQChannel = STM32TimerIRQList[timer];	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    TIM_ITConfig((TIM_TypeDef *)STM32TimerList[timer], TIM_IT_Update, ENABLE);//打开定时器中断
    
	McuTimerCtrlList[timer].Callback = NULL;

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
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	McuTimerCtrlList[timer].Callback = callback;
	McuTimerCtrlList[timer].retype = type;
	
	//复位定时器
    TIM_Cmd((TIM_TypeDef *)STM32TimerList[timer], DISABLE);
    TIM_SetCounter((TIM_TypeDef *)STM32TimerList[timer], 0);

	TIM_TimeBaseInitStruct.TIM_Prescaler = STM32TimerTickSet[tickdef]-1;//分频
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
    TIM_TimeBaseInitStruct.TIM_Period = tick - 1;  //周期
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 1;
    TIM_TimeBaseInit((TIM_TypeDef *)STM32TimerList[timer], &TIM_TimeBaseInitStruct);

	TIM_ClearFlag((TIM_TypeDef *)STM32TimerList[timer], TIM_FLAG_Update);
	
	TIM_ITConfig((TIM_TypeDef *)STM32TimerList[timer], TIM_IT_Update, ENABLE);//打开定时器中断

	TIM_Cmd((TIM_TypeDef *)STM32TimerList[timer], ENABLE);//使能定时器(启动)	
	
	return 0;
}

s32 mcu_timer_start(McuTimerNum timer)
{
	TIM_Cmd((TIM_TypeDef *)STM32TimerList[timer], ENABLE);
	
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
	TIM_Cmd((TIM_TypeDef *)STM32TimerList[timer], DISABLE);
	
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
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    if(timer == MCU_TIMER_1 || timer == MCU_TIMER_8
		|| timer == MCU_TIMER_10 || timer == MCU_TIMER_11
		|| timer == MCU_TIMER_9)
		RCC_APB2PeriphClockCmd(STM32TimerRccList[timer], ENABLE);	
	else
    	RCC_APB1PeriphClockCmd(STM32TimerRccList[timer], ENABLE);

	TIM_TimeBaseStructure.TIM_Prescaler = STM32TimerTickSet[tickdef]-1;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //---向上计数模式
    TIM_TimeBaseStructure.TIM_Period= tick - 1; //---自动重装载值
    TIM_TimeBaseInit((TIM_TypeDef *)STM32TimerList[timer], &TIM_TimeBaseStructure);//---初始化定时器 4
    
    //----初始化 TIM4 PWM 模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //---PWM 调制模式 1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //---比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //---输出极性低

	/*默认配置的是通道2*/
	if(ch == MCU_TIMER_CH1)
	{
    	TIM_OC1Init((TIM_TypeDef *)STM32TimerList[timer], &TIM_OCInitStructure);
    	TIM_SetCompare1((TIM_TypeDef *)STM32TimerList[timer], tick*duty/100);
    	TIM_OC1PreloadConfig((TIM_TypeDef *)STM32TimerList[timer], TIM_OCPreload_Enable); 
	}
	else if(ch == MCU_TIMER_CH2)
	{
    	TIM_OC2Init((TIM_TypeDef *)STM32TimerList[timer], &TIM_OCInitStructure);
    	TIM_SetCompare2((TIM_TypeDef *)STM32TimerList[timer], tick*duty/100);
    	TIM_OC2PreloadConfig((TIM_TypeDef *)STM32TimerList[timer], TIM_OCPreload_Enable); 
	}
	else if(ch == MCU_TIMER_CH3)
	{
    	TIM_OC3Init((TIM_TypeDef *)STM32TimerList[timer], &TIM_OCInitStructure);
    	TIM_SetCompare3((TIM_TypeDef *)STM32TimerList[timer], tick*duty/100);
    	TIM_OC3PreloadConfig((TIM_TypeDef *)STM32TimerList[timer], TIM_OCPreload_Enable); 
	}
	else if(ch == MCU_TIMER_CH4)
	{
    	TIM_OC4Init((TIM_TypeDef *)STM32TimerList[timer], &TIM_OCInitStructure);
    	TIM_SetCompare4((TIM_TypeDef *)STM32TimerList[timer], tick*duty/100);
    	TIM_OC4PreloadConfig((TIM_TypeDef *)STM32TimerList[timer], TIM_OCPreload_Enable); 
	}
	else
		return -1;
		
    TIM_ARRPreloadConfig((TIM_TypeDef *)STM32TimerList[timer], ENABLE);

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
	u16 timerch;
	u16 timitcc;
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM2_ICInitStructure;

	if(timer == MCU_TIMER_6 || timer == MCU_TIMER_7)
		return -1;

	if(timer == MCU_TIMER_1 || timer == MCU_TIMER_8
		|| timer == MCU_TIMER_10 || timer == MCU_TIMER_11
		|| timer == MCU_TIMER_9)
		RCC_APB2PeriphClockCmd(STM32TimerRccList[timer], ENABLE);	
	else
    	RCC_APB1PeriphClockCmd(STM32TimerRccList[timer], ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 0xffffffff; //设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler =STM32TimerTickSet[tickdef]; //预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数
	TIM_TimeBaseInit((TIM_TypeDef *)STM32TimerList[timer], &TIM_TimeBaseStructure); // 初始化定时器 2

	if(ch == MCU_TIMER_CH1)
	{
		timerch = TIM_Channel_1;
		timitcc = TIM_IT_CC1;
	}
	else if(ch == MCU_TIMER_CH2)
	{
		timerch = TIM_Channel_2;
		timitcc = TIM_IT_CC2;
	}
	else if(ch == MCU_TIMER_CH3)
	{
 		timerch = TIM_Channel_3;
		timitcc = TIM_IT_CC3;
	}
	else if(ch == MCU_TIMER_CH4)
	{
		timerch = TIM_Channel_4;
		timitcc = TIM_IT_CC4;
	}
	else
		return -1;

	//初始化通道 4
	TIM2_ICInitStructure.TIM_Channel = timerch; //选择输入端 IC4 映射到 TIM2


	if(edge == MCU_TIMER_CAP_RISING)
		TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	else if(edge == MCU_TIMER_CAP_FALLING)
		TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; 
	else if(edge == MCU_TIMER_CAP_BOTHEDGE)
		TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge; 
		
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //配置输入分频,不分频
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//配置输入滤波器 不滤波
	TIM_ICInit((TIM_TypeDef *)STM32TimerList[timer], &TIM2_ICInitStructure);//初始化 TIM2 IC4

	TIM_ClearITPendingBit((TIM_TypeDef *)STM32TimerList[timer], timitcc|TIM_IT_Update); //清除中断标志
	TIM_SetCounter((TIM_TypeDef *)STM32TimerList[timer], 0); 

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
	u16 timitcc;
	u32 value;

	if(timer == MCU_TIMER_6 || timer == MCU_TIMER_7)
		return -1;
	
	if(ch == MCU_TIMER_CH1)
	{
		timitcc = TIM_IT_CC1;
	}
	else if(ch == MCU_TIMER_CH2)
	{
		timitcc = TIM_IT_CC2;
	}
	else if(ch == MCU_TIMER_CH3)
	{
		timitcc = TIM_IT_CC3;
	}
	else if(ch == MCU_TIMER_CH4)
	{
		timitcc = TIM_IT_CC4;
	}
	else
		return -1;


	while(TIM_GetFlagStatus((TIM_TypeDef *)STM32TimerList[timer], timitcc) == RESET)
	{
		if(TIM_GetCounter((TIM_TypeDef *)STM32TimerList[timer]) > 0xffffffff-1000)
			return TIM_GetCounter((TIM_TypeDef *)STM32TimerList[timer]);//超时了,直接返回 CNT 的值
	}
	
	if(ch == MCU_TIMER_CH1)
	{
		value = TIM_GetCapture1((TIM_TypeDef *)STM32TimerList[timer]);
	}
	else if(ch == MCU_TIMER_CH2)
	{
		value = TIM_GetCapture2((TIM_TypeDef *)STM32TimerList[timer]);
	}
	else if(ch == MCU_TIMER_CH3)
	{
		value = TIM_GetCapture3((TIM_TypeDef *)STM32TimerList[timer]);
	}
	else if(ch == MCU_TIMER_CH4)
	{
		value = TIM_GetCapture4((TIM_TypeDef *)STM32TimerList[timer]);
	}
	return value;
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



/*	delay延时基准 未测试
	*/
uint32 McuDelayUs = 1;

