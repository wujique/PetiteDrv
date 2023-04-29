

#include "stdlib.h"

#include "mcu.h"
#include "petite_config.h"

#include "log.h"
#include "mem/alloc.h"

int wjq_alloc_init(void);
void wjq_free_m(void*ap);

uint8_t WjqAllocGd = 0;


/*
	二次封装，如果需要做互斥，在_m后缀的函数内实现。
*/
void*wjq_malloc_m(unsigned nbytes, const char *f, int l)
{
	void*p;
	//uart_printf("malloc:%d, %s, %d\r\n", nbytes, f, l);
	//uart_printf("malloc:%d\r\n", nbytes);
	if (WjqAllocGd == 0) {
		wjq_alloc_init();
		WjqAllocGd = 1;
	}
	p = kr_malloc(nbytes);

	if (p ==NULL) {
		/*对于嵌入式来说，没有机制整理内存，因此，不允许内存分配失败*/
		wjq_log(LOG_ERR, "\r\n\r\n----wujique malloc err!!---------\r\n\r\n");
		while(1);
	}
	return p;
}

void*wjq_calloc_m(size_t n, size_t size)
{
	void *p;

	//wjq_log(LOG_DEBUG, "wjq_calloc\r\n");
	if (WjqAllocGd == 0) {
		wjq_alloc_init();
		WjqAllocGd = 1;
	}
	
	p = kr_malloc(n*size);

	if(p!=NULL) {
		memset((char*) p, 0, n*size);
	}
	return p;
}

void *wjq_realloc_m(void *ap, unsigned int newsize)
{
	//ALLOC_HDR*bp, *p, *np;
	void *p;
	unsigned nunits;
	unsigned aunits;

	if (WjqAllocGd == 0) {
		wjq_alloc_init();
		WjqAllocGd = 1;
	}
	
	//wjq_log(LOG_DEBUG, "wjq_realloc: %d\r\n", newsize);
	if(ap == NULL) {
		p = kr_malloc(newsize);
		return p;	
	}

	if (newsize == 0) {
		wjq_free_m(ap);
		return NULL;
	}

	#if 0
	/*计算要申请的内存块数*/
	nunits = ((newsize + sizeof(ALLOC_HDR)-1) / sizeof(ALLOC_HDR))+1;
	/* 函数传入的ap是可使用内存的指针，往前退一个结构体位置，
		也就是下面的bp，才是记录内存信息的位置*/
	bp = (ALLOC_HDR*)ap-1;	/* point to block header */
	if (nunits <= bp->s.size) {
		/* 	新的申请数不比原来的大，暂时不处理
		浪费点内存。 		*/
		return ap;
	}
	#endif
	
	#if 1
	/*无论如何都直接申请内存然后拷贝数据*/
	p = kr_malloc(newsize);
	memcpy(p, ap, newsize);/* need fix bug*/
	wjq_free_m(ap);
	
	return p;
	#else
	/*
	  找到需要释放的内存的前后空闲块
	  其实就是比较内存块位置的地址大小
	*/
	for(p = freep; ! ((bp>p)&&(bp<p->s.ptr)); p = p->s.ptr)
	{
		if((p>=p->s.ptr)&&((bp>p)||(bp<p->s.ptr)))
		{
			/*
				当一个块，
				p>=p->s.ptr 本身起始地址指针大于下一块地址指针
				bp>p 要释放的块，地址大于P
				bp<p->s.ptr 要释放的块，地址小于下一块
			*/
			break;		/* freed block at start or end of arena */
		}
	}

	/**/
	if((bp + bp->s.size) == p->s.ptr)
	{
		/*增加的内存块*/
		aunits = (nunits - bp->s.size);
		if( aunits == p->s.ptr->s.size)
		{	
			/*刚刚好相等*/
			p->s.ptr = p->s.ptr->s.ptr;
			bp->s.size = nunits;
			return ap;
		}
		else if(aunits < p->s.ptr->s.size)
		{
			np = p->s.ptr + aunits;//切割aunits分出去，np就是剩下块的地址
			np->s.ptr = p->s.ptr->s.ptr;
			np->s.size = p->s.ptr->s.size - aunits;
				
			p->s.ptr = np;

			bp->s.size = nunits;
			return ap;
		}
		
	}
	
	/*需要重新申请内存*/
	bp = wjq_malloc_t(newsize);
	memcpy(bp, ap, newsize);
	wjq_free(ap);
	
	return bp;
	#endif
	
}

void wjq_free_m(void*ap)
{
	if(ap==NULL) return;
	
	kr_free(ap);
}

#include "p_malloc.h"

void wjq_malloc_test(void)
{
	char*p;

	p = (char*)
	wjq_malloc(1024);

	/*打印指针，看看是不是4字节对齐*/
	wjq_log(LOG_DEBUG, "pointer :%08x\r\n", p);

	memset(p, 0xf0, 1024);
	wjq_log(LOG_DEBUG, "data:%02x\r\n", * (p+1023));
	wjq_log(LOG_DEBUG, "data:%02x\r\n", * (p+1024));

	wjq_free_m(p);
	wjq_log(LOG_DEBUG, "alloc free ok\r\n");

	while(1);
}

/*
使用编译器定义的堆作为内存池，注意，直接使用malloc，会调用C库的函数，使用的就是堆
	，
要防止冲突
*/
//#define ALLOC_USE_HEAP	//用堆做内存池
#define ALLOC_USE_ARRAY //用定义的数组做内存池

/* Get addresses for the HEAP start and end */
#ifdef ALLOC_USE_HEAP
	#pragma section = "HEAP"

	char*__HEAP_START = __section_begin("HEAP");
	char*__HEAP_END = __section_end("HEAP");
#endif

#ifdef ALLOC_USE_ARRAY
	/*不同编译器用的宏不一样，各编译器定义请参考core_m4.h*/
#ifdef __GNUC__
	__attribute((aligned(4))) char AllocArray[AllocArraySize];
#else
	__align(4) //保证内存池四字节对齐
	char AllocArray[AllocArraySize];
#endif

	char*__HEAP_START = AllocArray;
	char*__HEAP_END =   AllocArray + AllocArraySize;
#endif

/*
	初始化内存堆
	*/
int wjq_alloc_init(void)
{
	kr_alloc_create_with_pool((void *)AllocArray, AllocArraySize);	
}


