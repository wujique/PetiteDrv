

#include "stdlib.h"
#include "log.h"
#include "mem/p_malloc.h"
#include "stimer.h"

#include "softtimer.h"

struct slstimer_s;

/**
 * @brief   定时器定义
 * 
 * @note    溢出节点：32位变量，单位ms 
 *    
 */
typedef struct slstimer_s
{
    /* pub data */
    char name[16];//方便调试
    softtimer_cb_t cb;
    void *userdata;

    /* pri */
    SOFTTIMER_V timeout;//超时时间点
    SOFTTIMER_V period;
    SoftTimerType_e type;//0, once; 1 period;


    /* 双向非循环链表 */
    struct slstimer_s *pre;
    struct slstimer_s *next;
    //slstloop_t *loop;//归属loop

}slstimer_t;

/**
 * @brief   SLST  loop
 * 
 * @note    支持多个loop
 */
typedef struct slstloop_s
{
    SOFTTIMER_V last;//上次loop时间点
    SOFTTIMER_V curtime;//当前loop时间

    /* 当前链表 */
    struct slstimer_s root;
    /* 溢出链表 */
    struct slstimer_s olist;
}slstloop_t;


/*------------------------------------------------------------*/

static void slst_list_lock(slstloop_t *slstloop_P)
{

}

static void slst_list_unlock(slstloop_t *slstloop_P)
{
    
}

/**
 * @brief   将定时器加入指定链表
 * 
 * @param   timer       参数描述
 * @return  int 
 * 
 * @note    有序插入
 */
static int slst_list_add(slstimer_t *head, slstimer_t *timer)
{

    slstimer_t *ptimer = head;
    slstimer_t *pre;

    LogSoftTimerD("add:%s\r\n", timer->name);

    while(1) {
        //LogSoftTimerD("add pre:%s\r\n", ptimer->name);
        if (ptimer->next == NULL) {
            ptimer->next = timer;
            timer->pre = ptimer;
            break;
        } else {
            ptimer = ptimer->next;
        }
        //LogSoftTimerD("timer:%s-%d, ptimer:%s-%d\r\n", timer->name, timer->timeout, ptimer->name, ptimer->timeout);
        if ( ptimer->timeout >= timer->timeout ) {

            pre = ptimer->pre;
            
            //LogSoftTimerD("pre:%s, timer:%s, next:%s\r\n", pre->name, timer->name, ptimer->name);
            
            pre->next = timer;

            timer->pre = pre;
            timer->next =  ptimer;
            
            ptimer->pre = timer;
            
            break;  
        }
   
    }

}

static int slst_list_del(slstimer_t *timer)
{
    slstimer_t *pre;
    slstimer_t *next;

    LogSoftTimerD("del:%s\r\n", timer->name);
    if (timer->pre == NULL) {
        LogSoftTimerW("no in list!\r\n");
        return 1;
    }
    pre = timer->pre;
    next = timer->next;
    //LogSoftTimerD("del pre:%s\r\n", pre->name);
    //LogSoftTimerD("del timer:%s\r\n", timer->name);
    if(next != NULL){
        //LogSoftTimerD("next pre:%s\r\n", next->name);
    }
        
    pre->next = timer->next;
    if(next != NULL) {
        next->pre = pre;
    }
    timer->pre = NULL;
    timer->next = NULL;
    return 0;
}

static void slst_show_loop_timer_list(slstloop_t *slstloop_P)
{
    slstimer_t *slstimer;

    LogSoftTimerD("\r\n----root----*/\r\n");
    slstimer = &slstloop_P->root;
    while(1) {
        LogSoftTimerD("[%s:%d]", slstimer->name, slstimer->timeout);
        if (slstimer->next == NULL) break;
        slstimer = slstimer->next;
    }  
    LogSoftTimerD("\r\n----olist----\r\n");
    slstimer = &slstloop_P->olist;
    while(1) {
        LogSoftTimerD("[%s:%d]", slstimer->name, slstimer->timeout);
        if (slstimer->next == NULL) break;
        slstimer = slstimer->next;
    }  
    LogSoftTimerD("\r\n---------\r\n");
}

/*-----------------------------------------------------*/
/**
 * @brief   支持多个loop？
 * 
 * @return  slstloop_t* 
 * 
 */
void *slst_create_loop(void)
{
    slstloop_t *Loop;
    SOFTTIMER_V systime;

    Loop = pcalloc(sizeof(slstloop_t),1);
    systime = Stime_get_systime();
    Loop->last  = systime;

    strcpy(Loop->root.name, "root");
    strcpy(Loop->olist.name, "olist");

    return Loop;
}


void *slst_create(char *name, softtimer_cb_t cb)
{
    slstimer_t *slstimer_p = NULL;

    LogSoftTimerFunIn

    slstimer_p  = pcalloc(sizeof(slstimer_t),1);

    slstimer_p->cb = cb;
    strncpy(slstimer_p->name, name, sizeof(slstimer_p->name)-1);

    LogSoftTimerI("create name:%s\r\n", name);

    LogSoftTimerFunOut
    return slstimer_p;
}


