/**
 * @file            mcu_i2s.c
 * @brief           CPU 片上设备I2S驱动
 * @author          wujique
 * @date            2017年11月16日 星期四
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2017年11月16日 星期四
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

#include "stm32f4xx.h"
#include "log.h"


#include "audio_pipeline.h"

#include "board_sysconf.h"

extern s32 fun_sound_set_free_buf(u8 index);



/**
 *@brief:      mcu_i2s_init
 *@details:    初始化I2S接口硬件
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2s_init (u8 type)
{
	GPIO_InitTypeDef GPIO_InitStructure;

/*		pochard
		LRC 	PA15--
		BCLK	PC10--
		ADCDAT 	PC11
		DACDAT	PB5--
		MCLK	PC7--
	*/
	if(type == 1) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);	// 初始化时钟

		GPIO_PinAFConfig(GPIOA,	GPIO_PinSource15,		GPIO_AF_SPI3);
		GPIO_PinAFConfig(GPIOC,	GPIO_PinSource10,		GPIO_AF_SPI3);	
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11,		GPIO_AF5_SPI3);
		GPIO_PinAFConfig(GPIOB,	GPIO_PinSource5,		GPIO_AF_SPI3);	
		GPIO_PinAFConfig(GPIOC,	GPIO_PinSource7,		GPIO_AF_SPI3);
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 速度等级
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 推挽输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//	无上下拉

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_11;		
		GPIO_Init(GPIOC,&GPIO_InitStructure);
	}
	
	/*
		LRC 	PB12
		BCLK	PB13
		ADCDAT 	PC2
		DACDAT	PC3
		MCLK	PC6
	*/
	if(type == 2) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);	// 初始化时钟

		GPIO_PinAFConfig(GPIOB,	GPIO_PinSource12,		GPIO_AF_SPI2);
		GPIO_PinAFConfig(GPIOB,	GPIO_PinSource13,		GPIO_AF_SPI2);	
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource2,		GPIO_AF6_SPI2);
		GPIO_PinAFConfig(GPIOC,	GPIO_PinSource3,		GPIO_AF_SPI2);	
		GPIO_PinAFConfig(GPIOC,	GPIO_PinSource6,		GPIO_AF_SPI2);
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 速度等级
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 推挽输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//	无上下拉
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6;		
		GPIO_Init(GPIOC,&GPIO_InitStructure);
	}
}
/**
 *@brief:      mcu_i2s_config
 *@details:    I2S配置
 *@param[in]   u32 AudioFreq   频率
               u16 Standard    标准
               u16 DataFormat  格式
 *@param[out]  无
 *@retval:     
 */
void mcu_i2s_config(u32 AudioFreq, u16 Standard,u16 DataFormat)
{
	I2S_InitTypeDef I2S_InitStructure;

	RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);				// 配置IIS PLL时钟
	RCC_PLLI2SCmd(ENABLE);											// 使能PLL
	while( RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == 0 );	// 等待配置完成
	
	RCC_APB1PeriphClockCmd(I2S_SPI_CLK, ENABLE);					// 初始化IIS时钟

	SPI_I2S_DeInit(I2S_SPI_BASE);
	
	I2S_InitStructure.I2S_AudioFreq 	= AudioFreq;			// 设置音频采样频率 
	I2S_InitStructure.I2S_Standard 		= Standard;	//	I2S Philips 标准
	I2S_InitStructure.I2S_DataFormat 	= DataFormat;		// 数据长度16位
	I2S_InitStructure.I2S_CPOL 			= I2S_CPOL_Low;				// 空闲状态电平位低
	I2S_InitStructure.I2S_Mode 			= I2S_Mode_MasterTx;			// 主机发送
	I2S_InitStructure.I2S_MCLKOutput 	= I2S_MCLKOutput_Enable;	// 主时钟输出
	I2S_Init(I2S_SPI_BASE, &I2S_InitStructure);
	
	I2S_Cmd(I2S_SPI_BASE, ENABLE);	// 使能IIS
}
/**
 *@brief:      mcu_i2s_dam_init
 *@details:    初始化I2S使用的DMA通道，双缓冲模式
 *@param[in]   u16 *buffer0  
               u16 *buffer1  
               u32 len       
 *@param[out]  无
 *@retval:     
 */
