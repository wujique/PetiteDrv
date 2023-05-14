/**
 * @file            mcu_uart.c
 * @brief           串口驱动
 * @author          wujique
 * @date            2017年10月24日 星期二
 * @version         初稿
 * @par             
 * @par History:
 * 1.日    期:      2017年10月24日 星期二
 *   作    者:     
 *   修改内容:        创建文件
		版权说明：
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

#include "mcu_uart.h"
#include "petite.h"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart7;
extern int BusUartGd[32];
/**
 *@brief:      mcu_uart_init
 *@details:       初始化串口设备
 *@param[in]  void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_init(McuUartNum comport, BusUartNode *BusNode, const BusUartPra *Pra)
{

	return 0;
}

/**
 *@brief:      mcu_uart_open
 *@details:    打开串口，实际就是初始化串口
 *@param[in]   s32 comport  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_open(McuUartNum comport, BusUartNode *BusNode, const BusUartPra *Pra)
{
	
	if(comport == MCU_UART_4) {
		HAL_UART_Receive_IT(&huart4, BusNode->buf, 1);
	} else if(comport == MCU_UART_7) {
		/* 如果不接上串口模块，执行这个函数会 hardfault */
		/* issue : 7b0 mdk, hardfault*/
		HAL_UART_Receive_IT(&huart7, BusNode->buf, 1);
	} else {
		/* 串口号不支持*/
		return -1;
	}
	
	return (0);
}
/**
 *@brief:      mcu_uart_close
 *@details:    关闭串口
 *@param[in]   s32 comport  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_close (McuUartNum comport)
{
	if(comport >= MCU_UART_MAX) return -1;

	if(comport == MCU_UART_2) {
		

	} else if(comport == MCU_UART_4) {

	} else return -1;

	return(0);
}

/**
 *@brief:      mcu_dev_uart_set_baud
 *@details:    设置串口波特率
 *@param[in]   s32 comport   
               s32 baud      
               s32 databits  
               s32 parity    
               s32 stopbits  
               s32 flowctl   
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_set_baud (McuUartNum comport, s32 baud)
{

	return 0;
}


/**
 *@brief:      mcu_dev_uart_write
 *@details:    写串口数据(发送数据)
 *@param[in]   s32 comport  
               u8 *buf      
               s32 len      
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_write(McuUartNum comport, u8 *buf, s32 len)
{
	u32 t;
	u16 ch;
  
	if (len <= 0) return(-1);
		
	if(buf == NULL) return(-1);
	
	if(comport == MCU_UART_4) {
		HAL_UART_Transmit(&huart4, buf, len, len*10);
	} else if(comport == MCU_UART_7) {
		HAL_UART_Transmit(&huart7, buf, len, len*10);
	} else return -1;
	
	return(0);
}
/*
	在HAL库函数中回调，将中断接收到的数据填入接收缓冲
	*/
void mcu_uart_rec(UART_HandleTypeDef *huart, uint8_t data)
{
	//uart_printf("%c", data);
	BusUartNode *node;
	struct _pkfifo *pfifo;
	
	if (huart == &huart4) {
		//uart_printf("%c", data);
		node = (BusUartNode *)BusUartGd[MCU_UART_4];
		pfifo = &node->Kfifo;
		PKFIFO_IN_1U8(pfifo, data);
	}else if (huart == &huart7) {
		node = (BusUartNode *)BusUartGd[MCU_UART_7];
		pfifo = &node->Kfifo;
		PKFIFO_IN_1U8(pfifo, data);
		//uart_printf("%c", data);
	}
}

/**
 *@brief:      mcu_dev_uart_test
 *@details:    串口测试
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
#if 0
void mcu_uart_test(void)
{
    u8 buf[12];
    s32 len = 0;
    s32 res;

	wjq_log(LOG_INFO, "%s,%s,%d,%s\r\n", __FUNCTION__,__FILE__,__LINE__,__DATE__);
	while(1)
	{
	    len =  mcu_uart_read (MCU_UART_4, buf, 10);
		if(len != 0)
		{
	    	//wjq_log(LOG_FUN, "mcu_dev_uart_read :%d\r\n", len);
	    	res = mcu_uart_write(MCU_UART_4, buf, len);
	    	//wjq_log(LOG_FUN, "mcu_dev_uart_write res: %d\r\n", res);
		}
	}
    
}

#endif

