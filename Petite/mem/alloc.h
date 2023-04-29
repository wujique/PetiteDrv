#ifndef __ALLOC_H_
#define __ALLOC_H_

int kr_alloc_create_with_pool(void* mem, size_t bytes);
void*kr_malloc(unsigned nbytes);
void kr_free(void*ap);



#endif

