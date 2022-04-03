#ifndef __TOUCH_H__
#define __TOUCH_H__


/* 触摸样点，电容和电阻屏都用这个结构 */
typedef struct{
	int16_t x;
	int16_t y;
	
	int16_t	pressure;/* 压力 */
	int16_t index;/* 多点触摸序号，如果只有一点触摸，则是1 */
}TouchPoint;


typedef struct {
	char name[16];
	uint8_t type;//类型，1 rtp，2 ctp

	/* drv */
	int (*init)(void);
	int (*open)(void);
	int (*close)(void);

	void (*task)(void);
}TouchDev;


#endif
