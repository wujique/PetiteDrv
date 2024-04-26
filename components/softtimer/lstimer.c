#include "stdlib.h"
#include "log.h"
#include "mem/p_malloc.h"
#include "stimer.h"
#include "cmsis_os.h"
#include "softtimer.h"

/*
    无锁单向链表
*/
struct NllsTimer_s;
/**
 * @brief   定时器节点
 * 
 * 
 */
typedef struct NllsTimer_s{
    /*creat */
    char name[16];
    softtimer_cb_t cb;
    /* start */
    SOFTTIMER_V debtime;
    SOFTTIMER_V base;
    SOFTTIMER_V period;
    SoftTimerType_e type;
    void *userdata;

    /* pri */
    struct NllsTimer_s *next;        
}NllsTimer_t;
/**
 * @brief   定时器LOOP
 * 
 * 
 */
typedef struct NllsTimerLoop_s{
    char name[16];

    NllsTimer_t root;
    volatile NllsTimer_t *end;

    osSemaphoreId Sem;

}NllsTimerLoop_t;

static void slst_list_lock(NllsTimerLoop_t *slstloop_P)
{

}

static void slst_list_unlock(NllsTimerLoop_t *slstloop_P)
{
    
}
/*-----------------------------------------------*/
void nllstimer_show_list(NllsTimer_t *ptimer, SOFTTIMER_V now)
{
    //SOFTTIMER_V timeout;
    SOFTTIMER_V pasttime;
    NllsTimer_t *p = ptimer;
    printf("\r\nname\tbase\tperiod\ttype\tdebtime\tpasttime\r\n");
    while(1) {
        if(p == NULL) break;

        //timeout = p->base+p->period;
        pasttime = now - p->base;
        printf("%s\t%d\t%d\t%d\t%d\t%d\r\n", p->name, p->base, p->period,p->type, p->debtime, pasttime);
        p=p->next;
    }
    printf("----------------------------\r\n");
}
/*-----------------------------------------------*/
void slst_heart_beat(void *userdata)
{
    //printf("soft timer heartbeat\r\n");
}
/**
 * @brief   创建一个定时器LOOP
 * 
 * @return  void* 
 * 
 */
void *slst_create_loop(void)
{
    LogSoftTimerFunIn

    NllsTimerLoop_t *loop;
    NllsTimer_t *ptimer;
    SOFTTIMER_V period;

    loop = pcalloc(sizeof(NllsTimerLoop_t), 1);
    loop->end = &(loop->root);
    loop->Sem = osSemaphoreNew (1, 0, NULL);

    ptimer = &(loop->root);
    strcpy(ptimer->name, "root");
    ptimer->base = Stime_get_systime();
    ptimer->cb = slst_heart_beat;

    period = 0-1;
    ptimer->period = period/10;
    printf("root period:%d\r\n", ptimer->period);
    ptimer->type = SOFTTIMER_TYPE_PERIODIC;
    LogSoftTimerFunOut
    
    return loop;

}
/**
 * @brief   创建定时器
 * 
 * @param   name        定时器名字
 * @param   cb          定时器回调函数
 * @return  void* 
 * 
 */
void *slst_create(char *name, softtimer_cb_t cb)
{    
    LogSoftTimerFunIn
    
    NllsTimer_t *ptimer;
    ptimer = pcalloc(sizeof(NllsTimer_t), 1); 

    strncpy(ptimer->name, name, sizeof(ptimer->name) - 1);
    ptimer->cb = cb; 

    LogSoftTimerD("name:%s\r\n", ptimer->name);
    LogSoftTimerFunOut
    
    return ptimer;

}
/**
 * @brief   启动定时器
 * 
 * @param   loop        添加定时器到此loop
 * @param   timer       定时器指针
 * @param   timeout_us  定时
 * @param   userdata    回调函数入参userdata
 * @param   type        SOFTTIMER_TYPE_ONCE 单次；SOFTTIMER_TYPE_PERIODIC 周期；
 * @return  int 
 * 
 */
int slst_start(void *loop, void *timer, SOFTTIMER_V timeout_us, void *userdata, SoftTimerType_e type)
{    
    //LogSoftTimerFunIn
    
    NllsTimerLoop_t *ploop = loop;
    NllsTimer_t *ptimer = timer; 
    SoftTimerType_e back_type;

    ptimer->period = timeout_us;
    ptimer->debtime = timeout_us;
    ptimer->userdata = userdata;
    back_type = ptimer->type;
    ptimer->type = type;
    SOFTTIMER_V systime;
    systime = Stime_get_systime();
    ptimer->base = systime;

    /* 如何加入链表 ？只有创建后第一次才需要加入链表 
        如果多个定时器同时加入链表，咋办？ 
        模仿CAS试试 */
    LogSoftTimerD("name:%s\r\n", ptimer->name);

    if (back_type == SOFTTIMER_TYPE_NO_LOOP) {
        /* 还不如加锁，，，，，*/
        #if 1
        while(1) {
            /*步骤1：等ploop->end指向的点的next为空
                如果不为空，说明其他人正在执行不走2-3，还没到4 */
            if (ploop->end->next != 0){
                osDelay(5);
                continue;
            } 
            /* 步骤2：将定时器加入最后一个点的next指针 
                如果在这，其他线程抢占，添加定时器，没有问题 */
            ploop->end->next = ptimer;
            /* 步骤3：判断是否加入成功 
                如果这里其他现成添加定时器，步骤三则判断不成功 */
            if (ploop->end->next != ptimer) {
                osDelay(5);
                continue;    
            }   
            /* 步骤4：
                如果到这里被抢占，因为next ！= 0，会释放 */
            ploop->end = ptimer;
            break;
        }
        #endif
    } else {
        //LogSoftTimerD("in list\r\n");
    }
    /** 发送信号量，立刻唤醒一次loop */
    osSemaphoreRelease(ploop->Sem);

    //LogSoftTimerFunOut
    
    return 0;
}
/**
 * @brief   停止定时器
 * 
 * @param   loop        参数描述
 * @param   timer       参数描述
 * @return  int 
 * 
 */
