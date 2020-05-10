/**
 * @file            dev_buzzer.c
 * @brief           PWM蜂鸣器驱动
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
#include "petite_config.h"

#include "log.h"
#include "mcu_timer.h"
#include "mcu_io.h"
/**
 *@brief:      dev_buzzer_init
 *@details:    初始化蜂鸣器
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 dev_buzzer_init(void)
{
	wjq_log(LOG_INFO, "init buzzer\r\n");
	/*
		用户从手册可以知道管脚和定时器通道的关系
	*/
	mcu_io_config_timer(MCU_PORT_D, MCU_IO_13, MCU_TIMER_4);
	mcu_timer_pwm_init(MCU_TIMER_4, MCU_TIMER_1US, 250, 50, MCU_TIMER_CH2);
	return 0;
}
/**
 *@brief:      dev_buzzer_open
 *@details:    打开蜂鸣器
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 dev_buzzer_open(void)
{
	wjq_log(LOG_INFO, "open buzzer\r\n");
	
	mcu_io_config_timer(MCU_PORT_D, MCU_IO_13, MCU_TIMER_4);
    //---使能 TIM4 
	mcu_timer_start(MCU_TIMER_4);
	
	return 0;
}
/**
 *@brief:      dev_buzzer_close
 *@details:    关闭蜂鸣器
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 dev_buzzer_close(void)
{
	wjq_log(LOG_INFO, "close buzzer\r\n");
    mcu_timer_stop(MCU_TIMER_4); //---关闭定时器 TIM4 


	/*关闭蜂鸣器时，要将IO改为普通IO，并且输出低电平，否则蜂鸣器会造成大电流*/
	mcu_io_config_out(MCU_PORT_D, MCU_IO_13);
	mcu_io_output_resetbit(MCU_PORT_D, MCU_IO_13);
	
	return 0;
}
/**
 *@brief:      dev_buzzer_test
 *@details:    测试蜂鸣器
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void dev_buzzer_test(void)
{
    
}

