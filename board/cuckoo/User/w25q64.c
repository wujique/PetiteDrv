
/*
	参考 STM32Cube_FW_H7_V1.9.0\Projects\STM32H7B3I-DK\Examples\OSPI\OSPI_NOR_MemoryMapped_DTR\Src
	例子用的Flash不一样
	*/

#include "stm32h7xx_hal.h"
#include <string.h>
#include "mcu.h"
#include "w25q128fv.h"

extern OSPI_HandleTypeDef hospi1;
#define QSPIHandle hospi1

/*
	不同的平台，不同的接口，W25QXX操作方式不一样
	比如：
		STM32F103的SPI模式
		STM32H750的QSPI模式
		RT1052的QSPI模式
		STM32H7B0的OCTOSPI模式


H7B0的OCTOSPI，会在芯片中配置一套所谓的SPI通信通用配置。
RT1052，则是可以通过LUT表配置命令序列。

	
	使用说明：
	1 QSPI模式和QPI模式不一样。
		SPI/DSPI/QSPI都是spi，都是串行。
		QPI则是并行。
		
	2 FLASH芯片上电默认是SPI模式。注意，是FLASH芯片断电重新上电。
	   
	3 要切换到QPI模式，先要把QE位置位为1，这个位在状态寄存器2.
	  然后发送0X38命令过去切换为QPI模式。
	  
	4 在QPI模式，发送0XFF指令，可以让芯片退出QPI模式。

	注意问题：
		在SPI模式下，指令用的是1LINE模式。QPI模式则用4LINE模式。
	
	*/
/*SPI FLASH 信息*/
typedef struct
{
	char *name;
	u32 JID;
	u32 MID;
	/*容量，块数，块大小等信息*/
	u32 pagesize;
	u32 sectornum;//总块数
	u32 sectorsize;//块大小
	u32 structure;//总容量
	
}_strSpiFlash;

/*
	** W25Q64JV, 在文档中找不到QPI模式的说明，是否不支持？**
	
	erase: [4K 45ms], [32K 120ms] ,[64K 150ms], [chip 20S~100S]
	*/
_strSpiFlash OspiFlashPra={
	.name = "W25Q64JVSI",
	.JID  = 0Xef4017,
	.MID  = 0Xef16,
	.pagesize = 256,
	.sectornum = 2048,
	.sectorsize = 4096,
	.structure = 0x800000, //8M
};

int W25Qxx_read_id(void)	
{
	OSPI_RegularCmdTypeDef	s_command;		// OSPI传输配置

	uint8_t Buff[4];	// 存储OSPI读到的数据
	uint32_t	ID;				// 器件的ID
	
	
	/*--------------------------------------读ID测试--------------------------------------------------*/ 
    /* Read Manufacture/Device ID */
	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= READ_ID_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_1_LINE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_1_LINE; 
	s_command.NbData 			= 2;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;
	
    if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
    }
	
    if (HAL_OSPI_Receive(&QSPIHandle, Buff, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
    }
	
    uart_printf("SPI  I/0 Read Device ID : 0x%2X 0x%2X\r\n", Buff[0], Buff[1]);
	

    /* Read Manufacture/Device ID Dual I/O*/
	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= DUAL_READ_ID_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_2_LINES;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_2_LINES;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_2_LINES; 
	s_command.NbData 			= 4;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;
    if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
    }
	
    if (HAL_OSPI_Receive(&QSPIHandle, Buff, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
    }
	
    uart_printf("Dual I/O Read Device ID : 0x%2X 0x%2X\r\n",Buff[0], Buff[1]);

	
    /* Read Manufacture/Device ID Quad I/O*/
	/* 通用配置 */
	s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= QUAD_READ_ID_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_4_LINES;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_4_LINES;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_4_LINES; 
	s_command.NbData 			= 2;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 4;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;
    if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
    }
	
    if (HAL_OSPI_Receive(&QSPIHandle, Buff, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
    }
	
    uart_printf("Quad I/O Read Device ID : 0x%2X 0x%2X\r\n",Buff[0],Buff[1]);

 	/*---------------------------------------------------------------------------------*/
	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= READ_JEDEC_ID_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_NONE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_1_LINE; 
	s_command.NbData 			= 3;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;

	/* HAL_OSPI_TIMEOUT_DEFAULT_VALUE 与 HAL_QPSI_TIMEOUT_DEFAULT_VALUE 有和不同？*/
    if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
    }
	
    if (HAL_OSPI_Receive(&QSPIHandle, Buff, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
    }
	
	ID = (Buff[0] << 16) | (Buff[1] << 8 ) | Buff[2];
    uart_printf("Read JEDEC ID : 0x%X\r\n", ID);	

	return ID; // 返回ID
}

