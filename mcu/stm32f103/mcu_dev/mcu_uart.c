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
#include "mcu_uart.h"
#include "log.h"
#include "alloc.h"
#include "p_kfifo.h"
#include "bus_uart.h"


static BusUartNode *McuUart[MCU_UART_MAX];
/*------------------------------------------------------*/

/**
 *@brief:      
 *@details:    打开串口，实际就是初始化串口
 *@param[in]   s32 comport  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_init(McuUartNum comport)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	if(comport >= MCU_UART_MAX)	return -1;

	if(comport == MCU_UART_1) {

		// 打开串口时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		// TXD ---- PA9
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		// RXD ---- PA10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART 初始化设置
		USART_InitStructure.USART_BaudRate = 115200;//波特率;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为 8 位
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发
		USART_Init(USART1, &USART_InitStructure); //初始化串口
		USART_Cmd(USART1, ENABLE); //使能串口
		USART_ClearFlag(USART1, USART_FLAG_TC);

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
		//Usart1NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //响应优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

	}
	else if(comport == MCU_UART_2)	{
		
		// 打开串口时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		// TXD ---- PA2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //复用
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		// RXD ---- PA3
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //复用
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART 初始化设置
		USART_InitStructure.USART_BaudRate = 115200;//波特率;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为 8 位
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发
		USART_Init(USART2, &USART_InitStructure); //初始化串口
		USART_Cmd(USART2, ENABLE); //使能串口
		USART_ClearFlag(USART2, USART_FLAG_TC);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
		//Usart2 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //响应优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

	}
	else if(comport == MCU_UART_3)	{

		// 打开串口时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		// TXD ---- PB10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //复用
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		// RXD ---- PB11
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //复用
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

		GPIO_Init(GPIOB, &GPIO_InitStructure);

		//USART 初始化设置
		USART_InitStructure.USART_BaudRate = 115200;//波特率;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为 8 位
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发
		USART_Init(USART3, &USART_InitStructure); //初始化串口
		USART_Cmd(USART3, ENABLE); //使能串口
		USART_ClearFlag(USART3, USART_FLAG_TC);

		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
		//Usart3 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //响应优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else {
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
s32 mcu_uart_deinit (McuUartNum comport)
{
	if(comport >= MCU_UART_MAX)
		return -1;

	if (comport == MCU_UART_1) {
    	USART_Cmd(USART1, DISABLE);
    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
    } else if(comport == MCU_UART_2) {
    	USART_Cmd(USART2, DISABLE);
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
    } else if(comport == MCU_UART_3) {
    	USART_Cmd(USART3, DISABLE);
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
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
    USART_InitTypeDef USART_InitStructure;

	if(comport >= MCU_UART_MAX)
		return -1;

    USART_Cmd(McuUart[comport].USARTx, DISABLE);
    USART_InitStructure.USART_BaudRate = baud; 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(McuUart[comport].USARTx, &USART_InitStructure);
	
    USART_Cmd(McuUart[comport].USARTx, ENABLE); 

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
s32 mcu_uart_send (McuUartNum comport, u8 *buf, s32 len)
{
    u32 t;
    u16 ch;
  
    if (len <= 0) return(-1);
    if(buf == NULL) return(-1);
	if(comport >= MCU_UART_MAX) return -1;

    while(len != 0)  {
        // 等待串口发送完毕
        t = 0;
        while(USART_GetFlagStatus(McuUart[comport].USARTx, USART_FLAG_TXE) == RESET)
        {
            if(t++ >= 0x1000000)
                return(-1);
        }  
        ch = (u16)(*buf & 0xff);
        USART_SendData(McuUart[comport].USARTx, ch);
        buf++;
        len--;
    }
    
    return(0);
}
/**
 *@brief:      mcu_uart3_IRQhandler
 *@details:    串口中断处理函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_uart3_IRQhandler(void)
{
    unsigned short ch;
	struct _pkfifo *pfifo;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET)
        {
            // 串口数据接收字符有丢失
            //wjq_log(LOG_DEBUG, "1");
            ch = USART_ReceiveData(USART3);
            USART_GetITStatus(USART3, USART_IT_ORE); // 清除ORE标记

            McuUart[MCU_UART_3]->OverFg = 1;
        }
        else
        {
            ch = USART_ReceiveData(USART3);
            //uart_printf("%02x", ch);
            pfifo = &McuUart[MCU_UART_3]->Kfifo;
			PKFIFO_IN_1U8(pfifo, ch);
        }
    }
    
    if(USART_GetITStatus(USART3, USART_IT_FE) != RESET)
    {
        /* Clear the USART3 Frame error pending bit */
        USART_ClearITPendingBit(USART3, USART_IT_FE);
        USART_ReceiveData(USART3);

    }
}
/**
 *@brief:      mcu_uart2_IRQhandler
 *@details:    串口中断处理函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_uart2_IRQhandler(void)
{
	unsigned short ch;
	struct _pkfifo *pfifo;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		if(USART_GetITStatus(USART2, USART_IT_ORE) != RESET)
		{
			// 串口数据接收字符有丢失
			//uart_printf("1");
			ch = USART_ReceiveData(USART2);
			USART_GetITStatus(USART2, USART_IT_ORE); // 清除ORE标记

			McuUart[MCU_UART_2]->OverFg = 1;
		}
		else
		{
			ch = USART_ReceiveData(USART2);
			//uart_printf("%02x", ch);
			pfifo = &McuUart[MCU_UART_2]->Kfifo;
			PKFIFO_IN_1U8(pfifo, ch);
		}
	}
	
	if(USART_GetITStatus(USART2, USART_IT_FE) != RESET)
	{
		/* Clear the USART3 Frame error pending bit */
		USART_ClearITPendingBit(USART2, USART_IT_FE);
		USART_ReceiveData(USART2);

	}
}


/**
 *@brief:      mcu_uart1_IRQhandler
 *@details:    串口中断处理函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_uart1_IRQhandler(void)
{
	unsigned short ch;
	struct _pkfifo *pfifo;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
		{
			// 串口数据接收字符有丢失
			//uart_printf("1");
			ch = USART_ReceiveData(USART1);
			USART_GetITStatus(USART1, USART_IT_ORE); // 清除ORE标记

			McuUart[MCU_UART_1]->OverFg = 1;
		}
		else
		{
			ch = USART_ReceiveData(USART1);
			//uart_printf("%02x", ch);
			pfifo = &McuUart[MCU_UART_1]->Kfifo;
			PKFIFO_IN_1U8(pfifo, ch);
		}
	}
	
	if(USART_GetITStatus(USART1, USART_IT_FE) != RESET)
	{
		/* Clear the USART3 Frame error pending bit */
		USART_ClearITPendingBit(USART1, USART_IT_FE);
		USART_ReceiveData(USART1);

	}
}




