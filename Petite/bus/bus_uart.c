/**
	屋脊雀工作室
	www.wujique.com
		版权说明：
		1 源码归屋脊雀工作室所有。
		2 可以用于商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/
/*代码说明：
	这是一个简单的串口封装层。之前没有这个层，在移植时，需要在对应的MCU的串口驱动实现
	串口管理流程和缓冲管理。将这两部分提取出来得到bus_uart。
	串口的使用，关键在缓冲如何处理，如何接受数据。
	现在的版本是用唤醒缓冲接收数据。应用层使用数据需要调用读接口进行一次数据拷贝。
	当数据量大，可以考虑直接将用回调函数，通过修改接收数缓冲的方式，减少数据拷贝。
	*/
#include "mcu.h"
#include "log.h"
#include "petite.h"


int BusUartGd[32];
/*
	初始化所有串口 
	*/
int bus_uart_init(void) {

	memset(BusUartGd, 0, sizeof(BusUartGd));
	//mcu_uart_init(0,0,0);
	return 0;
}

/*	打开串口 
	失败返回0；
	成功返回节点 
	需要传入串口的参数        	*/
void *bus_uart_open(char *name, const BusUartPra *Pra)
{
	int res;
	/* 查找有没有这个串口 */
	BusUartNode *node;
	McuUartNum comport;
	
	if(strcmp(name, "uart1") == 0) {
		comport = MCU_UART_1;	
	} else if(strcmp(name, "uart2") == 0) {
		comport = MCU_UART_2;	
	} else if(strcmp(name, "uart3") == 0) {
		comport = MCU_UART_3;	
	} else if(strcmp(name, "uart4") == 0) {
		comport = MCU_UART_4;
	} else if(strcmp(name, "uart7") == 0) {
		comport = MCU_UART_7;
	} else 	{
		uart_printf("no %s\r\n", name);
		return NULL;
	}

	if(BusUartGd[comport] != 0) {
		uart_printf("uart is openning\r\n");
		return NULL;
	}
	node = wjq_malloc(sizeof(BusUartNode));
	
	if (node == NULL) {
		
	}
	node->buf = wjq_malloc(Pra->bufsize);
	if (node->buf == NULL) {

	}
	pkfifo_init(&(node->Kfifo), node->buf, Pra->bufsize, 1);
	
	res = mcu_uart_open(comport, node, Pra);
	if(res != 0) {
		wjq_free(node->buf);
		wjq_free(node);	
		uart_printf("mcu uart open err\r\n");
		return NULL;
	}
	node->comport = comport;
	BusUartGd[comport] = (int)node;
	uart_printf("mcu uart open %08x\r\n", node);
	return (void *)node;
}

void bus_uart_close(void *vnode)
{
	BusUartNode *node = vnode;

	if(node == NULL) return;


	//mcu_uart_deinit(node->comport);
	wjq_free(node->buf);
	wjq_free(node);
	BusUartGd[node->comport] = 0;
	return;
}

/**
 *@brief:      mcu_dev_uart_tcflush
 *@details:    清串口接收缓冲
 *@param[in]   s32 comport  
 *@param[out]  无
 *@retval:     
 */
s32 bus_uart_tcflush(void *node)
{ 

}

/**
 *@brief:      mcu_uart_read
 *@details:    读串口数据（接收数据）
 *@param[in]   s32 comport  
               u8 *buf      
               s32 len      
 *@param[out]  无
 *@retval:     
 */
s32 bus_uart_read (void *vnode, u8 *buf, s32 len)
{
	BusUartNode *node = vnode;
    s32 i;
    struct _pkfifo *pfifo;
	int res;
	
    if (len <= 0) return(-1);
    if (buf == NULL) return(-1);
	if (BusUartGd[node->comport] == 0) return 0;
    i = 0;

	pfifo = &node->Kfifo;
	while(1) {
		/* 要优化，不能一直用while循环 */
		if(i >= len) break;
		PKFIFO_OUT_1U8(res, pfifo, buf);
		if(res == 0) break;
		buf ++;
		i ++;
	}
	
	return i;
}

int bus_uart_write(void *vnode, u8 *buf, s32 len)
{
	BusUartNode *node=vnode;
	return mcu_uart_send(node->comport, buf, len);
}

/**
 *@brief:      mcu_dev_uart_test
 *@details:    串口测试
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
const static BusUartPra TestPcPortPra={
	.BaudRate = 115200,
	.bufsize = 256,
	};
#define TEST_UART_PORT  "uart3"

void bus_uart_test(void)
{
    u8 buf[12];
    s32 len;
    s32 res;
	BusUartNode *LogUartNode;
	LogUartNode = bus_uart_open(TEST_UART_PORT, &TestPcPortPra);
	
	while(1) {
    	len =  bus_uart_read (LogUartNode, buf, 10);
		if(len != 0) {
    		//wjq_log(LOG_FUN, "mcu_dev_uart_read :%d\r\n", len);
    		res = bus_uart_write(LogUartNode, buf, len);
    		//wjq_log(LOG_FUN, "mcu_dev_uart_write res: %d\r\n", res);
    		//wjq_log(LOG_FUN, "%s,%s,%d,%s\r\n", __FUNCTION__,__FILE__,__LINE__,__DATE__);
		}
	}
}