/**
 *@brief:      w25qxx_waitwriteend
 *@details:    等待flash busy位清，通常在进行写操作后调用
 *@param[in]   uint32_t Timeout  超时时间，单位ms
 *@param[out]  无
 *@retval:     
 */
static int w25qxx_waitwriteend(uint32_t Timeout)
{
	OSPI_RegularCmdTypeDef	s_command;		
    OSPI_AutoPollingTypeDef sConfig; 	
	char reg[4];
	
	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= READ_STATUS_REG1_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_NONE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_1_LINE; 
	s_command.NbData 			= 1;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;

	#if 0
    if (HAL_OSPI_Command(&hospi1, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;	
    }
	
	/* 自动查询 比较标志位 */
	sConfig.Match		  = 0x00;	
	sConfig.Mask		  = W25Q128FV_FSR_BUSY;//busy 标志
	sConfig.MatchMode	  = HAL_OSPI_MATCH_MODE_AND;//与比较									
	sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE; 
	sConfig.Interval	  = 0x10;	//两次读之间间隔n clk时钟
	
	/* 超时时间，单位和systick相关，本处为ms*/
	if (HAL_OSPI_AutoPolling(&hospi1, &sConfig, Timeout) != HAL_OK) {
		return -4; 
	}
	#else
	do {
		if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
			Error_Handler();
		}

		if (HAL_OSPI_Receive(&QSPIHandle, reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)	{
			Error_Handler();
		}
	} while((reg[0] & W25Q128FV_FSR_BUSY) == W25Q128FV_FSR_BUSY);
	#endif
	return 0;			
}
/**
 *@brief:      w25qxx_writeen
 *@details:    FLASH 写使能:设置FLASH内的寄存器，
               允许写/擦除，每次写/擦除之前都需要发送
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
int w25qxx_writeen(void)
{
   OSPI_RegularCmdTypeDef  s_command;
   OSPI_AutoPollingTypeDef sConfig;

   /* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= WRITE_ENABLE_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_NONE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_NONE; 
	s_command.NbData 			= 0;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;
   if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
      return -3;	
   }

   /* 判断写标志是否成功 */
	#if 0
	/* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction    = OCTAL_READ_STATUS_REG_CMD;
  sCommand.Address        = 0x0;
  sCommand.AddressMode    = HAL_OSPI_ADDRESS_8_LINES;
  sCommand.AddressSize    = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_ENABLE;
  sCommand.DataMode       = HAL_OSPI_DATA_8_LINES;
  sCommand.DataDtrMode    = HAL_OSPI_DATA_DTR_ENABLE;
  sCommand.NbData         = 2;
  sCommand.DummyCycles    = DUMMY_CLOCK_CYCLES_READ_OCTAL;
  sCommand.DQSMode        = HAL_OSPI_DQS_ENABLE;

   do
  {
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_OSPI_Receive(hospi, reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
  } while((reg[0] & WRITE_ENABLE_MASK_VALUE) != WRITE_ENABLE_MATCH_VALUE);
   #endif
   
	return 0;  // 通信正常结束
}

/*
	复位Flash
	*/
int w25qxx_reset(void)
{
	OSPI_RegularCmdTypeDef s_command;

	/* Initialize the reset enable command */
	/* 通用配置 */
	s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= RESET_ENABLE_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
		/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_NONE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_NONE; 
	s_command.NbData 			= 0;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;	
	/* Send the command */
	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;
	}

	/* Send the reset memory command */
	s_command.Instruction = RESET_MEMORY_CMD;
	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;
	}

	w25qxx_waitwriteend(1000);

	return 0;
}


