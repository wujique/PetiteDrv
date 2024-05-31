
#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "mcu.h"
#include "petite_config.h"

#include "log.h"
#include "mem/alloc.h"
#include "mem/tlsf.h"

#include "cmsis_os.h"

osMutexId_t PmallocMutex = NULL;

int palloc_init(void);
void wjq_free_m(void*ap);

uint8_t WjqAllocGd = 0;

typedef struct
{
	void *(*init_pool)(void* mem, size_t bytes);
	void *(*imalloc)(void* node, size_t size);
	void (*ifree)(void *node, void* ptr);
	void *(*irealloc)(void *node, void* ptr, size_t size);
}MallocInterface;

MallocInterface TlsfInterface={
	.init_pool = tlsf_create_with_pool,
	.imalloc = tlsf_malloc,
	.ifree = tlsf_free,
	.irealloc = tlsf_realloc,
	};

MallocInterface KRallocInterface={
	.init_pool = kr_create_with_pool,
	.imalloc = kr_malloc,
	.ifree =  kr_free,
	.irealloc = kr_realloc,
	};

//MallocInterface *WJQInterface = &KRallocInterface;
MallocInterface *WJQInterface = &TlsfInterface;

/*
	用于支持多个堆。
	如果要支持多个堆，互斥锁就需要一个堆一个。
*/
static void *PmallocNode;

/*
	二次封装，如果需要做互斥，在_m后缀的函数内实现。
*/
void *wjq_malloc_m(unsigned nbytes, const char *f, int l)
{
	void*p;
	//printf("malloc:%d, %s, %d,", nbytes, f, l);

	if (WjqAllocGd == 0) {
		palloc_init();
		WjqAllocGd = 1;
	}
	osMutexAcquire(PmallocMutex, osWaitForever);
	p = WJQInterface->imalloc(PmallocNode, nbytes);
	osMutexRelease(PmallocMutex);
	if (p ==NULL) {
		/*对于嵌入式来说，没有机制整理内存，因此，不允许内存分配失败*/
		printf("\r\n\r\n----petite malloc err!!---------\r\n\r\n");
		while(1);
	}

	//printf("0X%08X\r\n", p);

	return p;
}

void *wjq_calloc_m(size_t n, size_t size)
{
	void *p;

	//printf("calloc:%d,%d,", n, size);
	if (WjqAllocGd == 0) {
		palloc_init();
		WjqAllocGd = 1;
	}
	osMutexAcquire(PmallocMutex, osWaitForever);
	p = WJQInterface->imalloc(PmallocNode, n*size);
	osMutexRelease(PmallocMutex);
	if(p!=NULL) {
		memset((char*) p, 0, n*size);
	} else {
		/*对于嵌入式来说，没有机制整理内存，因此，不允许内存分配失败*/
		printf("\r\n\r\n----petite cmalloc err!!---------\r\n\r\n");
		while(1);
	}

	//printf("0X%08X\r\n", p);
	return p;
}

void *wjq_realloc_m(void *ap, unsigned int newsize)
{
	void *p;
	
	if (WjqAllocGd == 0) {
		palloc_init();
		WjqAllocGd = 1;
	}
	//printf("realloc:0x%08x,%d,", ap, newsize);
	osMutexAcquire(PmallocMutex, osWaitForever);
	p = WJQInterface->irealloc(PmallocNode, ap, newsize);
	osMutexRelease(PmallocMutex);

	if (p ==NULL) {
		/*对于嵌入式来说，没有机制整理内存，因此，不允许内存分配失败*/
		printf("\r\n\r\n----petite rmalloc err!!---------\r\n\r\n");
		while(1);
	}

	//printf("0X%08X\r\n", p);
	return p;
}

void wjq_free_m(void*ap)
{
	if(ap==NULL) return;
	
	//printf("free_m:0x%08x\r\n", ap);
	osMutexAcquire(PmallocMutex, osWaitForever);
	WJQInterface->ifree(PmallocNode, ap);
	osMutexRelease(PmallocMutex);
	return;
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
#ifdef __CC_ARM//优先配置MDK，因为MDK也可能使用GNU扩展
	__align(4) //保证内存池四字节对齐
	char AllocArray[AllocArraySize];
#elif __GNUC__
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
int palloc_init(void)
{
	PmallocMutex = osMutexNew(NULL);
	if (PmallocMutex == NULL) printf("palloc_init mutex err!\r\n");
	osMutexAcquire(PmallocMutex, osWaitForever);
	PmallocNode = WJQInterface->init_pool((void *)AllocArray, AllocArraySize);	
	osMutexRelease(PmallocMutex);
	return 0;
}


