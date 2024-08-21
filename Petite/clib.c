
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#include "mcu.h"
#include "petite.h"

#if 0//(__PETITE_COMPILER_IS__ == _petite_COMPILER_ARM_CC_6)
void _sys_exit(int ret)
{
    (void)ret;
    while(1) {}
}
#endif

#if 0//__IS_COMPILER_ARM_COMPILER__
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

/*----------------------------------------*/

//#pragma import(__use_no_semihosting_swi)
//#pragma import(_main_redirection)
 
 
const char __stdin_name[150];
const char __stdout_name[150];
const char __stderr_name[150];
typedef int FILEHANDLE;
 
//重写标准库函数，这时printf、fopen、fclose等文件操作函数运行时就会调用你的重写函数，这些重写函数只是几个简单的例子，并没有重写所有的文件操作函数
void _sys_exit(int status)
{
    while(1);
}
FILEHANDLE _sys_open(const char *name, int openmode)
{
    return 0;
}
 
int _sys_close(FILEHANDLE fh)
{
    return 0;
}
 
int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
    return 0;
}
/* 重定义fputc和_sys_read可能有冲突*/ 
int _sys_read(FILEHANDLE fh, unsigned char*buf, unsigned len, int mode)
{
    return 0;
}
 
int _sys_istty(FILEHANDLE fh)
{
    return 0;
}
 
int _sys_seek(FILEHANDLE fh, long pos)
{
    return 0;
}
 
int _sys_ensure(FILEHANDLE fh)
{
    return 0;
}
 
long _sys_flen(FILEHANDLE fh)
{
    return 0;
}
 
int _sys_tmpnam(char *name, int fileno, unsigned maxlength)
{
    return 0;
}
 
void _ttywrch(int ch)
{
    return;
}
int remove(const char *filename)
{
    return 0;
}
 
char *_sys_command_string(char *cmd, int len)
{
    return 0;
}
 
/*----------------------*/
#include "time.h"
extern volatile uint32_t SysTickCnt;//系统上电后运行时间计数，如果是32位，1ms周期下最大计时49.7天
extern time_t time_dat;//设置localtime相对于公元1970年1月1日0时0分0秒算起至今的UTC时间所经过的秒数

clock_t clock (void) {
    return (SysTickCnt);
}
 
time_t time(time_t *t)
{
	if (t != NULL) *t = time_dat;

	return time_dat;
}
 
const char * __getzone(void)
{
    return ": GMT+8:GMT+9:+0800";
}