int w25qxx_status_init(void)
{ 
	OSPI_RegularCmdTypeDef s_command;
	uint8_t value = W25Q128FV_FSR_QE;
	
  	/* QSPI memory reset */
 	w25qxx_reset();
	
	/* Enable write operations */
	w25qxx_writeen();
	
	/* Set status register for Quad Enable,the Quad IO2 and IO3 pins are enable */
	/* 通用配置 */
	s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= WRITE_STATUS_REG2_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_NONE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_1_LINE; 
	s_command.NbData 			= 1;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;	
	/* Configure the command */
	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;
	}
	/* Transmit the data */
	if (HAL_OSPI_Transmit(&QSPIHandle, &value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;
	}

	/* automatic polling mode to wait for memory ready */  
	w25qxx_waitwriteend(1000);
	
  	return 0;
}
/*
	
	*/
int W25Qxx_init(void)
{
	u32 flash_jid;
	
	w25qxx_status_init();
	
	flash_jid = W25Qxx_read_id();
	if ( flash_jid != OspiFlashPra.JID) {
		uart_printf(" warr: flash id err!\r\n");
		return -1;
	} else {
		uart_printf("name  :%s\r\n", OspiFlashPra.name);
		uart_printf("sector:%d Sec\r\n", OspiFlashPra.sectornum);
		uart_printf("size  :%d Byte\r\n", OspiFlashPra.sectorsize);
		uart_printf("total :%d Byte\r\n", OspiFlashPra.structure);

	}

	//w25qxx_test();
	return 0;
}
/*
	Flash 一般特点：
	读，任意读。
	擦，最少sector（通常4K）擦
	写，一页一页写，通常时256字节

	写之前要擦，使用读-擦-回写保证数据正确，这流程放到flash粘合层，
	在芯片驱动中不做。
	*/
/**
 *@brief:	   w25qxx_erase_chip
 *@details:     整片擦除，时间很长，20S级别，具体请查看Flash规格书
 *@param[in]   u32 addr  地址，不使用；方便接口兼容
 *@param[out]  无
 *@retval:	   
 */
int w25qxx_erase_chip(u32 addr)
{
	OSPI_RegularCmdTypeDef	s_command;		
    OSPI_AutoPollingTypeDef sConfig; 	
	int ret;
	
	w25qxx_writeen();

	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= CHIP_ERASE_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_NONE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_NONE; 
	s_command.NbData 			= 0;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;
	
	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;
	}	
	/* 等待busy，也就是擦完成 */
	ret = w25qxx_waitwriteend(100000);
	return ret;		
}


/**
 *@brief:      w25qxx_erase_sector
 *@details:    擦除一个sector
 *@param[in]   u32 addr  地址，包含这个地址的sector将被擦除
 *@param[out]  无
 *@retval:     擦除时间 W25Q64 典型值为 45ms，最大值为400ms
 */
int w25qxx_erase_sector(u32 addr)
{
	OSPI_RegularCmdTypeDef  s_command;		// OSPI传输配置
	int ret;
	
	w25qxx_writeen();
	
	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= SECTOR_ERASE_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= addr;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_1_LINE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_NONE; 
	s_command.NbData 			= 0;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;
	
	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;
	}	
	/* 等待busy，也就是擦完成 1秒*/
	ret = w25qxx_waitwriteend(1000);
	return ret;	
}

/**
 *@brief:      w25qxx_read_any
 *@details:    读随意长度FLASH数据
 *@param[in]   u32 addr  
               u8 *dst  
               u32 len      
 *@param[out]  无
 *@retval:     
 */
int w25qxx_read_any(u32 addr, u32 rlen, u8 *dst)
{
   	OSPI_RegularCmdTypeDef  s_command;

	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= QUAD_INOUT_FAST_READ_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= addr;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_4_LINES;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0xf5;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_4_LINES;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_4_LINES; 
	s_command.NbData 			= rlen;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 4;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD; 
	
	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;		
	}	

	if (HAL_OSPI_Receive(&QSPIHandle, dst, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;	
	}
	
	return rlen;
}

/**
 *@brief:      w25qxx_page_prog
 *@details:    写数据到FLASH
 *@param[in]   u8* pbuffer     
               u32 addr  
               u16 wlen        
 *@param[out]  无
 *@retval:     页编程，也就是写数据，不能跨page，调用本函数者保证不跨page
 */
