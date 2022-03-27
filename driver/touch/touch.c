#include "mcu.h"
#include "petite.h"



typedef enum {
	TouchTpeNull = 0,
	TouchTpeGT9147,
	TouchTpeGT1151,
}eTouchType;


static eTouchType TouchType = -1;

extern int gt1151_task(void);
extern int gt9147_task(void);

void cap_touch_task_idle(void) 
{
	return;
}

void (*cap_touch_task)(void) = cap_touch_task_idle;

/*
	初始化触摸屏驱动，根据配置选择哪种触摸屏
	并且插入触摸屏轮询任务
	*/
int cap_touch_init(void)
{
	int res;

	res = gt9147_init();	
	if (res == 0) {
		TouchType = TouchTpeGT9147;	
		cap_touch_task = gt9147_task;
		return 0;
	}

	res = gt1151_init();	
	if (res == 0) {
		TouchType = TouchTpeGT1151;	
		cap_touch_task = gt1151_task;
		return 0;
	}

	return -1;
}

/* 触摸样点，电容和电阻屏都用这个结构 */
typedef struct{
	int16_t index;/* 多点触摸序号，如果只有一点触摸，则是1 */
	int16_t x;
	int16_t y;
	int16_t	pressure;/* 压力 */
}TouchPoint;

int16_t TouchPointBufW = 0;
int16_t TouchPointBufR = 0;

#define TouchPointBufSize 1024
TouchPoint TouchPointBuf[TouchPointBufSize];

/*
	touch buff have data return 1
	
	*/
int tp_is_press(void)
{
	if (TouchPointBufW != TouchPointBufR) return 1;	
	else return 0;
}


int tp_fill_buf(int16_t x, int16_t y)
{
	TouchPointBuf[TouchPointBufW].x = x;
	TouchPointBuf[TouchPointBufW].y = y;
	TouchPointBufW++;
	if (TouchPointBufW >= TouchPointBufSize) TouchPointBufW = 0;
	//uart_printf("w: x=%d, y=%d\r\n",  x, y);
	return 1;
}

int tp_get_point(int16_t *x, int16_t *y)
{
	//uart_printf("r..");
	if (TouchPointBufW == TouchPointBufR) return 0;

	*x = TouchPointBuf[TouchPointBufR].x;
	*y = TouchPointBuf[TouchPointBufR].y;
	
	//uart_printf("r: x=%d, y=%d\r\n",  *x, *y);
	
	TouchPointBufR++;
	if (TouchPointBufR >= TouchPointBufSize) TouchPointBufR = 0;
	
	return 1;
}


