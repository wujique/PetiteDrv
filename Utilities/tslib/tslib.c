/*
 *  tslib
 *
 *  Copyright (C) 2003 Chris Larson.
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tslib.h"

/**
 *@brief:      ts_input_read
 *@details:    读触摸屏样点数据
 *@param[in]                          
 *@param[out]  无
 *@retval:     static
 */
static int ts_input_read(struct ts_sample *samp, int nr)
{
	int ret = nr;
	
	ret = dev_touchscreen_read(samp, nr);

	return ret;
}
/*-------------------------------------------------------------------------------*/
struct tslib_linear 
{
	int	swap_xy;

// Linear scaling and offset parameters for pressure
	int	p_offset;
	int	p_mult;
	int	p_div;

// Linear scaling and offset parameters for x,y (can include rotation)
	int	a[7];
};
struct tslib_linear TslibLinear;

/**
 *@brief:      linear_read
 *@details:    校准
 *@param[in]   struct tslib_module_info *info  
               struct ts_sample *samp          
               int nr                          
 *@param[out]  无
 *@retval:     static
 */
static int linear_read(struct ts_sample *samp, int nr)
{
	struct tslib_linear *lin = &TslibLinear;
	
	int ret;
	int xtemp,ytemp;

	// 从下一层读取样点
	ret =  ts_input_read(samp, nr);
	//读到ret个样点
	if (ret >= 0) 
	{
		int nr;//重新申请一个nr变量?有必要也叫nr吗？让人误解

		for (nr = 0; nr < ret; nr++, samp++) 
		{
			xtemp = samp->x; ytemp = samp->y;

			samp->x = 	( lin->a[2] +
					lin->a[0]*xtemp + 
					lin->a[1]*ytemp ) / lin->a[6];
			
			samp->y =	( lin->a[5] +
					lin->a[3]*xtemp +
					lin->a[4]*ytemp ) / lin->a[6];
			
			samp->pressure = ((samp->pressure + lin->p_offset)
						 * lin->p_mult) / lin->p_div;

			/*XY轴对调*/
			if (lin->swap_xy) 
			{
				int tmp = samp->x;
				samp->x = samp->y;
				samp->y = tmp;
			}
		}
	}

	return ret;
}

calibration TsCalSet;

static int mod_linear_init(void)
{

	struct tslib_linear *lin;
	
	wjq_log(LOG_DEBUG, "mod_linear_init\r\n");

	lin = &TslibLinear;

	/*  下面四个数据不会修改？  */
	lin->p_offset = 0;
	lin->p_mult   = 1;
	lin->p_div    = 1;
	lin->swap_xy  = 0;

	/*
	 以下内容就是获取系统的校准数据
	 */
	lin->a[0] = TsCalSet.a[0];
	lin->a[1] = TsCalSet.a[1];
	lin->a[2] = TsCalSet.a[2];
	lin->a[3] = TsCalSet.a[3];
	lin->a[4] = TsCalSet.a[4];
	lin->a[5] = TsCalSet.a[5];
	lin->a[6] = TsCalSet.a[6];
		
	wjq_log(LOG_DEBUG, "mod linear init ok\r\n");
	
	return 1;
}

/*-------------------------------------------------------------------------------*/
#define VAR_PENDOWN 		0x00000001
#define VAR_LASTVALID		0x00000002
#define VAR_NOISEVALID		0x00000004
#define VAR_SUBMITNOISE 	0x00000008

struct tslib_variance 
{
	int delta;
    struct ts_sample last;	//上一个样点
    struct ts_sample noise;//噪声（可疑）
	unsigned int flags;
};

struct tslib_variance TsVariance;

static int sqr (int x)
{
	return x * x;
}
/**
 *@brief:      variance_read
 *@details:    滤波算法插件
 				处理飞点
 *@param[in]   struct tslib_module_info *info  
               struct ts_sample *samp          
               int nr                          
 *@param[out]  无
 *@retval:     static
 */
