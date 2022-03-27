


//#if (SYS_USE_TS_ADC_CASE == 1)
#if 0
#include "mcu_adc.h"

#define DEV_TP_S_PORT GPIOD
#define DEV_TP_S0 	  GPIO_Pin_11	
#define DEV_TP_S1 	  GPIO_Pin_12

#define DEV_TP_S0_L GPIO_ResetBits(DEV_TP_S_PORT, DEV_TP_S0)
#define DEV_TP_S0_H GPIO_SetBits(DEV_TP_S_PORT, DEV_TP_S0)

#define DEV_TP_S1_L GPIO_ResetBits(DEV_TP_S_PORT, DEV_TP_S1)
#define DEV_TP_S1_H GPIO_SetBits(DEV_TP_S_PORT, DEV_TP_S1)


#define DEV_TP_PRESS_SCAN {DEV_TP_S1_H;DEV_TP_S0_L;}
#define DEV_TP_SCAN_X {DEV_TP_S1_L;DEV_TP_S0_L;}
#define DEV_TP_SCAN_Y {DEV_TP_S1_L;DEV_TP_S0_H;}


#define TP_ADC_USE_TIMER MCU_TIMER_7

/*
测量到的压力电压阈值，小于PENdown，认为是下笔，
大于penup认为是起笔，两者之间不处理
这个值跟压力不一样，
上送的样点压力最小是0，也就是起笔后，
最大是电压阈值为0的时候（可能达不到），
通过计算转换，也就是0-400；0起笔，400，电压压力为0
*/
#define DEV_TP_PENDOWN_GATE (400)
#define DEV_TP_PENUP_GATE	 (2000)

static u8 TouchScreenStep = 9;
s32 TsAdcGd = -2;

void dev_ts_adc_tr(void);
s32 dev_ts_adc_task(u16 dac_value);

/*
	定时，单位10us
*/
static s32 dev_ts_adc_timerrun(u32 time)
{
	mcu_timer_config(MCU_TIMER_7, MCU_TIMER_10US, time, dev_ts_adc_tr, MCU_TIMER_ONE);
	mcu_timer_start(MCU_TIMER_7);	
}

s32 dev_ts_adc_init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DEV_TP_S0 | DEV_TP_S1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(DEV_TP_S_PORT, &GPIO_InitStructure);

	DEV_TP_PRESS_SCAN;

	mcu_adc_init(dev_ts_adc_task);
	mcu_timer_init(TP_ADC_USE_TIMER);
	TsAdcGd = -1;

	return 0;
}

s32 dev_ts_adc_open(void)
{
	if(TsAdcGd != -1)
		return -1;
	/* wjq bug
	
	要清定时器跟ADC中断标志	
	*/
	DEV_TP_PRESS_SCAN;
	mcu_adc_start_conv(ADC_Channel_9);
	TsAdcGd = 0;
	return 0;
}

s32 dev_ts_adc_close(void)
{
		return 0;
}
/**
 *@brief:      dev_touchscreen_tr
 *@details:    重新开始一次检测流程
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void dev_ts_adc_tr(void)
{
	if(TouchScreenStep	== 3)
	{
		mcu_adc_start_conv(ADC_Channel_8);
	}
	else
	{
		mcu_adc_start_conv(ADC_Channel_9);	
	}
}

/**
 *@brief:      dev_touchscreen_task
 *@details:    触摸屏ADC转换流程
 *@param[in]   u16 adc_value  
 *@param[out]  无
 *@retval:     
 */