void mcu_i2s_dma_init(u16 *buffer,u32 len)
{  
	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_str;
	u16 *buffer0;
	u16 *buffer1;

	buffer0 = buffer;
	buffer1 = buffer+len/2;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);	// 使IIS DMA时钟 
	DMA_DeInit(I2S_DMA_STREAM);	//	恢复初始DMA配置

	DMA_str.DMA_Channel 				= DMA_Channel_0;  					//	IIS DMA通道 
	DMA_str.DMA_PeripheralBaseAddr 	= (u32)&I2S_SPI_BASE->DR;							//	外设地址
	DMA_str.DMA_Memory0BaseAddr 		= (u32)buffer0;							//	缓冲区0
	DMA_str.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;			//	存储器到外设模式
	DMA_str.DMA_BufferSize 			= len/2;										//	数据长度 
	DMA_str.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;			//	外设非增量模式

	if(len == 1)
		DMA_str.DMA_MemoryInc 			= DMA_MemoryInc_Disable;	
	else
		DMA_str.DMA_MemoryInc 			= DMA_MemoryInc_Enable;					//	存储器增量模式

	DMA_str.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_HalfWord;	//	外设数据长度16位
	DMA_str.DMA_MemoryDataSize 		= DMA_MemoryDataSize_HalfWord;		//	存储器数据长度16位 
	DMA_str.DMA_Mode 					= DMA_Mode_Circular;						//	循环模式 
	DMA_str.DMA_Priority 				= DMA_Priority_High;						//	高优先级
	DMA_str.DMA_FIFOMode 				= DMA_FIFOMode_Disable; 				//	不使用FIFO      
	DMA_str.DMA_FIFOThreshold 		= DMA_FIFOThreshold_1QuarterFull;	//	FIFO阈值 
	DMA_str.DMA_MemoryBurst 			= DMA_MemoryBurst_Single;				//	外设突发单次传输
	DMA_str.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;			//	存储器突发单次传输
	DMA_Init(I2S_DMA_STREAM, &DMA_str);										//	初始化DMA
			
	DMA_DoubleBufferModeConfig(I2S_DMA_STREAM,(uint32_t)buffer0, DMA_Memory_0);	//	配置缓冲区1
	
	DMA_DoubleBufferModeConfig(I2S_DMA_STREAM,(uint32_t)buffer1, DMA_Memory_1);	//	配置缓冲区1

	DMA_DoubleBufferModeCmd(I2S_DMA_STREAM,ENABLE);										//	开启双缓冲模式
	DMA_ITConfig(I2S_DMA_STREAM,DMA_IT_TC,ENABLE);	//开启传输完成中断

	SPI_I2S_DMACmd(I2S_SPI_BASE, SPI_I2S_DMAReq_Tx,ENABLE);		//IIS TX DMA使能.

	NVIC_InitStructure.NVIC_IRQChannel = I2S_DMA_IRQ;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 *@brief:      mcu_i2s_dma_start
 *@details:    开始DMA传输
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2s_dma_start(void)
{   	  
	DMA_Cmd(I2S_DMA_STREAM,ENABLE);	// 开启DMA TX传输
}
/**
 *@brief:      mcu_i2s_dma_stop
 *@details:       停止DMA传输
 *@param[in]  void  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2s_dma_stop(void)
{   	 
	DMA_Cmd(I2S_DMA_STREAM,DISABLE);	//关闭 DMA TX传输
}

/**
 *@brief:      mcu_i2s_dma_process
 *@details:    I2S使用的DMA中断处理函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:     

 位 19 CT：当前目标（仅在双缓冲区模式下） (Current target (only in double buffer mode))
此位由硬件置 1 和清零，也可由软件写入。
0：当前目标存储器为存储器 0（使用 DMA_SxM0AR 指针寻址）
1：当前目标存储器为存储器 1（使用 DMA_SxM1AR 指针寻址）
只有 EN 为“0”时，此位才可以写入，以指示第一次传输的目标存储区。在使能数据流
后，此位相当于一个状态标志，用于指示作为当前目标的存储区。
 */
void mcu_i2s_dma_process(void)
{
	if(I2S_DMA_STREAM->CR&(1<<19)) {
		/*当前目标存储器为1，我们就设置空闲BUF为0*/
		audio_pipe_callback(0);
	} else {
		audio_pipe_callback(1);
	}
}

/*

	I2SEXT
	
	扩展 I2S (I2Sx_ext) 只能用于全双工模式。 I2Sx_ext 始终在从模式下工作。
*/
extern s32 fun_rec_set_free_buf(u8 index);


