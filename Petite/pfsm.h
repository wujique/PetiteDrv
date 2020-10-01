#ifndef __PFSM_H__
#define __PFSM_H__

/*  状态节点，包含两部分：
		当前状态和回调函数。
		可切换状态表。
	*/
typedef struct _strFsmNode{
	/* 当前 状态*/
	uint32_t nowsta;
	uint32_t event;
	/*可切换状态列表*/
	uint32_t newsta;
	/* 动作 */
	void (*action)(void);

}FsmNode;

typedef struct _strFsmHandle{
	char *fname;
	uint32_t nowsta;
	uint32_t len;
	FsmNode *node;
	}FsmHandle;

#endif

