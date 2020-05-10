/**
 * @file            mcu_bsp_stm32.c
 * @brief           adc驱动
 * @author          wujique
 * @date            2017年12月8日 星期五
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2017年12月8日 星期五
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
	封装IO操作，以便移植到其他芯片
*/
#include "mcu.h"
#include "log.h"
#include "mcu_io.h"

const GPIO_TypeDef *Stm32H750PortList[MCU_PORT_MAX] = {NULL, GPIOA,  GPIOB, GPIOC, GPIOD,
									GPIOE, GPIOF, GPIOG};

/*
	初始化所有IO的时钟
*/
void mcu_io_init(void)
{
	  /* GPIO Ports Clock Enable */
  	__HAL_RCC_GPIOA_CLK_ENABLE();
  	__HAL_RCC_GPIOB_CLK_ENABLE();
  	__HAL_RCC_GPIOC_CLK_ENABLE();
  	__HAL_RCC_GPIOD_CLK_ENABLE();	
  	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	return;
}


s32 mcu_io_config_timer(MCU_PORT port, MCU_IO pin, McuTimerNum timer)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(port == NULL)
		return -1;
	/*
	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
    GPIO_InitStructure.GPIO_Pin = pin; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //---复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //---速度 50MHz
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure);
*/
	return 0;
}

s32 mcu_io_config_dac(MCU_PORT port, MCU_IO pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
/*
	GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//---模拟模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure);//---初始化 GPIO
*/
	return 0;
}

void mcu_io_config_in(MCU_PORT port, MCU_IO pin)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(port == NULL)
		return;

  	GPIO_InitStruct.Pin = pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	HAL_GPIO_Init((GPIO_TypeDef *)Stm32H750PortList[port], &GPIO_InitStruct);
}


void mcu_io_config_out(MCU_PORT port, MCU_IO pin)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(port == NULL)
		return;

	GPIO_InitStruct.Pin = pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init((GPIO_TypeDef *)Stm32H750PortList[port], &GPIO_InitStruct);

}

/*
	这个要改，改为设置功能，而不是设置所谓的AF，AF是STM32的概念
	要改为，配置为PWM、DAC、ADC等通用功能
*/
void mcu_io_config_af(MCU_PORT port, MCU_IO pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(port == NULL)
		return;
/*
	GPIO_InitStructure.GPIO_Pin = pin; //GPIOA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度 100MHz
	GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure); //初始化 PA3
*/
}

void mcu_io_output_setbit(MCU_PORT port, MCU_IO pin)
{
	if(port == NULL)
		return;
	
	HAL_GPIO_WritePin((GPIO_TypeDef *)Stm32H750PortList[port], pin, GPIO_PIN_SET);
}

void mcu_io_output_resetbit(MCU_PORT port, MCU_IO pin)
{
	if(port == NULL)
		return;
	
	HAL_GPIO_WritePin((GPIO_TypeDef *)Stm32H750PortList[port], pin, GPIO_PIN_RESET);
}		

u8 mcu_io_input_readbit(MCU_PORT port, MCU_IO pin)
{
	if(port == NULL)
		return GPIO_PIN_SET;
	
	return HAL_GPIO_ReadPin((GPIO_TypeDef *)Stm32H750PortList[port], pin);
}

u16 mcu_io_input_readport(MCU_PORT port)
{
	if(port == NULL)
		return GPIO_PIN_SET;
	
	return 0xff;//GPIO_ReadInputData((GPIO_TypeDef *)Stm32PortList[port]);
}



