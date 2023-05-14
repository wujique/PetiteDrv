/*




*/
#include "main.h"
#include "board_sysconf.h"


#include "ff.h"
#include "ff_gen_drv.h"

extern DSTATUS SD_disk_initialize (
                         BYTE drv		/* Physical drive number (0) */
                           );
extern DSTATUS SD_disk_status (
                     BYTE drv		/* Physical drive number (0) */
                       );
extern DRESULT SD_disk_read (
                   BYTE pdrv,			/* Physical drive number (0) */
                   BYTE *buff,			/* Pointer to the data buffer to store read data */
                   DWORD sector,		/* Start sector number (LBA) */
                   UINT count			/* Sector count (1..255) */
                     );
extern DRESULT SD_disk_write (
                    BYTE pdrv,			/* Physical drive number (0) */
                    const BYTE *buff,	/* Pointer to the data to be written */
                    DWORD sector,		/* Start sector number (LBA) */
                    UINT count			/* Sector count (1..255) */
                      );

extern DRESULT SD_disk_ioctl (
                    BYTE drv,		/* Physical drive number (0) */
                    BYTE ctrl,		/* Control code */
                    void *buff		/* Buffer to send/receive control data */
                      );

const Diskio_drvTypeDef  SD_Driver =
{
  SD_disk_initialize,
  SD_disk_status,
  SD_disk_read,
#if  _USE_WRITE == 1
  SD_disk_write,
#endif /* _USE_WRITE == 1 */

#if  _USE_IOCTL == 1
  SD_disk_ioctl,
#endif /* _USE_IOCTL == 1 */
};


FATFS 	SD_FatFs; 
FRESULT 	MyFile_Res;    



/* sdcard init and mount fatfs */
void sdcard_fatfs_init(void)	
{
	BYTE work[FF_MAX_SS]; 
	
	FATFS_LinkDriver(&SD_Driver, SYS_FS_FATFS_SD);
	uart_printf("\r\nmount...");
	MyFile_Res = f_mount(&SD_FatFs, SYS_FS_FATFS_SD, 1);	
	
	if (MyFile_Res == FR_OK) {
		uart_printf("ok\r\n");
	} else {
		uart_printf("format...\r\n");
		
		MyFile_Res = f_mkfs(SYS_FS_FATFS_SD, FM_FAT32, 0, work,sizeof work);
		
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

	f_getfree(SYS_FS_FATFS_SD, &fre_clust,&fs);			//获取SD卡剩余的簇

	tot_sect = (fs->n_fatent-2) * fs->csize;	//总扇区数量 = 总的簇 * 每个簇包含的扇区数
	fre_sect = fre_clust * fs->csize;			//计算剩余的可用扇区数	   

	SD_CardCapacity = tot_sect / 2048 ;	// SD卡总容量 = 总扇区数 * 512( 每扇区的字节数 ) / 1048576(换算成MB)
	SD_FreeCapacity = fre_sect / 2048 ;	//计算剩余的容量，单位为M
	uart_printf("-------------------pra-----------------\r\n");		
	uart_printf("SD %dMB\r\n",SD_CardCapacity);	
	uart_printf("SD %dMB\r\n",SD_FreeCapacity);
}

BYTE 	MyFile_ReadBuffer[32];	//要读出的数据
BYTE	MyFile_WriteBuffer[32] = "STM32H7B0 SD test fatfs";	//要写入的数据
FIL MyFile; 		// 文件对象
#define FATFS_TEST_FILE "1:FatFs_Test.txt"


int  FatFs_FileTest(void)	
{
	uint8_t i = 0;
	uint16_t rlen = 0;	

	UINT 	MyFile_Num;		//	数据长度

	uart_printf("-------------FatFs open & write---------------\r\n");
	
	MyFile_Res = f_open(&MyFile, FATFS_TEST_FILE, FA_CREATE_ALWAYS | FA_WRITE);
	if(MyFile_Res == FR_OK) {
		uart_printf("ok,write...\r\n");
		
		MyFile_Res = f_write(&MyFile, MyFile_WriteBuffer,strlen(MyFile_WriteBuffer),&MyFile_Num);
		if (MyFile_Res == FR_OK) {
			uart_printf("ok \r\n");
			uart_printf("%s\r\n",MyFile_WriteBuffer);
		} else {
			uart_printf("err!\r\n");
			f_close(&MyFile);	  //关闭文件	
			return -1;			
		}
		f_close(&MyFile);	  //关闭文件			
	} else {
		uart_printf("err!\r\n");
		f_close(&MyFile);	  //关闭文件	
		return -1;		
	}
	
	uart_printf("-------------FatFs read---------------\r\n");	
	
	rlen = strlen(MyFile_WriteBuffer);							
	MyFile_Res = f_open(&MyFile, FATFS_TEST_FILE, FA_OPEN_EXISTING | FA_READ);	
	MyFile_Res = f_read(&MyFile,MyFile_ReadBuffer,rlen,&MyFile_Num);	
	if(MyFile_Res == FR_OK) {
		uart_printf("ok, verify...\r\n");
		PrintFormat(MyFile_ReadBuffer, rlen);
		for(i=0;i<rlen;i++) {
			if(MyFile_WriteBuffer[i] != MyFile_ReadBuffer[i]) {
				uart_printf("err!\r\n");
				f_close(&MyFile);	  
				return -1;
			}
		}
		uart_printf("ok\r\n");
		uart_printf("%s\r\n", MyFile_ReadBuffer);
	} else {
		uart_printf("err!\r\n");
		f_close(&MyFile);	  //关闭文件	
		return -1;		
	}	
	
	f_close(&MyFile);	  //关闭文件	

	uart_printf("sdmmc and fatfs test finish!\r\n");
	return 0;
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