static int variance_read(struct ts_sample *samp, int nr)
{
	struct tslib_variance *var = &TsVariance;
	
	struct ts_sample cur;
	int count = 0, dist;

	while (count < nr) 
	{
		/*
			如果N+1、N+2两个点跟N点的距离超出阀值，则可能是快速移动，
			这时会将VAR_SUBMITNOISE标识置起，
			置这个标识是为了下一个循环判断N+1跟N+2之间是否也超出阀值，
		*/
		if (var->flags & VAR_SUBMITNOISE) 
		{
			cur = var->noise;////不重新获取新样点，而是将noise中的样点作为当前样点
			var->flags &= ~VAR_SUBMITNOISE;
		} 
		else 
		{
			/*  
			读取一个新样点
			*/
			if (linear_read(&cur, 1) < 1)
				return count;
		}

		if (cur.pressure == 0)//样点压力值为0, 
		{
			/* Flush the queue immediately when the pen is just
			 * released, otherwise the previous layer will
			 * get the pen up notification too late. This 
			 * will happen if info->next->ops->read() blocks.
			 */
			if (var->flags & VAR_PENDOWN) 
			{
				/*上一点pressure不为0，会将VAR_PENDOWN 标识置起，
				这次压力为0，说明可能是提笔，可能而已，也可能是个噪声样点
				先保存到noise*/
				var->flags |= VAR_SUBMITNOISE;
				var->noise = cur;//先将样点保存到noise
			}
			/* Reset the state machine on pen up events. */
			var->flags &= ~(VAR_PENDOWN | VAR_NOISEVALID | VAR_LASTVALID);
			goto acceptsample;
		} else
			var->flags |= VAR_PENDOWN;

		if (!(var->flags & VAR_LASTVALID)) 
		{
			var->last = cur;
			var->flags |= VAR_LASTVALID;
			continue;
		}

		if (var->flags & VAR_PENDOWN) {
			/* Compute the distance between last sample and current */
			dist = sqr (cur.x - var->last.x) +
			       sqr (cur.y - var->last.y);

			if (dist > var->delta) {
				//uart_printf("%d-",dist);
				/* 
				Do we suspect the previous sample was a noise? 
				
				上一个样点如果也超出阈值，就会将VAR_NOISEVALID置位，
				这次的样点又超出阈值，那么就可以认为是快速移动
				*/
				if (var->flags & VAR_NOISEVALID) {
					//uart_printf("q-");	
					/* Two "noises": it's just a quick pen movement */
					samp [count++] = var->last = var->noise;
					
					var->flags = (var->flags & ~VAR_NOISEVALID) |
									VAR_SUBMITNOISE;
				} else{
					/*第一次超出阈值，置位标志*/
					var->flags |= VAR_NOISEVALID;
				}
				/* The pen jumped too far, maybe it's a noise ... */
				var->noise = cur;
				continue;
			} else{
				//uart_printf("g ");
				var->flags &= ~VAR_NOISEVALID;
			}
		}

acceptsample:
		samp [count++] = var->last;
		var->last = cur;
	}
	
	return count;
}


static int mod_variance_init(void)
{
	struct tslib_variance *var;

	wjq_log(LOG_DEBUG, "mod_variance_init\r\n");

	var = &TsVariance;

	if (var == NULL)
		return NULL;

	var->delta = 10;
	var->flags = 0;

    var->delta = sqr (var->delta);

	return 1;
}

/*-------------------------------------------------------------------------------*/

/**
 * This filter works as follows: we keep track of latest N samples,
 * and average them with certain weights. The oldest samples have the
 * least weight and the most recent samples have the most weight.
 * This helps remove the jitter and at the same time doesn't influence
 * responsivity because for each input sample we generate one output
 * sample; pen movement becomes just somehow more smooth.
 */

#define NR_SAMPHISTLEN	4

/* To keep things simple (avoiding division) we ensure that
 * SUM(weight) = power-of-two. Also we must know how to approximate
 * measurements when we have less than NR_SAMPHISTLEN samples.
 */
static const unsigned char weight [NR_SAMPHISTLEN - 1][NR_SAMPHISTLEN + 1] =
{
	/* The last element is pow2(SUM(0..3)) */
	{ 5, 3, 0, 0, 3 },	/* When we have 2 samples ... */
	{ 8, 5, 3, 0, 4 },	/* When we have 3 samples ... */
	{ 6, 4, 3, 3, 4 },	/* When we have 4 samples ... */
};

struct ts_hist {
	int x;
	int y;
	unsigned int p;
};

