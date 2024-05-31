#include "mcu.h"
#include "log.h"


extern const GPIO_TypeDef *Stm32H7B0PortList[MCU_PORT_MAX];

/*
	VI2C io口配置 ，
	
	*/
void mcu_vi2c_io_config_out(MCU_PORT port, MCU_IO pin)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(port == NULL) return;

	GPIO_InitStruct.Pin = pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init((GPIO_TypeDef *)Stm32H7B0PortList[port], &GPIO_InitStruct);

}

void mcu_vi2c_io_config_in(MCU_PORT port, MCU_IO pin)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(port == NULL)
		return;

  	GPIO_InitStruct.Pin = pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	HAL_GPIO_Init((GPIO_TypeDef *)Stm32H7B0PortList[port], &GPIO_InitStruct);
}


