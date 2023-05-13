/*




*/
#include "main.h"

#include "sdmmc_sd.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "board_sysconf.h"


FATFS 	SD_FatFs; 
FRESULT 	MyFile_Res;    


/* sdcard init and mount fatfs */
void sdcard_fatfs_init(void)	
{
	BYTE work[FF_MAX_SS]; 
	
	FATFS_LinkDriver(&SD_Driver, SDPath);
	uart_printf("\r\nmount...");
	MyFile_Res = f_mount(&SD_FatFs, SDPath, 1);	
	
	if (MyFile_Res == FR_OK) {
		uart_printf("ok\r\n");
	} else {
		uart_printf("format...\r\n");
		
		MyFile_Res = f_mkfs(SDPath,FM_FAT32,0,work,sizeof work);
		
		if (MyFile_Res == FR_OK)
			uart_printf("ok！\r\n");
		else
			uart_printf("err！\r\n");
	}
}

void FatFs_GetVolume(void)	
{
	FATFS *fs;		
	uint32_t SD_CardCapacity = 0;		//SD卡的总容量
	uint32_t SD_FreeCapacity = 0;		//SD卡空闲容量
	DWORD fre_clust, fre_sect, tot_sect; 	//空闲簇，空闲扇区数，总扇区数

	f_getfree(SDPath, &fre_clust, &fs);			//获取SD卡剩余的簇

	tot_sect = (fs->n_fatent-2) * fs->csize;	//总扇区数量 = 总的簇 * 每个簇包含的扇区数
	fre_sect = fre_clust * fs->csize;			//计算剩余的可用扇区数	   

	SD_CardCapacity = tot_sect / 2048 ;	// SD卡总容量 = 总扇区数 * 512( 每扇区的字节数 ) / 1048576(换算成MB)
	SD_FreeCapacity = fre_sect / 2048 ;	//计算剩余的容量，单位为M
	uart_printf("-------------------pra-----------------\r\n");		
	uart_printf("SD %dMB\r\n",SD_CardCapacity);	
	uart_printf("SD %dMB\r\n",SD_FreeCapacity);
}

BYTE 	MyFile_ReadBuffer[32];	
BYTE	MyFile_WriteBuffer[32] = "STM32H7B0 SD test fatfs";	
FIL MyFile; 		

/*  测试读写文件 */
#define TEST_FILE_NAME "0:/FatFs_Test.txt"
uint8_t  FatFs_FileTest(void)	//文件创建和写入测试
{
	uint8_t i = 0;
	uint16_t rlen = 0;	

	UINT 	MyFile_Num;		//	数据长度

	uart_printf("-------------FatFs open & write---------------\r\n");
	
	MyFile_Res = f_open(&MyFile,TEST_FILE_NAME, FA_CREATE_ALWAYS | FA_WRITE);
	if(MyFile_Res == FR_OK) {
		uart_printf("ok,write...\r\n");
		
		MyFile_Res = f_write(&MyFile, MyFile_WriteBuffer,strlen(MyFile_WriteBuffer),&MyFile_Num);
		if (MyFile_Res == FR_OK) {
			uart_printf("ok \r\n");
			uart_printf("%s\r\n",MyFile_WriteBuffer);
		} else {
			uart_printf("err!\r\n");
			f_close(&MyFile);	  //关闭文件	
			return ERROR;			
		}
		f_close(&MyFile);	  //关闭文件			
	} else {
		uart_printf("err!\r\n");
		f_close(&MyFile);	  //关闭文件	
		return ERROR;		
	}
	
	uart_printf("-------------FatFs read---------------\r\n");	
	
	rlen = strlen(MyFile_WriteBuffer);							
	MyFile_Res = f_open(&MyFile, TEST_FILE_NAME, FA_OPEN_EXISTING | FA_READ);	
	MyFile_Res = f_read(&MyFile,MyFile_ReadBuffer,rlen,&MyFile_Num);	
	if(MyFile_Res == FR_OK) {
		uart_printf("ok, verify...\r\n");
		PrintFormat(MyFile_ReadBuffer, rlen);
		for(i=0;i<rlen;i++) {
			if(MyFile_WriteBuffer[i] != MyFile_ReadBuffer[i]) {
				uart_printf("err!\r\n");
				f_close(&MyFile);	  
				return ERROR;
			}
		}
		uart_printf("ok\r\n");
		uart_printf("%s\r\n", MyFile_ReadBuffer);
	} else {
		uart_printf("err!\r\n");
		f_close(&MyFile);	  //关闭文件	
		return ERROR;		
	}	
	
	f_close(&MyFile);	  //关闭文件	

	uart_printf("sdmmc and fatfs test finish!\r\n\r\n\r\n\r\n");
	return SUCCESS;
}

/*
	初始化文件SD卡，并挂载文件系统
	*/
void sd_fatfs_init(void)
{
	sdcard_fatfs_init();

	FatFs_GetVolume();	
	FatFs_FileTest();
	
}

