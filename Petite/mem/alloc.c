
/*
 * File:		alloc.c
 * Purpose: 	generic malloc() and free() engine
 *
 * Notes:		99% of this code stolen/borrowed from the K&R C
 *				examples.
 *
 */
#include "stdlib.h"

/*
	单链表管理空闲内存的动态内存分配算法。

	缺点：
	1 分配时间不定，最慢则为轮询整个链表。
	2 内存精度16字节，一块内存最小16+16字节，
	  当申请小内存时，效率只有n/32，非常低。
	  比如频繁申请4字节内存，1K次，实际有效内存4K，
	  但却暂用32K内存，利用率4/32 = 12.5%
*/


/* 
内存分配方法选择，
_ALLOC_BEST_FIT_ 最适应法
_ALLOC_FIRST_FIT_ 首次适应法
*/
#define _ALLOC_BEST_FIT_	
//#define _ALLOC_FIRST_FIT_

/********************************************************************/

/*
 * This struct forms the minimum block size which is allocated, and
 * also forms the linked list for the memory space used with alloc()
 * and free().	It is padded so that on a 32-bit machine, all malloc'ed
 * pointers are 16-byte aligned.
 */
/*
	本内存分配方法最基本的结构就是下面这个结构体。
	在每一块空闲内存的头部都有一个，
	这个结构体size记录了本块内存的大小,
	ptr则连接到下一块空闲内存。
	分配内存时，从一块空闲内存切割需要的内存出去。
	在32位系统，这个结构体是16字节。
*/
typedef struct ALLOC_HDR
{
	struct 
	{
		struct ALLOC_HDR*ptr;
		unsigned int size;/*本快内存容量*/
	} s;

	unsigned int align;
	unsigned int pad;
} ALLOC_HDR;


static ALLOC_HDR base; /*空闲内存链表头结点*/
static ALLOC_HDR*freep = NULL;

static unsigned int AllocCnt = 0;

/*-------------------------------------------------------------------*/
void kr_free(void *node, void*ap)
{
	ALLOC_HDR*bp, *p;

	/* 最好的判断是不是应该判断在堆范围内？*/
	if(ap==NULL) return;

	/* 函数传入的ap是可使用内存的指针，往前退一个结构体位置，
		也就是下面的bp，才是记录内存信息的位置*/
	bp = (ALLOC_HDR*)ap-1;											/* point to block header */

	AllocCnt -= bp->s.size;

	/*
	  找到需要释放的内存的前后空闲块
	  其实就是比较内存块位置的地址大小
	*/
	for(p = freep; ! ((bp>p)&&(bp<p->s.ptr)); p = p->s.ptr) {
		if((p>=p->s.ptr)&&((bp>p)||(bp<p->s.ptr))) {
			/*
				当一个块，
				p>=p->s.ptr 本身起始地址指针大于下一块地址指针
				bp>p 要释放的块，地址大于P
				bp<p->s.ptr 要释放的块，地址小于下一块 		*/
			break;		/* freed block at start or end of arena */
		}
	}

	/*判断是否能跟一个块合并，能合并就合并，不能合并就用链表连起来*/
	if((bp+bp->s.size)==p->s.ptr) {
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	} else {
		bp->s.ptr = p->s.ptr;
	}

	/*同样处理跟后一块的关系*/
	if((p+p->s.size)==bp) {
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	} else {
		p->s.ptr = bp;
	}

	freep = p;

}


/*---------------------------------------------------------*/
void*kr_malloc(void *node, unsigned nbytes)
{

	ALLOC_HDR*p, *prevp;
	unsigned nunits;

#ifdef _ALLOC_BEST_FIT_
	ALLOC_HDR *bp = NULL;
    ALLOC_HDR *bprevp;
#endif

	/*计算要申请的内存块数*/
	nunits = ((nbytes+sizeof(ALLOC_HDR)-1) / sizeof(ALLOC_HDR))+1;

	AllocCnt += nunits;
	//wjq_log(LOG_DEBUG, "AllocCnt:%d\r\n", AllocCnt*sizeof(ALLOC_HDR));

	/*第一次使用malloc，内存链表没有建立
	  初始化链表*/
	prevp = freep;
	if(prevp == NULL) {
		return NULL;
	}

	/*查询链表，查找合适块*/
	for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {

		if(p->s.size==nunits) {
			prevp->s.ptr = p->s.ptr;
			freep = prevp;

			/*返回可用内存指针给用户，
			可用内存要出去内存块管理结构体*/
			return (void*) (p+1);
		} else if(p->s.size > nunits) {
			#ifdef _ALLOC_BEST_FIT_/*最适合法*/
			if (bp == NULL) {
                bp = p;
                bprevp = prevp;
            }
			
            if (bp->s.size > p->s.size) {
                bprevp = prevp;
                bp = p;                
            }
			#else/*首次适应法*/
			p->s.size -= nunits;
			p += p->s.size;
			p->s.size = nunits;

			freep = prevp;
			/*返回可用内存指针给用户，
			可用内存要出去内存块管理结构体*/
			return (void*) (p+1);
			#endif
		}

		/*分配失败*/
		if (p==freep) {
			#ifdef _ALLOC_BEST_FIT_
			if (bp != NULL) {
                freep = bprevp;
                p = bp;
                
                p->s.size -= nunits;
                p += p->s.size;     //P 指向将分配出去的空间
                p->s.size = nunits; //记录分配的大小，这里不用设置ptr了，因为被分配出去了

                return (void *)(p + 1); //减去头结构体才是真正分配的内存    
            }
			#endif
			
			return NULL;
		}

	}
}

void *kr_realloc(void *node, void* ptr, size_t size)
{
	//ALLOC_HDR*bp, *p, *np;
	void *p;
	unsigned nunits;
	unsigned aunits;
	
	//wjq_log(LOG_DEBUG, "wjq_realloc: %d\r\n", newsize);
		if(ptr == NULL) {
			p = kr_malloc(node, size);
			return p;	
		}
	
		if (size == 0) {
			kr_free(node, ptr);
			return NULL;
		}
	
	#if 0
		/*计算要申请的内存块数*/
		nunits = ((newsize + sizeof(ALLOC_HDR)-1) / sizeof(ALLOC_HDR))+1;
		/* 函数传入的ap是可使用内存的指针，往前退一个结构体位置，
			也就是下面的bp，才是记录内存信息的位置*/
		bp = (ALLOC_HDR*)ap-1;	/* point to block header */
		if (nunits <= bp->s.size) {
			/*	新的申请数不比原来的大，暂时不处理
			浪费点内存。		*/
			return ap;
		}
	#endif
		
	#if 1
		/*无论如何都直接申请内存然后拷贝数据*/
		p = kr_malloc(node, size);
		memcpy(p, ptr, size);/* need fix bug*/
		wjq_free_m(node, ptr);
		
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


void *kr_create_with_pool(void* mem, size_t bytes)
{
	ALLOC_HDR*p;
	
	p = (ALLOC_HDR*) mem;
	p->s.size = (bytes / sizeof(ALLOC_HDR));
	p->s.ptr =&base;
	base.s.ptr = p;
	base.s.size = 0;
	freep =&base;

	/*经过初始化后，只有一块空闲块*/
	return (void *)&base;
}


/***************************** end ***************************************/

