#ifndef _P_MALLOC_
#define _P_MALLOC_


extern void wjq_free_m( void *ap );
extern void *wjq_malloc_m( unsigned nbytes , const char *f, int l);
extern void *wjq_realloc_m(void *mem_address, unsigned int newsize);
extern void* wjq_calloc_m(unsigned int n, unsigned int size);

#define wjq_free(ptr)               wjq_free_m(ptr)
#define wjq_malloc(size)            wjq_malloc_m(size, __FUNCTION__, __LINE__)
#define wjq_calloc(nitems, size)    wjq_calloc_m(nitems, size)
#define wjq_realloc(ptr, size)      wjq_realloc_m(ptr, size)

/** 宏定义的 接口 */
#define p_m_free(ptr)   do{ if(ptr != NULL ){ wjq_free_m(ptr); ptr = NULL; }}while(0);
/** 函数定义的接口 */
#define pfree              wjq_free_m
#define pmalloc(size)           wjq_malloc_m(size, __FUNCTION__, __LINE__)
#define pcalloc(nitems, size)   wjq_calloc_m(nitems, size)
#define prealloc(ptr, size)     wjq_realloc_m(ptr, size)

#endif


