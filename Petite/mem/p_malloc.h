#ifndef _P_MALLOC_
#define _P_MALLOC_


extern void wjq_free_m( void *ap );
extern void *wjq_malloc_m( unsigned nbytes , const char *f, int l);
extern void *wjq_realloc_m(void *mem_address, unsigned int newsize);
extern void* wjq_calloc_m(unsigned int n, unsigned int size);

//#define wjq_free(p) {wjq_free_m(p); p = 0;}
#define wjq_free(p)   wjq_free_m(p)
#define wjq_malloc(n) wjq_malloc_m(n, __FILE__, __LINE__)
#define wjq_calloc(n)  wjq_calloc_m(n)
#define wjq_realloc(x,y) wjq_realloc_m(x, y)

/** 宏定义的 接口 */
#define p_m_free(ptr)   do{if(ptr != NULL ){ wjq_free_m(ptr); ptr = NULL; }}while(0);
/** 函数定义的接口 */
#define p_f_free   wjq_free_m
#define pmalloc(n)  wjq_malloc_m(n, __FILE__, __LINE__)
#define pcalloc(n)  wjq_calloc_m(n)
#define prealloc(x,y) wjq_realloc_m(x, y)

#endif