s32 dev_ts_adc_task(u16 dac_value)
{
	static u16 pre_y, pre_x;
	static u16 sample_x;
	static u8 pendownup = 1;
	struct ts_sample tss;
	
	if(TsAdcGd != 0)
		return -1;
	
	if(TouchScreenStep == 0)//压力检测第一步ADC转换结束
	{	
		pre_y = dac_value;
		TouchScreenStep	= 1;
		mcu_adc_start_conv(ADC_Channel_8);
	}
	else if(TouchScreenStep == 1)
	{
		pre_x = dac_value;
		//TS_DEBUG(LOG_DEBUG, "--press :%d %d\r\n", pre_y, pre_x);

		if(pre_x + DEV_TP_PENDOWN_GATE > pre_y)
		{
			TouchScreenStep	= 2;
			DEV_TP_SCAN_X;
			dev_ts_adc_timerrun(2);
		}
		else if(pre_x + DEV_TP_PENUP_GATE < pre_y)//没压力，不进行XY轴检测
		{
			/* 起笔只上送一点缓冲*/
			if(pendownup == 0)
			{
				pendownup = 1;
				tss.pressure = 0;//压力要搞清楚怎么计算
				tss.x = 0xffff;
				tss.y = 0xffff;
				dev_touchscreen_write(&tss,1);
				
			}
			TouchScreenStep	= 0;

			DEV_TP_PRESS_SCAN;
			//打开一个定时器，定时时间到了才进行压力检测
			dev_ts_adc_timerrun(100);
		}
		else
		{
			/*上下笔的过渡，丢弃*/
			TouchScreenStep	= 0;

			DEV_TP_PRESS_SCAN;

			dev_ts_adc_timerrun(20);
		}
	}
	else if(TouchScreenStep == 2)
	{
		sample_x =  dac_value;
		
		TouchScreenStep	= 3;
		DEV_TP_SCAN_Y;

		dev_ts_adc_timerrun(2);
	}
	else if(TouchScreenStep == 3)//一轮结束，重启启动压力检测
	{
		tss.pressure = DEV_TP_PENDOWN_GATE-(pre_y - pre_x);//压力要搞清楚怎么计算
		tss.x = sample_x;
		tss.y = dac_value;
		dev_touchscreen_write(&tss,1);
		//TS_DEBUG(LOG_DEBUG, "tp :%d, %d, %d\r\n", tss.pressure, tss.x, tss.y);
		pendownup = 0;
		
		TouchScreenStep	= 0;
		DEV_TP_PRESS_SCAN;
		
		dev_ts_adc_timerrun(2);
	}
	else//异常，启动压力检测
	{
		TouchScreenStep	= 0;
		DEV_TP_PRESS_SCAN;

		dev_ts_adc_timerrun(100);
	}

	return 0;
}

/**
 *@brief:      dev_touchscreen_test
 *@details:       触摸屏采样测试
 *@param[in]  void  
 *@param[out]  无
 *@retval:     
 */
s32 dev_touchscreen_test(void)
{
	/*轮询，不使用中断 把MCU_ADC_IRQ屏蔽*/
#ifndef MCU_ADC_IRQ
	u16 adc_y_value;
	u16 adc_x_value;
	u16 pre;//压力差

	dev_touchscreen_init();
	dev_touchscreen_open();
	
	while(1)
	{
		Delay(1000);
		/*  检测压力 */
		DEV_TP_PRESS_SCAN;
		adc_y_value = mcu_adc_get_conv(ADC_Channel_9);
		adc_x_value = mcu_adc_get_conv(ADC_Channel_8);
		pre = adc_y_value-adc_x_value;
		TS_DEBUG(LOG_FUN,"touch pre:%d, %d, %d\r\n", adc_x_value, adc_y_value, pre);	


		if(adc_x_value + 200 > adc_y_value)//200为测试阀门，实际中要调试
		{
			/* 检测X轴*/
			DEV_TP_SCAN_X;
			adc_x_value = mcu_adc_get_conv(ADC_Channel_9);
			//uart_printf("ADC_Channel_8:%d\r\n", adc_x_value);
			
			/* 检测Y轴*/
			DEV_TP_SCAN_Y;
			adc_y_value = mcu_adc_get_conv(ADC_Channel_8);
			//uart_printf("ADC_Channel_8:%d\r\n", adc_y_value);

			TS_DEBUG(LOG_FUN,"----------get a touch:%d, %d, %d\r\n", adc_x_value, adc_y_value, pre);

		}

	}
#else//中断模式执行触摸屏检测流程
	struct ts_sample s;
	s32 ret;
	u8 pensta = 1;//没接触
	
	dev_touchscreen_init();
	dev_touchscreen_open();
	
	while(1)
	{
		ret = dev_touchscreen_read(&s,1);
		if(ret == 1)
		{
			if(s.pressure != 0 && pensta == 1)
			{
				pensta = 0;
				wjq_log(LOG_FUN, "pen down\r\n");
				wjq_log(LOG_FUN, ">%d %d %d-\r\n", s.pressure, s.x, s.y);
			}
			else if(s.pressure == 0 && pensta == 0)
			{	
				pensta = 1;
				wjq_log(LOG_FUN, "\r\n--------pen up--------\r\n");	

			}
		}
	}
#endif

}
#endif

