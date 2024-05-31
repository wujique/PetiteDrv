/**
 * @file            mcu_i2c.c
 * @brief           
 * @author          test
 * @date            2017年10月30日 星期一
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:      2017年10月30日 星期一
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
#include "bus_i2c.h"

#define MCU_I2C_DEBUG

#ifdef MCU_I2C_DEBUG
#define I2C_DEBUG	wjq_log 
#else
#define I2C_DEBUG(a, ...)
#endif

extern const GPIO_TypeDef *Stm32PortList[MCU_PORT_MAX];

#define SCCB_TIMEOUT_MAX    10000
	
void SCCB_GPIO_Config(I2C_TypeDef* I2CDef)
{
	
	I2C_InitTypeDef  I2C_InitStruct;	  
	/****** Configures the I2C1 used for OV9655 camera module configuration *****/
	
	/* I2C1 clock enable */
	if(I2CDef == I2C1)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	else if(I2CDef == I2C2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

	/* Configure I2C2 */
	/* I2C DeInit */
	I2C_DeInit(I2CDef);
	  
	///* Enable the I2C peripheral */
	//I2C_Cmd(CAREMA_SCCB_I2C, ENABLE);
	
	/* Set the I2C structure parameters */
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0xFE;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 10000;
	
	/* Initialize the I2C peripheral w/ selected parameters */
	I2C_Init(I2CDef, &I2C_InitStruct);
	
	I2C_Cmd(I2CDef, ENABLE);
	
	I2C_AcknowledgeConfig(I2CDef, ENABLE);

}
	
uint8_t bus_sccb_writereg(I2C_TypeDef* I2CDef, uint8_t DeviceAddr, uint16_t Addr, uint8_t Data)
{
	
	uint32_t timeout = SCCB_TIMEOUT_MAX;
	
	/* Generate the Start Condition */
	I2C_GenerateSTART(I2CDef, ENABLE);
	
	/* Test on I2C1 EV5 and clear it */
	timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_MODE_SELECT))
	{
	  /* If the timeout delay is exceeded, exit with error code */
	  if ((timeout--) == 0) return 0xFF;
	}
	 
	/* Send DCMI selected device slave Address for write */
	I2C_Send7bitAddress(I2CDef, DeviceAddr, I2C_Direction_Transmitter);
	
	/* Test on I2C1 EV6 and clear it */
	timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	  /* If the timeout delay is exceeded, exit with error code */
	  if ((timeout--) == 0) return 0xFF;
	}
	
	/* Send I2C1 location address LSB */
	I2C_SendData(I2CDef, (uint8_t)(Addr));
	
	/* Test on I2C1 EV8 and clear it */
	timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	  /* If the timeout delay is exceeded, exit with error code */
	  if ((timeout--) == 0) return 0xFF;
	}
	
	/* Send Data */
	I2C_SendData(I2CDef, Data);
	
	/* Test on I2C1 EV8 and clear it */
	timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	  /* If the timeout delay is exceeded, exit with error code */
	  if ((timeout--) == 0) return 0xFF;
	}  
	
	/* Send I2C1 STOP Condition */
	I2C_GenerateSTOP(I2CDef, ENABLE);
	
	/* If operation is OK, return 0 */
	return 0;

}
	
uint8_t bus_sccb_readreg(I2C_TypeDef* I2CDef, uint8_t DeviceAddr, uint16_t Addr)
{
  uint32_t timeout = SCCB_TIMEOUT_MAX;
  uint8_t Data = 0;

  /* Generate the Start Condition */
  I2C_GenerateSTART(I2CDef, ENABLE);

  /* Test on I2C1 EV5 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_MODE_SELECT))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  }
  
  /* Send DCMI selected device slave Address for write */
  I2C_Send7bitAddress(I2CDef, DeviceAddr, I2C_Direction_Transmitter);
 
  /* Test on I2C1 EV6 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  }

  /* Send I2C1 location address LSB */
  I2C_SendData(I2CDef, (uint8_t)(Addr));

  /* Test on I2C1 EV8 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Clear AF flag if arised */
  I2CDef->SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(I2CDef, ENABLE);
  
  /* Test on I2C1 EV6 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_MODE_SELECT))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Send DCMI selected device slave Address for write */
  I2C_Send7bitAddress(I2CDef, DeviceAddr, I2C_Direction_Receiver);

  /* Test on I2C1 EV6 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  }
 
  /* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(I2CDef, DISABLE);

  /* Test on I2C1 EV7 and clear it */
  timeout = SCCB_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2CDef, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
	/* If the timeout delay is exceeded, exit with error code */
	if ((timeout--) == 0) return 0xFF;
  }

  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(I2CDef, ENABLE);

  /* Receive the Data */
  Data = I2C_ReceiveData(I2CDef);

  /* return the read data */
  return Data;
}

s32 mcu_hi2c_init(DevI2c * dev)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure I2C GPIOs */	
	GPIO_InitStructure.GPIO_Pin = dev->sclpin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[dev->sclport], &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = dev->sdapin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[dev->sdaport], &GPIO_InitStructure);

	if (strcmp(dev->pnode.name, "I2C1") == 0) {
		SCCB_GPIO_Config(I2C1);
    } else if (strcmp(dev->pnode.name, "I2C2") == 0) {
		SCCB_GPIO_Config(I2C2);
    } else {
		return -1;
	}
	
	return 0;
}


s32 mcu_hi2c_transfer(DevI2cNode *node, u8 addr, u8 rw, u8* data, s32 datalen)
{
	if(rw == MCU_I2C_MODE_W)//写
    {

    }
    else if(rw == MCU_I2C_MODE_R)//读
    {
		
    }
	return -1;
}