int w25qxx_page_prog(u32 addr, u16 wlen, u8* src)
{
   	OSPI_RegularCmdTypeDef  s_command;

	w25qxx_writeen();

	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= QUAD_INPUT_PAGE_PROG_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= addr;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_1_LINE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_4_LINES; 
	s_command.NbData 			= wlen;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;

	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;	
	}	

	if (HAL_OSPI_Transmit(&QSPIHandle, src, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;	
	}
	
	/* W25Q64 编程时间 0.4ms，不大于1ms */
	w25qxx_waitwriteend(10);
	
	return wlen; // 写数据成功
}

/**
 *@brief:      w25qxx_write_any
 *@details:    写数据到FLASH
 *@param[in]   u8* pbuffer     
               u32 addr  
               u16 wlen        
 *@param[out]  无
 *@retval:     连续写，可跨页，但是，写之前请先擦除
 */
int w25qxx_write_any(u32 addr, u16 wlen, u8* src)
{
	s32 len;
	u8 command[4];

	while (wlen) {	  

		/* 1page 正好是256 字节，求出当前地址指向的页能写入的数据量 */
		len = 256 - (addr & 0xff);
		
		if (len < wlen) {
			
		} else {
			len = wlen;
		}
		
		w25qxx_page_prog(addr, len, src);

		/* 调整相关索引 */
		wlen -= len;
		src += len;
		addr += len;	   
	}
		
	return 0;
}

#if 0
int w25qxx_QPI_read_any(u32 addr, u32 rlen, u8 *dst)
{
	OSPI_RegularCmdTypeDef s_command;
  	OSPI_MemoryMappedTypeDef sMemMappedCfg;
	
	/* Memory-mapped mode configuration ------------------------------- */

	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= FAST_READ_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_4_LINES;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= addr;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_4_LINES;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_4_LINES; 
	s_command.NbData 			= rlen;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 2;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD; 
	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
	  Error_Handler();
	}

	if (HAL_OSPI_Receive(&QSPIHandle, dst, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;	
	}

}


int w25qxx_qpi_mode_enter(void)
{
	OSPI_RegularCmdTypeDef s_command;
		
	/* Configure the Enter QPI Mode command */

	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= ENTER_QPI_MODE_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_NONE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_NONE; 
	s_command.NbData 			= 0;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;

	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;	
	}	
}

int w25qxx_qpi_mode_exit(void)
{
	OSPI_RegularCmdTypeDef s_command;

	/* Configure the EXIT QPI Mode command */

	s_command.OperationType 	= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= EXIT_QPI_MODE_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_4_LINES;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_NONE;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_NONE; 
	s_command.NbData			= 0;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 0;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD;

	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return -1;	
	}

}
#endif


/* https://www.jianshu.com/p/77f7bd0817e9 */
									
