#ifndef __ALLOC_H_
#define __ALLOC_H_

extern void *kr_create_with_pool(void* mem, size_t bytes);
extern void *kr_malloc(void *node, size_t nbytes);
extern void kr_free(void *node, void*ap);
extern void *kr_realloc(void *node, void* ptr, size_t size);
#endif

