#ifndef __P_KFIFO_H__
#define __P_KFIFO_H__

/* kfifo 结构 */
struct _pkfifo {
    unsigned int    in;//队尾下标，入队列的offset为(in % size 或者 in & mask)
    unsigned int    out;//队首下标，出队列的offset为(out % size 或者 out & mask)
    unsigned int    size;//缓冲区元素个数(size) - 1；使用&mask，替换%size，提升效率
    unsigned int    esize;//缓存区每个元素的size(element size)
    void        	*data;//存数据的缓冲
};

/* 写入一个U8类型 */
#define PKFIFO_IN_1U8(pkfifo, ch) do{\
						*((char *)pkfifo->data+pkfifo->in) = ch;\
						pkfifo->in++;\
						if(pkfifo->in >= pkfifo->size) pkfifo->in = 0;\
						}while(0);
						
/* 读出一个U8类型 */						
#define PKFIFO_OUT_1U8(res, pkfifo, pch) do{\
						res = 0;\
						if(pkfifo->out == pkfifo->in) break;\
						*(pch) = *((char *)pkfifo->data+pkfifo->out);\
						pkfifo->out++;\
						if(pkfifo->out >= pkfifo->size) pkfifo->out = 0;\
						res = 1;\
						}while(0);

/* 写入一个U16类型 */						
#define PKFIFO_IN_1U16(pkfifo, ch) do{\
						*((u16 *)pkfifo->data+pkfifo->in) = ch;\
						pkfifo->in++;\
						if(pkfifo->in >= pkfifo->size) pkfifo->in = 0;\
						}while(0);

extern int pkfifo_init(struct _pkfifo *fifo, void *buffer,	unsigned int size, size_t esize);
extern int pkfifo_in_1(struct _pkfifo *fifo, void *buffer);
#endif

