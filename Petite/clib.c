
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "mcu.h"

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

#if 1
#ifdef __CC_ARM//优先配置MDK，因为MDK也可能使用GNU扩展
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#warning "__CC_ARM redef printf putchar in fputc"
#elif __GNUC__
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

#endif 

#if 0
//AC5
#pragma import(__use_no_heap_region)  //声明不使用C库的堆
//AC6
__asm(".global __use_no_heap_region\n\t"); //声明不使用C库的堆

void *malloc (size_t size){
        return OS_Malloc(size);
}

void free(void* p){
        OS_Free(p);
}

void *realloc(void* p,size_t want){
        return OS_Realloc(p,want);
}

void *calloc(size_t nmemb, size_t size){
        return OS_Calloc(nmemb,size);
}
#endif