struct tslib_dejitter 
{
	int delta;
	int x;
	int y;
	int down;
	int nr;
	int head;
	struct ts_hist hist[NR_SAMPHISTLEN];
};

struct tslib_dejitter TsDejitter;

static void average (struct tslib_dejitter *djt, struct ts_sample *samp)
{
	const unsigned char *w;
	int sn = djt->head;
	int i, x = 0, y = 0;
	unsigned int p = 0;

    w = weight [djt->nr - 2];

	for (i = 0; i < djt->nr; i++) {
		x += djt->hist [sn].x * w [i];
		y += djt->hist [sn].y * w [i];
		p += djt->hist [sn].p * w [i];
		sn = (sn - 1) & (NR_SAMPHISTLEN - 1);
	}

	samp->x = x >> w [NR_SAMPHISTLEN];
	samp->y = y >> w [NR_SAMPHISTLEN];
	samp->pressure = p >> w [NR_SAMPHISTLEN];
	
}
/**
 *@brief:      dejitter_read
 *@details:    触摸屏去噪算法插件
 			   （均值平滑滤波）
 *@param[in]   struct tslib_module_info *info  
               struct ts_sample *samp          
               int nr                          
 *@param[out]  无
 *@retval:     static
 */
static int dejitter_read(struct ts_sample *samp, int nr)
{
    struct tslib_dejitter *djt = &TsDejitter;
	struct ts_sample *s;
	int count = 0, ret;

	ret = variance_read(samp, nr);
	
	for (s = samp; ret > 0; s++, ret--) 
	{
		if (s->pressure == 0) 
		{
			/*
			 * Pen was released. Reset the state and
			 * forget all history events.
			 */
			djt->nr = 0;
			samp [count++] = *s;
                        continue;
		}

        /* If the pen moves too fast, reset the backlog. */
		if (djt->nr) 
		{
			int prev = (djt->head - 1) & (NR_SAMPHISTLEN - 1);
			if (sqr (s->x - djt->hist [prev].x) +
			    sqr (s->y - djt->hist [prev].y) > djt->delta) 
			{
                djt->nr = 0;
			}
		}

		djt->hist[djt->head].x = s->x;
		djt->hist[djt->head].y = s->y;
		djt->hist[djt->head].p = s->pressure;
		
		if (djt->nr < NR_SAMPHISTLEN)
			djt->nr++;

		/* We'll pass through the very first sample since
		 * we can't average it (no history yet).
		 */
		if (djt->nr == 1)
			samp [count] = *s;
		else 
		{
			average (djt, samp + count);
			//samp [count].tv = s->tv;
		}
		count++;
		/*
		巧妙的处理环形缓冲索引循环问题，不过只有当缓冲个数是2的N次方是才可以这样，
		这种方法的根本原理是通过与操作清掉进位。
		*/
		djt->head = (djt->head + 1) & (NR_SAMPHISTLEN - 1);
	}

	return count;
}


static int mod_dejitter_init(void)
{
	struct tslib_dejitter *djt;

	wjq_log(LOG_DEBUG, "mod_dejitter_init\r\n");

	djt = &TsDejitter;

	memset(djt, 0, sizeof(struct tslib_dejitter));
	
	djt->delta = 100;
    djt->head = 0;

	djt->delta = sqr(djt->delta);

	return 1;
}

/*---------------------------------对外接口-----------------------------------------------*/
/**
 *@brief:      ts_open
 *@details:    打开一个TS设备
 *@param[in]   const char *name  
               int nonblock      
 *@param[out]  无
 *@retval:     struct

 */
struct tsdev *ts_open(const char *name, int nonblock)
{
	mod_dejitter_init();
	mod_variance_init();
	mod_linear_init();
	return (struct tsdev *)1;
}

int ts_read(struct tsdev *ts, struct ts_sample *samp, int nr)
{
	int result;

	result = dejitter_read(samp, nr);

	return result;

}

int ts_config(struct tsdev *ts)
{
	return 0;	
}

/*
	直接读样点接口，样点不经过TSLIB处理
*/
int ts_read_raw(struct tsdev *ts, struct ts_sample *samp, int nr)
{
	int result = ts_input_read(samp, nr);

	return result;
}
/*
	设置校准参数
*/
int ts_set_cal(calibration *CalSet)
{
	memcpy(&TsCalSet, CalSet, sizeof(calibration));		
}

