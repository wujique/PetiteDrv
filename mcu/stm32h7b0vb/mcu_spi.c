/**
 * @file            mcu_spi.c
 * @brief           片上SPI控制器驱动
 * @author          test
 * @date            2017年10月26日 星期四
 * @version         初稿
 * @par             
 * @par History:
 * 1.日    期:      2017年10月26日 星期四
 *   作    者:      test
 *   修改内容:      创建文件
		版权说明：
		1 源码归屋脊雀工作室所有。
		2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且保留版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/
#include "mcu.h"
#include "log.h"

//#define MCU_SPI_DEBUG

#ifdef MCU_SPI_DEBUG
#define SPI_DEBUG	uart_printf 
#else
#define SPI_DEBUG(a, ...)
#endif

extern SPI_HandleTypeDef hspi1;

#define COG_CS_PIN                    GPIO_PIN_15                 
#define COG_CS_GPIO_PORT              GPIOA                      
//#define COG_CS_GPIO_CLK               RCC_APB2Periph_GPIOA

#define MCU_SPI_WAIT_TIMEOUT 0x40000

/*
	相位配置，一共四种模式
*/
typedef struct
{
	u16 CPOL;
	u16 CPHA;	
}_strSpiModeSet;

#if 0
const _strSpiModeSet SpiModeSet[SPI_MODE_MAX]=
	{
		{SPI_CPOL_Low, SPI_CPHA_1Edge},
		{SPI_CPOL_Low, SPI_CPHA_2Edge},
		{SPI_CPOL_High, SPI_CPHA_1Edge},
		{SPI_CPOL_High, SPI_CPHA_2Edge}
	};
#endif

/**
 *@brief:      mcu_spi_init
 *@details:    初始化SPI控制器，暂时支持SPI3
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_init(const char *name, const SpiIoDef *io)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	#if 0
	HAL_GPIO_WritePin(COG_CS_GPIO_PORT, COG_CS_PIN, GPIO_PIN_SET);
	/*Configure GPIO pins : VI2C_SDA_Pin PA15 */
  	GPIO_InitStruct.Pin = COG_CS_PIN;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  	HAL_GPIO_Init(COG_CS_GPIO_PORT, &GPIO_InitStruct);
	#endif
	SPI_DEBUG("hspi init!\r\n");
	MX_SPI1_Init();
    return 0;
}

/**
 *@brief:      mcu_spi_open
 *@details:       打开SPI
 *@param[in]   SPI_DEV dev  ：SPI号
               u8 mode      模式
               u16 pre      预分频
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_open(const char *name, SPI_MODE mode, u16 KHz)
{
	SPI_DEBUG("mcu_hspi_open\r\n");
    return 0;
}
/**
 *@brief:      mcu_spi_close
 *@details:    关闭SPI 控制器
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_close(const char *name)
{
	SPI_DEBUG("mcu_hspi_close\r\n");
    return 0;
}
/**
 *@brief:      mcu_spi_transfer
 *@details:    SPI 传输
 *@param[in]   u8 *snd  
               u8 *rsv  
               s32 len  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_transfer(const char *name, u8 *snd, u8 *rsv, s32 len)
{
    s32 i = 0;
    s32 pos = 0;
    u32 time_out = 0;
    u16 ch;

    if( ((snd == NULL) && (rsv == NULL)) || (len < 0) ) {
    	SPI_DEBUG("return -1\r\n");
        return -1;
    }

	if ( snd == NULL ) {
		HAL_SPI_Receive(&hspi1, rsv, len, 100);	
		SPI_DEBUG("HAL_SPI_Receive!\r\n");
	} else if ( rsv == NULL) {
		HAL_SPI_Transmit(&hspi1, snd, len, 100);
		SPI_DEBUG("HAL_SPI_Transmit!\r\n");
	} else {
		HAL_SPI_TransmitReceive(&hspi1, snd, rsv, len, 100);
		SPI_DEBUG("HAL_SPI_TransmitReceive!\r\n");
	}

    return i;
}


#if 0
/**
 *@brief:      mcu_spi_cs
 *@details:    操控对应SPI的CS
 *@param[in]   
               u8 sta   1 高电平，0 低电平     
 *@param[out]  无
 *@retval:     
 */
s32 mcu_lcd_spi_cs(u8 sta)
{
	switch(sta)
	{
		case 1:
			HAL_GPIO_WritePin(COG_CS_GPIO_PORT, COG_CS_PIN, GPIO_PIN_SET);
			break;
			
		case 0:
			HAL_GPIO_WritePin(COG_CS_GPIO_PORT, COG_CS_PIN, GPIO_PIN_RESET);
			break;
			
		default:
			return -1;

	}

	return 0;
}
#endif