int slst_start(void *loop, void *timer, SOFTTIMER_V timeout_us, void *userdata, SoftTimerType_e type)
{
    //LogSoftTimerFunIn

    slstloop_t *slstloop_P = loop;
    slstimer_t *slstimer = timer;

    SOFTTIMER_V systime;
    systime = Stime_get_systime();

    /* 如果此时loop正在操作这个定时器，会咋样？*/
    slstimer->timeout = systime + timeout_us;
    slstimer->type = type;
    slstimer->period = timeout_us;
    slstimer->userdata = userdata;

    LogSoftTimerI("%s-%d-%d-%d-%d\r\n", slstimer->name, timeout_us, systime, slstimer->timeout, type);

    if (slstimer->pre != NULL){
        LogSoftTimerW(" in list!\r\n");
        return 1;
    }

    slst_list_lock(slstloop_P);
    if (slstimer->timeout < systime) {
        LogSoftTimerW("over list!\r\n");
        slst_list_add(&slstloop_P->olist, slstimer);
    } else {
        slst_list_add(&slstloop_P->root, slstimer);
    }
    slst_list_unlock(slstloop_P);

    ///@todo 发消息唤醒loop

    //LogSoftTimerFunOut

    return 0;
}

int slst_stop(void *loop, void *timer)
{
    slstloop_t *slstloop_P = loop;
    //LogSoftTimerFunIn
    slst_list_lock(slstloop_P);
    slst_list_del(timer);
    slst_list_unlock(slstloop_P);
    //LogSoftTimerFunOut
}

int slst_delete(void *loop, void *timer)
{
    //LogSoftTimerFunIn
    slstloop_t *slstloop_P = loop;
    slst_stop(slstloop_P, timer);

    p_m_free(timer);

    //LogSoftTimerFunOut
}


static void slst_loop_deal_list(slstloop_t *slstloop_P, unsigned char type)
{
    softtimer_cb_t cb;
    slstimer_t *ptimer = &(slstloop_P->root);
    slstimer_t *pre_timer;

    SOFTTIMER_V curtime;

    if (type == 0) curtime = slstloop_P->curtime;
    else curtime = SLST_MAX;

    ptimer = &(slstloop_P->root);

    while(1) {
        if (ptimer->next == NULL) {
            break;
        } 
        ptimer = ptimer->next;
        //LogSoftTimerD("name:%s, timeout:%d\r\n", ptimer->name, ptimer->timeout);
        if (curtime >= ptimer->timeout) {

            /* 先 将节点从链表中剔除 */
            pre_timer = ptimer->pre;
            slst_list_del(ptimer);

            /** 周期定时器，重新加入链表 */
            /* 有没有办法优化周期性定时器的操作？
                如果用无序链表，周期性定时器就不需要操作链表，
                只需要更新时间基点 */
            if (ptimer->type == 1) {
                ptimer->timeout = slstloop_P->curtime + ptimer->period;
                if ((type == 1) || ( ptimer->timeout < slstloop_P->curtime ) ) {
                    slst_list_add(&slstloop_P->olist, ptimer);
                } else {
                    slst_list_add(&slstloop_P->root, ptimer);
                }  
            }  

            /* 再执行回调 */
            slst_list_unlock(slstloop_P);
            ptimer->cb(ptimer->userdata);
            slst_list_lock(slstloop_P);

            /* 执行回调后，ptimer已经从链表剔除，重新指向前一个节点 */
            ptimer =  pre_timer;    
        }
    }
}

/**
 * @brief   描述
 * 
 * @param   past        参数描述
 * @return  int 
 * 
 */
int slst_loop(void *loop)
{
    //LogSoftTimerFunIn

    slstloop_t *slstloop_P = loop;

    SOFTTIMER_V systime;
    systime = Stime_get_systime();
    slstloop_P->curtime = systime;

    #if 1
    //LogSoftTimerD("\r\ncur:%d\r\n", slstloop_P->curtime);
    //slst_show_loop_timer_list();
    #endif

    slst_list_lock(slstloop_P);
    if (slstloop_P->curtime < slstloop_P->last) {
        /* 溢出  把当前链表全部处理 */
        slst_loop_deal_list(slstloop_P, 1);
        
        //LogSoftTimerI("change list\r\n");
        /* 将溢出链表切到当前链表*/
        slstimer_t *olist;
        olist = slstloop_P->olist.next;
        slstloop_P->olist.next = NULL;

        slstloop_P->root.next = olist;
        olist->pre = &(slstloop_P->root);
    }

    slst_loop_deal_list(slstloop_P, 0);

    slstloop_P->last = slstloop_P->curtime;

    ///@todo 返回第一个节点的距离，以便阻塞, 就不需要一直轮询。
    SOFTTIMER_V next_timer;
    slstimer_t *firsttimer;
    firsttimer = slstloop_P->root.next;
    if (firsttimer != NULL)
        next_timer = firsttimer->timeout;
    else {
        next_timer = SLST_MAX;
    }
    /*----------------------------------*/
    slst_list_unlock(slstloop_P);
    
    SOFTTIMER_V outtime;
    outtime = Stime_get_systime();
    
    //LogSoftTimerW("loop:%d\r\n", outtime - slstloop_P->curtime);

    outtime = outtime - slstloop_P->curtime;
    next_timer = next_timer - slstloop_P->curtime;

    if (outtime > next_timer)
        next_timer = 2;
    else 
        next_timer = next_timer - outtime;

    //LogSoftTimerD("sleep: %d\r\n", next_timer);
    //LogSoftTimerFunOut

    return next_timer;
}

/***/
void slst_task(void *loop)
{
    SOFTTIMER_V sleep_time;
    while(1) {
        sleep_time = slst_loop(loop);
        /* 等待信号 */
        osDelay(sleep_time);
    }
}


