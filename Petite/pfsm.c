/*
	一个很 直白 简单 粗暴 的状态机 
*/
#include "mcu.h"
#include "petite_config.h"
#include "board_sysconf.h"
#include "log.h"

#include "pfsm.h"


/*  用法 ：
	0 用枚举定义状态和事件
	1 用FsmNode定义状态表
	2 用FsmHandle定义状态机
*/
/*
	状态机处理事件
*/
int pfsm_event(FsmHandle *Fsm, uint32_t event)
{
	uint32_t i;
	FsmNode *node;
	int res = -1;
	
	//uart_printf("fsm:%s, sta:%d, event:%d\r\n", 
	//			Fsm->fname, Fsm->nowsta, event);

	i = 0;
	while(1) {
		if (i >=  Fsm->len) {
			uart_printf("fsm err!\r\n");
			break;
		}
		node = Fsm->node + i;
		
		if (node->nowsta == Fsm->nowsta
			&& node->event == event) {
			//uart_printf("pfsm run %d\r\n", i);
			if ( node->action != NULL) {
				node->action();	
			}
			Fsm->nowsta = node->newsta;
			res = 0;
			break;
		}
			
		i++;
	}

	return res;
}

#if 0



#endif

