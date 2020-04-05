/**
 * @file            dev_srf05.c
 * @brief           触摸按键驱动程序
 * @author          wujique
 * @date            2019年03月23日 星期二
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:    
 *   作    者:      屋脊雀工作室
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
	超声波测距模块HY-SRF05
	原理：
	1 VCC 是5V供电，实测3.3V也能工作
	2 Trig脚输出至少10us高电平，触发测量。
	3 模块自动发送超声波检测距离。
	4 如测量成功，则在ECHO脚返回一个高电平，电平持续时间就是超声波发射返回时间
		距离  mm = 340*100*10/(1*1000*1000)/2*T
		其中T单位us，公式简化：Xmm = Tus*17/100

		建议测试间隔超过60ms。

	驱动设计：
	1 发送延时使用硬延时。
	2 接收计时用定时器捕获。
*/
#include "mcu.h"
#include "log.h"
#include "dev_srf05.h"


s32 Srf05Gd = -2;
/*
	超声波测距使用PF8 进行捕获，PF8是定时器13的通道1 
*/

#define SRF05_TRIG_PIN MCU_IO_9
#define SRF05_TRIG_PORT MCU_PORT_B

#define SRF05_ECHO_PIN	MCU_IO_8
#define SRF05_ECHO_PORT	MCU_PORT_B

/**
 *@brief:      dev_srf05_trig
 *@details:    触发测量，10us以上高电平
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
static s32 dev_srf05_trig(void)
{
	mcu_io_output_setbit(SRF05_TRIG_PORT, SRF05_TRIG_PIN);
	Delay(1);
	mcu_io_output_resetbit(SRF05_TRIG_PORT, SRF05_TRIG_PIN);
	return 0;
}


s32 dev_srf05_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	mcu_io_config_out(SRF05_TRIG_PORT, SRF05_TRIG_PIN);
	mcu_io_output_resetbit(SRF05_TRIG_PORT, SRF05_TRIG_PIN);

	mcu_io_config_timer(SRF05_ECHO_PORT, SRF05_ECHO_PIN, MCU_TIMER_5);
	
	Srf05Gd = -1;
	return 0;
}

s32 dev_srf05_open(void)
{
	if(Srf05Gd != -1)
		return -1;
	Srf05Gd = 0;
	return 0;
}

s32 dev_srf05_close(void)
{
	Srf05Gd = -1;
	return 0;
}
/**
 *@brief:      dev_touchkey_read
 *@details:    读设备，获取触摸事件
 *@param[in]   u8 *buf    
               u32 count  
 *@param[out]  无
 *@retval:     
 */
s32 dev_srf05_read(u8 *buf, u32 count)
{
	u32 capr,capf;
	u32 len;
	
	if(Srf05Gd != 0)
		return -1;
	
	dev_srf05_trig();
	mcu_timer_cap_init(MCU_TIMER_4, MCU_TIMER_1US, MCU_TIMER_CH1, MCU_TIMER_CAP_RISING);
	mcu_timer_start(MCU_TIMER_4);
	wjq_log(LOG_DEBUG, "1 ");
	capr = mcu_timer_get_cap(MCU_TIMER_4, MCU_TIMER_CH1);
	wjq_log(LOG_DEBUG, "2 ");
	mcu_timer_cap_init(MCU_TIMER_4, MCU_TIMER_1US, MCU_TIMER_CH1, MCU_TIMER_CAP_FALLING);
	mcu_timer_start(MCU_TIMER_4);
	wjq_log(LOG_DEBUG, "3 ");
	capf = mcu_timer_get_cap(MCU_TIMER_4, MCU_TIMER_CH1);
	wjq_log(LOG_DEBUG, "4 ");
	len = capf*17/100;
	wjq_log(LOG_DEBUG, "\r\n%d-%d--len:%d-\r\n", capr, capf, len);
	
	return 0;

}

s32 dev_srf05_write(void)
{
	
	return 0;
}


s32 dev_srf05_test(void)
{
	dev_srf05_init();
	dev_srf05_open();
	
	while(1)
	{
		dev_srf05_read(0,0);
		wjq_log(LOG_DEBUG, "srf05 run\r\n");
		Delay(1000);
	}
}




