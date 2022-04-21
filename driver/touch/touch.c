#include "mcu.h"
#include "petite.h"
#include "petite_config.h"
#include "board_sysconf.h"

#include "touch.h"
#include "log.h"
#include "tslib.h"


#define DEV_TS_DEBUG

#ifdef DEV_TS_DEBUG
#define TS_DEBUG	wjq_log 
#else
#define TS_DEBUG(a, ...)
#endif

int16_t TouchPointBufW = 0;
int16_t TouchPointBufR = 0;

#define TouchPointBufSize 1024
TouchPoint TouchPointBuf[TouchPointBufSize];

const TouchDev *gdTpDev = NULL;


extern int gt1151_task(void);
extern int gt9147_task(void);

void cap_touch_task_idle(void) 
{
	return;
}

void (*tp_task)(void) = cap_touch_task_idle;
//void (*cap_touch_task)(void) = NULL;


/*
	touch buff have data return 1
	
	*/
int tp_is_press(void)
{
	if (TouchPointBufW != TouchPointBufR) return 1;	
	else return 0;
}

/*
	底层将触摸点填入缓冲
	*/
int ctp_fill_buf(int16_t x, int16_t y)
{
	TouchPointBuf[TouchPointBufW].x = x;
	TouchPointBuf[TouchPointBufW].y = y;
	TouchPointBufW++;
	if (TouchPointBufW >= TouchPointBufSize) TouchPointBufW = 0;
	//uart_printf("w: x=%d, y=%d\r\n",  x, y);
	return 1;
}
int ctp_get_point(TouchPoint *samp, int nr)
{
	int i = 0;
	TouchPoint *p;
	
	p = samp;

	while(i < nr) {
		if (TouchPointBufW == TouchPointBufR) break;

		p->x = TouchPointBuf[TouchPointBufR].x;
		p->y = TouchPointBuf[TouchPointBufR].y;
	
		//uart_printf("r: x=%d, y=%d\r\n",  *x, *y);
	
		TouchPointBufR++;
		if (TouchPointBufR >= TouchPointBufSize) TouchPointBufR = 0;
		
		p++;
		i++;

	}
	
	return i;
}



/**
 *@brief:      rtp_fill_buff
 *@details:    将样点写入缓冲，底层调用
 *@param[in]   struct ts_sample *samp  
               int nr                  
 *@param[out]  无
 *@retval:     
 */
s32 rtp_fill_buff(TouchPoint *samp, int nr)
{
	int index;
	TouchPoint *p = samp;
	
	index = 0;
	while(1) {
		if(index >= nr) break;	
		
		TouchPointBuf[TouchPointBufW].pressure = p->pressure;//压力要搞清楚怎么计算
		TouchPointBuf[TouchPointBufW].x = p->x;
		TouchPointBuf[TouchPointBufW].y = p->y;
		TouchPointBufW++;
		if (TouchPointBufW >=  TouchPointBufSize) TouchPointBufW = 0;
		
		p++;
		index++;
	}
		
	return index;
}

/**
 *@brief:        
 *@details:    读触摸屏采样原始样点，应用层或者tslib调用
 *@param[in]   struct ts_sample *samp  
               int nr                  
 *@param[out]  无
 *@retval:     
 */
s32 tslib_read_rtp(struct ts_sample *samp, int nr)
{
	int i = 0;
	struct ts_sample *p;
	p = samp;
	
	while(1) {
		if(i>=nr) break;

		if(TouchPointBufW ==  TouchPointBufR) break;

		p->pressure = TouchPointBuf[TouchPointBufR].pressure;
		p->x = TouchPointBuf[TouchPointBufR].x;
		p->y = TouchPointBuf[TouchPointBufR].y;
		
		TouchPointBufR++;
		if(TouchPointBufR >= TouchPointBufSize) TouchPointBufR = 0;
		p++;
		i++;
	}

	return i;
}

int rtp_get_point(TouchPoint *rtpsamp, int nr)
{
	int i = 0;
	int res;
	TouchPoint *p;
	
	struct ts_sample samp;
	p = rtpsamp;
	
	/* 电阻屏*/
	struct tsdev *ts;
	while(i < nr) {
		res = ts_read(ts, &samp, 1);
		if (res == 1) {
			
			p->pressure = samp.pressure;
			p->x = samp.x;
			p->y = samp.y;
			//uart_printf("[%d %d] ", samp.x, samp.y);
			p++;
			i++;
		} else return i;
	}
	
	return i;
}

/*----------------------------------------------------------------------------*/
/**
 *@brief:      dev_touchscreen_init
 *@details:    
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
int tp_init(const TouchDev *TpDev)
{
	int res;

	if (TpDev == NULL) return -1;
	
	uart_printf("init tp:%s\r\n", TpDev->name);
	res = TpDev->init();
	if (res == 0) {
		tp_task = TpDev->task;
		gdTpDev = TpDev;
		return 0;
	}

	return -1;
}


int tp_open(void)
{
	int res;
	
	res = gdTpDev->open();	

	/* 电阻屏，需要用到 tslib */
	struct tsdev *ts;
	if (gdTpDev->type == 1) {
		ts = ts_open_module();
	}

	return res;
}

int tp_close(void)
{
	gdTpDev->close();
	return 0;
}
/*
	
	*/

int tp_get_point(TouchPoint *samp, int nr)
{
	if (gdTpDev->type == 2) {
		return ctp_get_point(samp, nr);
	} else if (gdTpDev->type == 1) {
		return rtp_get_point(samp, nr);
	} else return 0;
}

int tp_get_pointxy (int16_t *x, int16_t *y)
{
	int res;
	
	TouchPoint samp;
	
	res = tp_get_point(&samp, 1);

	if (res == 1) {
		*x = samp.x;
		*y = samp.y;
		return 1;
	}

	return 0;
}