int w25qxx_map(void)
{
	OSPI_RegularCmdTypeDef s_command;
  	OSPI_MemoryMappedTypeDef sMemMappedCfg;
	
	/* Memory-mapped mode configuration ------------------------------- */

	/* 通用配置 */
    s_command.OperationType		= HAL_OSPI_OPTYPE_COMMON_CFG;
	s_command.FlashId			= HAL_OSPI_FLASH_ID_1; 
	/* 命令 */
	s_command.Instruction		= QUAD_INOUT_FAST_READ_CMD;
	s_command.InstructionMode	= HAL_OSPI_INSTRUCTION_1_LINE;		 
	s_command.InstructionSize	= HAL_OSPI_INSTRUCTION_8_BITS;	
	s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  	/* 地址配置*/
	s_command.Address			= 0;
	s_command.AddressMode		= HAL_OSPI_ADDRESS_4_LINES;			 
	s_command.AddressSize		= HAL_OSPI_ADDRESS_24_BITS; 
	s_command.AddressDtrMode 	= HAL_OSPI_ADDRESS_DTR_DISABLE;
	/* alternate*/
	s_command.AlternateBytes = 0xf5;
	s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_4_LINES;
	s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	/* 数据 传输*/
	s_command.DataMode			= HAL_OSPI_DATA_4_LINES; 
	s_command.NbData 			= 0;
	s_command.DataDtrMode		= HAL_OSPI_DATA_DTR_DISABLE;						
	s_command.DummyCycles		= 4;
	/* 其他 */
	s_command.DQSMode			= HAL_OSPI_DQS_DISABLE; 			
	s_command.SIOOMode			= HAL_OSPI_SIOO_INST_EVERY_CMD; 
	if (HAL_OSPI_Command(&QSPIHandle, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
	  Error_Handler();
	}

	sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;

	if (HAL_OSPI_MemoryMapped(&QSPIHandle, &sMemMappedCfg) != HAL_OK) {
	  Error_Handler();
	}

	//test_iap();
	return 0;
}


#if 1
char w25qxx_testbuf_r[4096];
char w25qxx_testbuf_w[4096];
#define W25QXX_TEST_ADDR (4096*1024)
__IO uint8_t *mem_addr;
extern const u8 test_qspi_tab[33];

void w25qxx_test(void)
{
	int i;
	u8 cnt=0;
	uint32_t tickstart, tmp;
	
	uart_printf("w25qxx_test...\r\n");

	//while(1) 
	{
		cnt++;
		for (i = 0;i<4096; i++) {
			w25qxx_testbuf_w[i] = cnt;	
		}
		tickstart = HAL_GetTick();
		w25qxx_read_any(W25QXX_TEST_ADDR, 4096, w25qxx_testbuf_r);
		//uart_printf("read...\r\n");
		//PrintFormat(w25qxx_testbuf_r, 128);
		
		//uart_printf("erase_sector...\r\n");
		w25qxx_erase_sector(W25QXX_TEST_ADDR);

		//uart_printf("read...\r\n");
		//w25qxx_read_any(W25QXX_TEST_ADDR, 4096, w25qxx_testbuf_r);
		//PrintFormat(w25qxx_testbuf_r, 128);
		
		//uart_printf("write_any...\r\n");
		w25qxx_write_any(W25QXX_TEST_ADDR+16, 4096-32, &w25qxx_testbuf_w[16]);

		//uart_printf("read...\r\n");
		w25qxx_read_any(W25QXX_TEST_ADDR, 4096, w25qxx_testbuf_r);
		//PrintFormat(w25qxx_testbuf_r, 128);
		tmp = HAL_GetTick();
		
		uart_printf("finish: %d\r\n", tmp-tickstart);

		#if 0
		uart_printf("w25qxx_map...\r\n");
		w25qxx_map();
		uart_printf("dr read...\r\n");
		mem_addr = (__IO uint8_t *)(0x90000000);
	    PrintFormat(mem_addr, 256);

		PrintFormat(test_qspi_tab, 32);

		uart_printf("run ex flash code...\r\n");
		u8 cnt = 0;
		while(1) {
			cnt = test_qpi_run_fun(cnt);
			uart_printf("%d ", cnt);
			if(cnt >= 128) break;
		}
		uart_printf("----run code in qspi flash ok\r\n");

		HAL_OSPI_DeInit(&hospi1);
		MX_OCTOSPI1_Init();
		#endif
	}
}


void w25qxx_test_2(void)
{
	int i;
	u8 cnt=0;
	//uart_printf("w25qxx_test...\r\n");

	HAL_OSPI_DeInit(&hospi1);
	MX_OCTOSPI1_Init();
	

	cnt++;
	for (i = 0;i<4096; i++) {
		w25qxx_testbuf_w[i] = cnt;	
	}

	w25qxx_read_any(W25QXX_TEST_ADDR, 4096, w25qxx_testbuf_r);
	//uart_printf("read...\r\n");
	//PrintFormat(w25qxx_testbuf_r, 128);
	
	//uart_printf("erase_sector...\r\n");
	w25qxx_erase_sector(W25QXX_TEST_ADDR);

	//uart_printf("read...\r\n");
	//w25qxx_read_any(W25QXX_TEST_ADDR, 4096, w25qxx_testbuf_r);
	//PrintFormat(w25qxx_testbuf_r, 128);
	
	//uart_printf("write_any...\r\n");
	w25qxx_write_any(W25QXX_TEST_ADDR+16, 4096-32, &w25qxx_testbuf_w[16]);

	//uart_printf("read...\r\n");
	w25qxx_read_any(W25QXX_TEST_ADDR, 4096, w25qxx_testbuf_r);
	//PrintFormat(w25qxx_testbuf_r, 128);
	
	//uart_printf("finish\r\n");

	w25qxx_map();

}


#endif

