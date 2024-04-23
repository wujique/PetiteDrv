#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "mcu.h"

typedef enum {
	TP_TYPE_RTP = 1,
	TP_TYPE_CTP = 2,
}TpType;


/* 触摸样点，电容和电阻屏都用这个结构 */
typedef struct{
	int16_t x;
	int16_t y;
	
	int16_t	pressure;/* 压力 */
	int16_t index;/* 多点触摸序号，如果只有一点触摸，则是1 */
}TouchPoint;


/**
 * @brief   触摸屏设备定义
 * 
 * 
 */
typedef struct {
	char *name;

	/** 触摸屏像素 */
	uint16_t w;//X轴
	uint16_t h;//Y轴

	/* 电阻屏或电容屏*/
	TpType type;
	/* 用于匹配 驱动*/
	uint16_t icid;//

	MCU_PORT rstport;
	uint16_t rstpin;

	MCU_PORT intport;
	uint16_t intpin;

	char *basebus;

}DevTouch;


/**
 * @brief   触摸屏驱动定义
 * 
 * 
 */
typedef struct {
	char name[16];
	uint8_t type;//类型，1 rtp，2 ctp

	/* drv */
	int (*init)(const DevTouch *dev);
	int (*open)(void);
	int (*close)(void);

	void (*task)(const DevTouch *dev);
}TouchDrv;


typedef struct{
	int gd;
	uint16_t rotate;//90,180, 270, 360
	const DevTouch *dev;
	const TouchDrv *drv;
	
}NodeTouch;

#define LogTouchDrv(l,args...) petite_log(l, "touch", NULL,__FUNCTION__, __LINE__, ##args);

int tp_init(const DevTouch *TpDev);
int tp_open(char *name);
int tp_close(void);
int tp_rotate(uint16_t rotate);
int tp_get_pointxy (int16_t *x, int16_t *y);
int tp_is_press(void);
int tp_task_loop(void *userdata);

#endif
