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
#define TouchPointBufSize 256
TouchPoint TouchPointBuf[TouchPointBufSize];

/*
	底层将触摸点填入缓冲
	*/
int ctp_fill_buf(int16_t x, int16_t y)
{
	TouchPointBuf[TouchPointBufW].x = x;
	TouchPointBuf[TouchPointBufW].y = y;
	TouchPointBuf[TouchPointBufW].pressure = 100;
	TouchPointBuf[TouchPointBufW].index = 1;
	TouchPointBufW++;
	if (TouchPointBufW >= TouchPointBufSize) TouchPointBufW = 0;
	//LogTouchDrv(LOG_INFO, "w: x=%d, y=%d\r\n",  x, y);
	return 1;
}

/*---------------------------电阻屏样点处理----------------------------------------*/
int16_t TPOriBufW = 0;
int16_t TPOriBufR = 0;
#define TPOriBufSize 256
TouchPoint TPOriBuf[TPOriBufSize];

/**
 *@brief:      rtp_fill_buff
 *@details:    将样点写入缓冲，底层调用
 *@param[in]   struct ts_sample *samp  
               int nr                  
 *@param[out]  无
 *@retval:     
 */
int rtp_fill_buf(int16_t x, int16_t y, int16_t	pressure)
{
	TouchPointBuf[TouchPointBufW].x = x;
	TouchPointBuf[TouchPointBufW].y = y;
	TouchPointBuf[TouchPointBufW].pressure = pressure;
	TouchPointBuf[TouchPointBufW].index = 1;
	TouchPointBufW++;
	if (TouchPointBufW >= TouchPointBufSize) TouchPointBufW = 0;
	//LogTouchDrv(LOG_INFO, "w: x=%d, y=%d\r\n",  x, y);
	return 1;
}

s32 rtp_fill_ori_buff(TouchPoint *samp, int nr)
{
	int index;
	TouchPoint *p = samp;
	
	index = 0;
	while(1) {
		if(index >= nr) break;	
		
		TPOriBuf[TPOriBufW].pressure = p->pressure;//压力要搞清楚怎么计算
		TPOriBuf[TPOriBufW].x = p->x;
		TPOriBuf[TPOriBufW].y = p->y;
		TPOriBufW++;
		if (TPOriBufW >=  TouchPointBufSize) TPOriBufW = 0;
		
		p++;
		index++;
	}
		
	return index;
}

/**
 *@brief:        
 *@details:    读触摸屏采样原始样点，tslib调用
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

		if(TPOriBufW ==  TPOriBufR) break;

		p->pressure = TPOriBuf[TPOriBufR].pressure;
		p->x = TPOriBuf[TPOriBufR].x;
		p->y = TPOriBuf[TPOriBufR].y;
		
		TPOriBufR++;
		if(TPOriBufR >= TouchPointBufSize) TPOriBufR = 0;
		p++;
		i++;
	}

	return i;
}

void rtp_tslib_deal_point(void)
{

	int res;
	struct ts_sample samp;
	struct tsdev *ts;

	res = ts_read(ts, &samp, 1);
	if (res == 1) {
		if (samp.pressure == 0) {
			//uart_printf("-0-");
			return;
		}

		rtp_fill_buf(samp.x, samp.y, samp.pressure);
	} 


}

/*----------------------------------------------------------------------------*/
//extern const TouchDrv CtpGt9147;
extern const TouchDrv CtpGt1151;
extern const TouchDrv RtpXpt2046;


NodeTouch TouchDevNode;

/**
 *@brief:      dev_touchscreen_init
 *@details:    
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
int tp_init(const DevTouch *TpDev)
{
	int res;

	if (TpDev == NULL) return -1;
	
	LogTouchDrv(LOG_INFO, "init tp:%s\r\n", TpDev->name);

	TouchDevNode.dev = TpDev;

	if (TpDev->icid == 0x1158) {
		TouchDevNode.drv = &CtpGt1151;

	} else if (TpDev->icid == 0x9147) {
		//TouchDevNode.drv = &CtpGt9147;

	} else if (TpDev->icid == 0x2046) {
		TouchDevNode.drv = &RtpXpt2046;

	} else {
		return -1;
	}

	const TouchDrv *drv = TouchDevNode.drv;

	res = drv->init(TpDev);
	TouchDevNode.rotate = 0;
	return res;
}

/**
 * @brief   描述
 * 
 * @param   name        参数描述
 * @return  int 
 * 
 */
int tp_open(char *name)
{
	int res;
	const TouchDrv *drv = TouchDevNode.drv;

	res = drv->open();	

	/* 电阻屏，需要用到 tslib */
	struct tsdev *ts;
	if (drv->type == TP_TYPE_RTP) {
		ts = ts_open_module();
	}

	return res;
}

int tp_close(void)
{
	const TouchDrv *drv = TouchDevNode.drv;

	drv->close();
	return 0;
}

int tp_rotate(uint16_t rotate)
{
	TouchDevNode.rotate = rotate;
}


int tp_get_point(TouchPoint *samp, int nr)
{
	int i = 0;
	TouchPoint *p;
	
	p = samp;

	while(i < nr) {
		if (TouchPointBufW == TouchPointBufR) break;

		p->x = TouchPointBuf[TouchPointBufR].x;
		p->y = TouchPointBuf[TouchPointBufR].y;
	
		//LogTouchDrv(LOG_INFO, "r: x=%d, y=%d\r\n",  p->x, p->y);
	
		TouchPointBufR++;
		if (TouchPointBufR >= TouchPointBufSize) TouchPointBufR = 0;
		
		p++;
		i++;

	}
	
	return i;
}
#if 0
static int tp_get_point(TouchPoint *samp, int nr)
{
	const TouchDrv *drv = TouchDevNode.drv;

	if (drv->type == TP_TYPE_CTP) {
		return ctp_get_point(samp, nr);
	} else if (drv->type == TP_TYPE_RTP) {
		return ctp_get_point(samp, nr);
	} else return 0;
}
#endif

int tp_get_pointxy (int16_t *x, int16_t *y)
{
	int res;
	
	TouchPoint samp;
	
	uint16_t w;//X轴
	uint16_t h;//Y轴

	w = TouchDevNode.dev->w;
	h = TouchDevNode.dev->h;

	res = tp_get_point(&samp, 1);

	if (res == 1) {

		/* 旋转 */
		if (TouchDevNode.rotate == 90) {
			*x = samp.y;
			*y =  w - samp.x;
		} else if (TouchDevNode.rotate == 180) {
			//*x = 480 - samp.y;
			//*y =  samp.x;
		} else if (TouchDevNode.rotate == 270) {
			*x = h - samp.y;
			*y =  samp.x;
		} else {
			*x = samp.x;
			*y = samp.y;
		}
		//uart_printf("(%d-%d)", *x, *y);
		return 1;
	}
	//uart_printf("e\r\n");
	return 0;
}

/**
 * @brief   判断是否有按下
 * 
 * @return  int 
 * 
 */
int tp_is_press(void)
{
	if (TouchPointBufW != TouchPointBufR) return 1;	
	else return 0;
}

static uint16_t touch_task_cnt = 0;

int tp_task_loop(uint8_t tick)
{
		
	touch_task_cnt += tick;
	if (touch_task_cnt >= 30) {
		touch_task_cnt = 0;
		
		const TouchDrv *drv = TouchDevNode.drv;
		if (drv->task != NULL) {
			drv->task(TouchDevNode.dev);
		}
		
	}

}