/**
 *@brief:      mcu_i2sext_config
 *@details:    配置I2SEXT（应该跟I2S一样吧？合并？）
 *@param[in]   u32 AudioFreq   
               u16 Standard    
               u16 DataFormat  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2sext_config(u32 AudioFreq, u16 Standard,u16 DataFormat)
{  
	I2S_InitTypeDef I2Sext_InitStructure;
	
	I2Sext_InitStructure.I2S_Mode = I2S_Mode_MasterTx;//I2S_FullDuplexConfig会进行转换
	I2Sext_InitStructure.I2S_Standard=Standard;//IIS标准
	I2Sext_InitStructure.I2S_DataFormat=DataFormat;//IIS数据长度
	I2Sext_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Enable;//主时钟输出,i2sext无效
	I2Sext_InitStructure.I2S_AudioFreq=AudioFreq;//IIS频率设置
	I2Sext_InitStructure.I2S_CPOL=I2S_CPOL_Low;//空闲状态时钟电平
	
	I2S_FullDuplexConfig(I2S_EXT_TYPE, &I2Sext_InitStructure);//初始化I2S2ext配置
	
	I2S_Cmd(I2S_EXT_TYPE, ENABLE);		//I2S2ext I2S EN使能.
}
/**
 *@brief:      mcu_i2sext_dma_init
 *@details:    设置I2S EXT DMA缓冲
 *@param[in]   u16* buf0  
               u16 *buf1  
               u32 len    
 *@param[out]  无
 *@retval:     
 */
void mcu_i2sext_dma_init(u16* buf0, u16 *buf1, u32 len)
{	

	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能 

	DMA_DeInit(I2S_EXT_DMA);
	while (DMA_GetCmdStatus(I2S_EXT_DMA) != DISABLE){}//等待DMA1_Stream3可配置 

	DMA_ClearITPendingBit(I2S_EXT_DMA,DMA_IT_FEIF3|DMA_IT_DMEIF3|DMA_IT_TEIF3|DMA_IT_HTIF3|DMA_IT_TCIF3);//清空DMA1_Stream3上所有中断标志

	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&I2S_EXT_TYPE->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = len;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度：16位 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //不使用FIFO模式		  
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//外设突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//存储器突发单次传输
	DMA_Init(I2S_EXT_DMA, &DMA_InitStructure);//初始化DMA Stream

	DMA_DoubleBufferModeConfig(I2S_EXT_DMA, (u32)buf0, DMA_Memory_0);//双缓冲模式配置
	DMA_DoubleBufferModeConfig(I2S_EXT_DMA, (u32)buf1, DMA_Memory_1);//双缓冲模式配置
	
	DMA_DoubleBufferModeCmd(I2S_EXT_DMA,ENABLE);//双缓冲模式开启

	DMA_ITConfig(I2S_EXT_DMA,DMA_IT_TC,ENABLE);//开启传输完成中断

	SPI_I2S_DMACmd(I2S_EXT_TYPE, SPI_I2S_DMAReq_Rx, ENABLE);//I2S2ext RX DMA请求使能.
	
	NVIC_InitStructure.NVIC_IRQChannel = I2S_EXT_DMA_IRQ; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0x00;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置

} 

/**
 *@brief:      mcu_i2sext_dma_start
 *@details:    开始I2SEXT DMA传输
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2sext_dma_start(void)
{   	  
	DMA_Cmd(I2S_EXT_DMA,ENABLE);	// 开启DMA TX传输
}
/**
 *@brief:      mcu_i2sext_dma_stop
 *@details:    停止DMA传输
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2sext_dma_stop(void)
{   	 
	DMA_Cmd(I2S_EXT_DMA,DISABLE);	//关闭 DMA TX传输
}
/**
 *@brief:      mcu_i2sext_dma_process
 *@details:    DMA中断
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2sext_dma_process(void)
{
	if(I2S_EXT_DMA->CR&(1<<19)) {
		//fun_rec_set_free_buf(0);
	} else {
		//fun_rec_set_free_buf(1);
	}

}



s32 mcu_i2s_dataformat(u32 Freq, u8 Standard, short BitsPerSample)
{
	u16 standard;
	u16 dataformat;
	

	if (Standard == AUDIO_I2S_LSB) {
		standard = 	I2S_Standard_LSB;
	} else if(Standard == AUDIO_I2S_MSB) {
		standard = 	I2S_Standard_MSB;
	} else if(Standard == AUDIO_I2S_Phillips) {
		standard = 	I2S_Standard_Phillips;
	} else if(Standard == AUDIO_I2S_PCM) {
		standard = 	I2S_Standard_PCMLong;
	} else {
		standard = 	I2S_Standard_Phillips;	
	}

	if (BitsPerSample == 16) {
		dataformat = 	I2S_DataFormat_16b;
	} else if(BitsPerSample == 20) {
		dataformat = 	I2S_DataFormat_16bextended;
	} else if(BitsPerSample == 24) {
		dataformat = 	I2S_DataFormat_24b;
	} else if(BitsPerSample == 32) {
		dataformat = 	I2S_DataFormat_32b;
	}
	
	mcu_i2s_config(Freq, standard, dataformat);
	mcu_i2sext_config(Freq, standard, dataformat);

	return 0;
}



