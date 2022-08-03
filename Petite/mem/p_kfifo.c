/*
	内核无锁队列 -- kfifo
	参考linux 中的kfifo，实现一个精简的kfifo模块

	https://www.jianshu.com/p/9d7edaba5d8c
	1 一个整数如果是2的整数幂，那么对齐取余操作%，可以转化为与其&操作，提升效率
	2 使用整数自动溢出构成的一个环来替代循环数组，简化判空、判满及各种长度运算
	3 先操作buffer，再计数。实现无锁队列

	屋脊雀注：
		linux中的KFIFO更适合每次写入较多数据。
		在单片机的系统中，我们更多情况是：
			写，通常是1次写一个数据；
			读，则1次读多个数据。
		比如串口接收，1次只有一个数据。
		处理数据则会尽量读多点数据同时处理，这样能加快处理速度。
		所以我们认为，在单片机中，简单粗暴的直接写，可能效率更高。

		kfifo有两个技术点
		1 数据指针是void *，这样的做法是用于支持不同的数据字长，
			比如u8 u16，甚至可以是结构体。称之为元素。
		2 esize就是元素的长度，U8就是1字节，u16就是2字节。
		3 初始化，size会被设置为2的n次方，在单片机中并不是最好的策略。
			比如，定义一个65字节的缓冲，2次幂的值是128，会浪费很多空间。
*/
#include "mcu.h"
#include "petite_config.h"
#include "board_sysconf.h"
#include "log.h"

#include "p_kfifo.h"

/*
	初始化队列
	fifo
	buffer  缓冲指针
	size	元素个数
	esize	元素大小
*/
int pkfifo_init(struct _pkfifo *fifo, void *buffer,
		unsigned int size, size_t esize)
{
	fifo->in = 0;
	fifo->out = 0;
	fifo->size = size;
	fifo->esize = esize;
	fifo->data = buffer;
	
	return 0;
}
/* 一次写入一个元素，通常用于中断接收数据 */
int pkfifo_in_1(struct _pkfifo *fifo, void *buffer)
{
	char *p;

	p = (char *)(fifo->data) + fifo->in*fifo->esize;
	/* 拷贝一个元素*/
	memcpy(p, buffer, fifo->esize);
	fifo->in++;
	if(fifo->in >= fifo->size) fifo->in = 0;
		
	return 1;
}

int pkfifo_out(struct _pkfifo *fifo, void *buffer, unsigned int len)
{
	unsigned int cnt =0;

	while(1) {
		if(cnt >= len) break;
		if(fifo->in == fifo->out) break;

		
	}	

	return cnt;
}

/*-----------------------------------------------------*/
struct _pkfifo kfifo_u8;
char kfifo_u8_buf[516];

struct _pkfifo kfifo_u16;
u16 kfifo_u16_buf[516];

void pkfifo_test(void)
{
	int i;
	struct _pkfifo *pfifo;
	u16 tmpu16;
	char tmpu8;

	pkfifo_init(&kfifo_u8, (void *)kfifo_u8_buf, 516, sizeof(char));
	tmpu8 = 0;
	for(i=0; i< 512;i++) {
		pkfifo_in_1(&kfifo_u8, &tmpu8);
		tmpu8++;
	}
	pfifo = &kfifo_u8;
	PKFIFO_IN_1U8(pfifo, 0x55);

	PrintFormat(kfifo_u8_buf, 513);
	/**/
	pkfifo_init(&kfifo_u16, (void *)kfifo_u16_buf, 516, sizeof(u16));
	tmpu16 = 0;
	for(i=0; i< 512;i++) {
		pkfifo_in_1(&kfifo_u16, &tmpu16);
		tmpu16++;
	}
	pfifo = &kfifo_u16;
	PKFIFO_IN_1U16(pfifo, 0x1234);
	PrintFormatU16(kfifo_u16_buf, 513);

	#if 0
	u16 a,b,c;
	
	b = 0xfff0;
	a = 0xffff;
	a += 5;
	c = a-b;
	uart_printf("------a:%d, a-b:%d,c:%d\r\n", a, a-b,c);
	#endif
}



