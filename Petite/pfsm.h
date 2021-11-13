#ifndef __PFSM_H__
#define __PFSM_H__

/*  状态切换节点 
	在nowsta状态下，发生event，
	则执行action函数，并将状态切到newsta
	*/
typedef struct _strFsmNode{
	/* 当前 状态*/
	uint32_t nowsta;
	/* 时间 */
	uint32_t event;
	/*新状态*/
	uint32_t newsta;
	/* 动作 */
	void (*action)(void);

}FsmNode;

/* 状态机 */
typedef struct _strFsmHandle{
	/* 名称 */
	char *fname;
	/* 当前状态 */
	uint32_t nowsta;
	/* 状态表切换表FsmNode数*/
	uint32_t len;
	/* 状态切换表指针*/
	FsmNode *node;
	}FsmHandle;

#endif

