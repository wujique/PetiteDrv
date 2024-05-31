/**
 * @file            mcu_io.c
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

const GPIO_TypeDef *Stm32PortList[MCU_PORT_MAX] = {NULL, GPIOA,  GPIOB, GPIOC, GPIOD,
									GPIOE, GPIOF, GPIOG, GPIOH,
									GPIOI, GPIOJ, GPIOK};

/*
	初始化所有IO的时钟
*/
void mcu_io_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOJ, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOK, ENABLE);
}

uint8_t GpioTimDef[MCU_TIMER_MAX]=
	{
		NULL,
		GPIO_AF_TIM1,
		GPIO_AF_TIM2,	
		GPIO_AF_TIM3,
		GPIO_AF_TIM4,
		GPIO_AF_TIM5,
		NULL,	
		NULL,
		GPIO_AF_TIM8,
		GPIO_AF_TIM9,
		GPIO_AF_TIM10,	
		GPIO_AF_TIM11,
		GPIO_AF_TIM12,
		GPIO_AF_TIM13,
		GPIO_AF_TIM14,	
	};

s32 mcu_io_config_timer(MCU_PORT port, MCU_IO pin, McuTimerNum timer)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t pinsource;
	uint8_t GPIO_AF;
	
	if(port == NULL)
		return -1;

	if(timer == MCU_TIMER_6 || timer == MCU_TIMER_7)
		return -1;
	
	GPIO_AF = GpioTimDef[timer];

	pinsource = math_log2(pin);
	/*need fix*/
	GPIO_PinAFConfig((GPIO_TypeDef *)Stm32PortList[port], pinsource, GPIO_AF); //---管脚复用为 TIM4功能
	
    GPIO_InitStructure.GPIO_Pin = pin; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //---复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //---速度 50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //---推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //---上拉
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure);
}

s32 mcu_io_config_dac(MCU_PORT port, MCU_IO pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//---模拟模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//---下拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure);//---初始化 GPIO
}
/*
	将IO口配置为输入， 上拉
*/
void mcu_io_config_in(MCU_PORT port, MCU_IO pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	if(port == NULL)
		return;
	
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure);//初始化  	
}


void mcu_io_config_out(MCU_PORT port, MCU_IO pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if(port == NULL)
		return;
	
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure);//初始化
}

void mcu_io_output_setbit(MCU_PORT port, MCU_IO pin)
{
	if(port == NULL)
		return;
	
	GPIO_SetBits((GPIO_TypeDef *)Stm32PortList[port], pin);
}

void mcu_io_output_resetbit(MCU_PORT port, MCU_IO pin)
{
	if(port == NULL)
		return;
	
	GPIO_ResetBits((GPIO_TypeDef *)Stm32PortList[port], pin);
}		

u8 mcu_io_input_readbit(MCU_PORT port, MCU_IO pin)
{
	if(port == NULL)
		return Bit_SET;
	
	return GPIO_ReadInputDataBit((GPIO_TypeDef *)Stm32PortList[port], pin);
}

u16 mcu_io_input_readport(MCU_PORT port)
{
	if(port == NULL)
		return NULL;
	
	return GPIO_ReadInputData((GPIO_TypeDef *)Stm32PortList[port]);
}


void mcu_vi2c_io_config_out(MCU_PORT port, MCU_IO pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if(port == NULL)
		return;
	
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure);//初始化
}

void mcu_vi2c_io_config_in(MCU_PORT port, MCU_IO pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	if(port == NULL)
		return;
	
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[port], &GPIO_InitStructure);//初始化  	
}


