/*
	p_list.h 是从linux拷贝来的
	这是一个经典的实现，
	我的总结是：
		1 提出共同点出来（重复的代码/流程），做成一个模块，。
		2 看问题的角度决定代码流程。
	*/
#include "mcu.h"
#include "log.h"
#include "mem/p_list.h"


/*
测试宏offsetof功能：获取结构体成员在结构体中的偏移

struct test  
{  
        int i;  
        int j;  
        char k;
		struct list_head list;
		char*name;
};  



void list_test_offsetof(void)
{
	    struct test temp;  
  
        wjq_log(LOG_FUN, "&temp = %p\r\n", &temp);          // temp的起始地址  
        wjq_log(LOG_FUN, "&(temp.k) = %p\r\n", &(temp.k));      // temp.k的地址  
        wjq_log(LOG_FUN, "&(((struct test *)0)->k) = %ld\r\n", (size_t)&((struct test *)0)->k);       //k在temp中的偏移量  
  		while(1);
}
*/
/*定义并初始化一个链表头，空链表*/
struct list_head TestList={&TestList, &TestList};

struct test  
{  
        int i;  
        int j;  
        char k;
		struct list_head list;
		char*name;
};

struct test t1={
	.name = "tset list 1",
	}; 
struct test t2={
	.name = "tset list 2",
	}; 
struct test t3={
	.name = "tset list 3",
	}; 


void list_test(void)
{
	int res;
	
	struct test *p;

	res = list_empty(&TestList);
	if(res  == 1)
	{
		wjq_log(LOG_DEBUG, "list is empty\r\n");
	}
	
	list_add(&(t1.list), &TestList);

	p = list_entry(TestList.next,struct test, list);
	
	wjq_log(LOG_DEBUG, "name:%s\r\n", p->name);

	while(1);
	
}


