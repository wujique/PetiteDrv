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
#include "bus/bus_spi.h"
#include "board_sysconf.h"
#include "petite_dev.h"

#define MCU_SPI_DEBUG

#ifdef MCU_SPI_DEBUG
#define SPI_DEBUG	wjq_log 
#else
#define SPI_DEBUG(a, ...)
#endif


#define MCU_SPI_WAIT_TIMEOUT 0x40000

/*
	相位配置，一共四种模式
*/
typedef struct
{
	u16 CPOL;
	u16 CPHA;	
}_strSpiModeSet;

const _strSpiModeSet SpiModeSet[SPI_MODE_MAX]=
	{
		{SPI_CPOL_Low, SPI_CPHA_1Edge},
		{SPI_CPOL_Low, SPI_CPHA_2Edge},
		{SPI_CPOL_High, SPI_CPHA_1Edge},
		{SPI_CPOL_High, SPI_CPHA_2Edge}
	};

/**
 *@brief:      mcu_spi_init
 *@details:    初始化SPI控制器，暂时支持SPI3
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_init(const DevSpi *dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStruct;
	uint8_t GPIO_AF;
	uint32_t RCC_CLK;
	uint16_t pinsource;
	
	/*配置IO口*/
    GPIO_InitStructure.GPIO_Pin = dev->clkpin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//---复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//---推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//---100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//---上拉
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[dev->clkport], &GPIO_InitStructure);//---初始化

	GPIO_InitStructure.GPIO_Pin = dev->misopin;
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[dev->misoport], &GPIO_InitStructure);//---初始化

	GPIO_InitStructure.GPIO_Pin = dev->mosipin;
    GPIO_Init((GPIO_TypeDef *)Stm32PortList[dev->mosiport], &GPIO_InitStructure);//---初始化


    //配置引脚复用映射
    if(strcmp(dev->pdev.name, "SPI3") == 0) {
		GPIO_AF = GPIO_AF_SPI3;
    } else if(strcmp(dev->pdev.name, "SPI1") == 0) {
		GPIO_AF = GPIO_AF_SPI1;
    } else if(strcmp(dev->pdev.name, "SPI2") == 0) {
		GPIO_AF = GPIO_AF_SPI2;
    }
	
	pinsource = math_log2(dev->clkpin);
    GPIO_PinAFConfig((GPIO_TypeDef *)Stm32PortList[dev->clkport], pinsource,  GPIO_AF); //复用
    pinsource = math_log2(dev->misopin);
    GPIO_PinAFConfig((GPIO_TypeDef *)Stm32PortList[dev->misoport], pinsource, GPIO_AF); //复用
    pinsource = math_log2(dev->mosipin);
    GPIO_PinAFConfig((GPIO_TypeDef *)Stm32PortList[dev->mosiport], pinsource, GPIO_AF); //复用

    // ---使能时钟
    if(strcmp(dev->pdev.name, "SPI3") == 0) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    } else if(strcmp(dev->pdev.name, "SPI1") == 0) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    } else if(strcmp(dev->pdev.name, "SPI2") == 0) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    }
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
s32 mcu_hspi_open(DevSpiNode *node, SPI_MODE mode, u16 KHz)
{
	SPI_InitTypeDef SPI_InitStruct;
	SPI_TypeDef* SPIC;
		
	if (mode >= SPI_MODE_MAX) return -1;

	if (strcmp(node->pnode.pdev->name, "SPI3") == 0) {
		SPIC = SPI3;
    } else if (strcmp(node->pnode.pdev->name, "SPI1") == 0) {
		SPIC = SPI1;
    } else if (strcmp(node->pnode.pdev->name, "SPI2") == 0) {
		SPIC = SPI2;
    }

	SPI_I2S_DeInit(SPIC);
	SPI_Cmd(SPIC, DISABLE); 

	/*need fix : 根据参数KHz计算分频*/
	
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//---双线双向全双工
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//---主模式
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//---8bit帧结构
    SPI_InitStruct.SPI_CPOL = SpiModeSet[mode].CPOL;
    SPI_InitStruct.SPI_CPHA = SpiModeSet[mode].CPHA;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; //---SPI_NSS_Hard; 片选由硬件管理，SPI控制器不管理
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;  //---预分频
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//---数据传输从 MSB 位开始
    SPI_InitStruct.SPI_CRCPolynomial = 7;//---CRC 值计算的多项式

    SPI_Init(SPIC, &SPI_InitStruct);

	SPI_Cmd(SPIC, ENABLE);
			
    return 0;
}
/**
 *@brief:      mcu_spi_close
 *@details:    关闭SPI 控制器
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_close(DevSpiNode *node)
{
    SPI_TypeDef* SPIC;
		
	if (strcmp(node->pnode.pdev->name, "SPI3") == 0) {
		SPIC = SPI3;
    } else if (strcmp(node->pnode.pdev->name, "SPI1") == 0) {
		SPIC = SPI1;
    } else if (strcmp(node->pnode.pdev->name, "SPI2") == 0) {
		SPIC = SPI2;
    }
	
	SPI_Cmd(SPIC, DISABLE);
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
s32 mcu_hspi_transfer(DevSpiNode *node, u8 *snd, u8 *rsv, s32 len)
{
    s32 i = 0;
    s32 pos = 0;
    u32 time_out = 0;
    u16 ch;
	SPI_TypeDef* SPIC;
	
	if (node == NULL) return -1;
	
    if ( ((snd == NULL) && (rsv == NULL)) || (len < 0) ) {
        return -2;
    }
	
    if (strcmp(node->pnode.pdev->name, "SPI3") == 0) {
		SPIC = SPI3;
    } else if (strcmp(node->pnode.pdev->name, "SPI1") == 0) {
		SPIC = SPI1;
    } else if (strcmp(node->pnode.pdev->name, "SPI2") == 0) {
		SPIC = SPI2;
    }
    /* 忙等待 */
    time_out = 0;
    while (SPI_I2S_GetFlagStatus(SPIC, SPI_I2S_FLAG_BSY) == SET) {
        if (time_out++ > MCU_SPI_WAIT_TIMEOUT) {
            return(-3);
        }
    }

    /* 清空SPI缓冲数据，防止读到上次传输遗留的数据 */
    time_out = 0;
    while (SPI_I2S_GetFlagStatus(SPIC, SPI_I2S_FLAG_RXNE) == SET) {
        SPI_I2S_ReceiveData(SPIC);
        if(time_out++ > MCU_SPI_WAIT_TIMEOUT) {
            return(-4);
        }
    }

    /* 开始传输 */
    for(i=0; i < len; ) {
        // 写数据
        /*发送指针为NULL，说明仅仅是读数据 */
        if(snd == NULL) {
            SPI_I2S_SendData(SPIC, 0xff);
        } else {
            ch = (u16)snd[i];
            SPI_I2S_SendData(SPIC, ch);
        }
        i++;
        
        // 等待接收结束
        time_out = 0;
        while (SPI_I2S_GetFlagStatus(SPIC, SPI_I2S_FLAG_RXNE) == RESET) {
            time_out++;
            if (time_out > MCU_SPI_WAIT_TIMEOUT) {
                return -5;
            }    
        }
        // 读数据
        /* 接收指针为空，读数据后丢弃 */
        if (rsv == NULL) {
            SPI_I2S_ReceiveData(SPIC);
        } else { 
            ch = SPI_I2S_ReceiveData(SPIC);
            rsv[pos] = (u8)ch;
        } 
        pos++;

    }

    return i;
}