int slst_stop(void *loop, void *timer)
{    
    //LogSoftTimerFunIn
    
    NllsTimer_t *ptimer = timer; 
    ptimer->type = SOFTTIMER_TYPE_STOP;

    //LogSoftTimerFunOut
    
    return 0;  
}
/**
 * @brief   删除定时器
 * 
 * @param   loop        参数描述
 * @param   timer       参数描述
 * @return  int 
 * 
 */
int slst_delete(void *loop, void *timer)
{    
    //LogSoftTimerFunIn
    
    NllsTimer_t *ptimer = timer; 
    ptimer->type = SOFTTIMER_TYPE_STOP;

    //LogSoftTimerFunOut

    return 0;
}

/**
 * @brief   轮询定时器
 * 
 * @param   loop        参数描述
 * @return  int 
 * 
 */
int slst_loop(void *loop)
{
    NllsTimerLoop_t *ploop = loop;
    NllsTimer_t *ptimer = &(ploop->root);
    NllsTimer_t *ltimer = ptimer;

    SOFTTIMER_V systime;
    SOFTTIMER_V pasttime;
    
    systime = Stime_get_systime();

    //LogSoftTimerD("cur time:%d\r\n", systime);
    //printf("\r\nname\tbase\tperiod\ttype\tdebtime\tpasttime\r\n");

    //nllstimer_show_list(&ploop->root, systime);

    while(1) {
        if (ptimer == NULL) {
            break;
        }
        //printf("%s\t%d\t%d\t%d\t%d", ptimer->name, ptimer->base, ptimer->period,ptimer->type, ptimer->debtime);
        switch(ptimer->type) {
            case SOFTTIMER_TYPE_ONCE:
            case SOFTTIMER_TYPE_PERIODIC:
                /* 必须每次都获取系统时间，以便避免：
                    定时器1到，回调中 重新start 定时器2,
                    接着LOOP定时器2，base 大于 systime， 
                    算出来的pasttime就很大了，
                    因此定时器2立刻就执行， 不符合逻辑*/
                systime = Stime_get_systime();
                /* 计算每个定时器已经过去的时间 */
                pasttime = systime- ptimer->base;
                //printf("\t%d", pasttime);
                ptimer->base = systime;
                /*  使用debtime 的原因：
                    如果使用 if (pasttime >= ptimer->period),
                    
                    假如8bit机器，base = 100, period = 255最大值，
                    那这个定时器很可能永远不会执行，
                    因为pasttime不可能大于 255，只能等于255，
                    但是等于255这个点，很可能是遇不到的。
                    
                    本方案有一个心跳定时器，配合使用debtime，
                    可以避免类似问题。
                    */
                if(pasttime  >= ptimer->debtime) {
                    /* 必须先配置stop，以为在回调中用户可能重新start */
                    if (ptimer->type == SOFTTIMER_TYPE_ONCE){
                        ptimer->type = SOFTTIMER_TYPE_STOP;
                    }
                    ptimer->debtime = ptimer->period;
                    //printf("\r\n");
                    ptimer->cb(ptimer->userdata);
                } else {
                    ptimer->debtime -= pasttime;
                }
                break;
            
            case SOFTTIMER_TYPE_DELETE:
                if (ptimer != ploop->end) {
                    LogSoftTimerD("del:%s\r\n", ptimer->name);
                    /* 不删除最后一个节点，防止其他线程 start新节点时崩溃 */
                    ltimer->next = ptimer->next;
                    /* 强行清0 */
                    memset(ptimer, 0, sizeof(NllsTimer_t));

                    p_m_free(ptimer);
                    ptimer = ltimer;
                }
                break;

            case SOFTTIMER_TYPE_NO_LOOP:
                /* 不可能发生 */
            case SOFTTIMER_TYPE_STOP:
                /* 啥都不干 */
            default:
                
                break;
        }
        
        ltimer = ptimer;
        ptimer = ptimer->next;
    }

    #if 1//计算下一次定时
    int ret = osSemaphoreAcquire(ploop->Sem, 1); 
    if (osOK == ret) {
        //printf("sem\r\n");    
    }

    SOFTTIMER_V sleeptime = SLST_MAX;
    ptimer = &(ploop->root);
    while(1) {
        if (ptimer == NULL) {
            break;
        }

        switch(ptimer->type) {
            case SOFTTIMER_TYPE_ONCE:
            case SOFTTIMER_TYPE_PERIODIC:
                if (ptimer->debtime < sleeptime){
                        sleeptime = ptimer->debtime;
                    }
                break;
            
            default:break;
        }

        ptimer = ptimer->next;
    }
    //printf("sleeptime:%d\r\n", sleeptime);
    return sleeptime;
    #endif

    return 5;
}

void slst_task(void *loop)
{
    SOFTTIMER_V sleep_time;
    NllsTimerLoop_t *ploop = loop;
    while(1) {
        sleep_time = slst_loop(loop);
        /* 等待信号 */
        //osDelay(sleep_time);
        osSemaphoreAcquire(ploop->Sem, sleep_time);
    }
}




