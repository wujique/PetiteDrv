#include "stdlib.h"
#include "log.h"
#include "mem/p_malloc.h"
#include "stimer.h"

#include "softtimer.h"

/*
        请考虑：
        1. 在回调中把自己stop
        2. 在回调中重新开始自己
        3. 在回调中删了别人
        4. 在回调中创建新定时器
*/

#if 1
/*-------------------------- 测试 ---------------------------*/
void *TestSlstLoop;

void *timer_p;
void timer_p_slst_cb_t(void *userdata)
{
    printf("cb:%s\r\n", (char *)userdata);
    slst_start(TestSlstLoop, timer_p, 20, "timer_p :10", SOFTTIMER_TYPE_PERIODIC);
}

void *timer_once;
void timer_once_slst_cb_t(void *userdata)
{
    printf("cb:%s\r\n", (char *)userdata);
    slst_start(TestSlstLoop, timer_once, SLST_MAX, "timer_once :SLST_MAX", SOFTTIMER_TYPE_ONCE);
}

void *timer_once_1;
void *timer_once_2;

void timer_once_1_slst_cb_t(void *userdata)
{
    printf("cb:%s\r\n", (char *)userdata);
    /*回调中开启自己 */
    slst_start(TestSlstLoop, timer_once_1, 60, "timer_once_1 :60", SOFTTIMER_TYPE_ONCE);
    /*回调中开启别人 */
    slst_start(TestSlstLoop, timer_once_2, 20, "timer_once_2 :20", SOFTTIMER_TYPE_ONCE);
    slst_start(TestSlstLoop, timer_p, 50, "timer_p :50", SOFTTIMER_TYPE_PERIODIC);
}


void timer_once_2_slst_cb_t(void *userdata)
{
    printf("cb:%s\r\n", (char *)userdata);
    
    /* 回调中删除自己 */
    slst_stop(TestSlstLoop, timer_once_2);
    /* 回调中删除别人*/
    slst_stop(TestSlstLoop, timer_p);
}

void slstimer_test(void)
{
    TestSlstLoop = slst_create_loop();
    timer_p = slst_create("period", timer_p_slst_cb_t);
    timer_once = slst_create("once_0", timer_once_slst_cb_t);
    timer_once_1 = slst_create("once_1", timer_once_1_slst_cb_t);
    timer_once_2 = slst_create("once_2", timer_once_2_slst_cb_t);
    /* 测试案例：一开始就定义一个 SLST_MAX 的定时器*/
    slst_start(TestSlstLoop, timer_once, SLST_MAX, "timer_once :SLST_MAX", SOFTTIMER_TYPE_ONCE);
    
    //slst_start(TestSlstLoop, timer_p, 50, "timer_p :50", SOFTTIMER_TYPE_PERIODIC);
    //slst_start(TestSlstLoop, timer_once_1, 60, "timer_once_1 :60", SOFTTIMER_TYPE_ONCE);
    //slst_start(TestSlstLoop, timer_once_2, 20, "timer_once_2 :20", SOFTTIMER_TYPE_ONCE);

    /* 定义一个 SLST_MAX /10 的周期定时器，然后定义一个 SLST_MAX 的定时器， 在SLST_MAX定时器中重新开始SLST_MAX*/

    /* 定义一个 1/10 周期定时器，在第三次时把自己stop，*/

    while(1) {
        slst_task(TestSlstLoop);
    }
}

#endif
