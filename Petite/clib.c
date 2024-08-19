
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#include "mcu.h"
#include "petite.h"

#if (__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_6)
void _sys_exit(int ret)
{
    (void)ret;
    while(1) {}
}
#endif

#if ((__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_5 )\
		|| (__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_6 ))
/* 为 arm compiler 5 和 arm compiler 6 都添加这个空函数 */
void _ttywrch(int ch)
{
    //ARM_2D_UNUSED(ch);
}
#endif

#if ((__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_6) && defined(__MICROLIB))
void __aeabi_assert(const char *chCond, const char *chLine, int wErrCode) 
{
    (void)chCond;
    (void)chLine;
    (void)wErrCode;
    
    while(1) {
        __NOP();
    }
}
#endif

/*
	移植某些组件时，缺失某些函数会编译失败。
	在此定义一些空函数。
	*/
void exit(int status)
{

}


int system(const char *command)
{

	return 0;
}


int isascii(int c) {
    return (c >= 0) && (c <= 127);
}

#if ((__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_5 )\
		|| (__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_6 ))//优先配置MDK，因为MDK也可能使用GNU扩展
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#warning "__CC_ARM redef printf putchar in fputc"
#elif (__PETITE_COMPILER_IS__ == _petite_COMPILER_GCC)
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#warning "__GNUC__ redef printf putchar in __io_putchar"
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#warning "default redef printf putchar in fputc"
#endif /* __GNUC__ */

extern s32 log_redef_putc(u8 *buf, s32 len);
PUTCHAR_PROTOTYPE
{
	#if 0
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART3, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	#else
	/* 用hal库实现putc，效率很低*/
	char tmp[2];
	tmp[0] = ch;

	log_redef_putc(tmp, 1);
	#endif
	
    return ch;
}

/*-------------------------------------------------------------*/
#if 1
#include "mem/p_malloc.h"

//AC5
#if (__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_5 )
	#pragma import(__use_no_heap_region)  //声明不使用C库的堆
#endif

#if (__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_6 )
//AC6:Arm Compiler 6（armclang）
//__asm(".global __use_no_heap_region\n\t"); //声明不使用C库的堆
#endif

void *malloc (size_t size){
        return wjq_malloc_m(size);
}

void free(void* p){
        wjq_free_m(p);
}

void *realloc(void* p,size_t want){
        return wjq_realloc_m(p, want);
}

void *calloc(size_t nmemb, size_t size){
        return wjq_calloc_m(nmemb,size);
}
#endif

void clib_test(void)
{

	/* test: c 库内存分配 */
	void *p = NULL;
	void *pcl = NULL;

	p = malloc(128);
	if (p == NULL) printf("malloc err!\r\n");
	pcl = calloc(256, 1);
	if (p == NULL) printf("calloc err!\r\n");
	p = realloc(p, 256);
	if (p == NULL) printf("realloc err!\r\n");
	free(p);
	free(pcl);

}