#ifndef __SLSTIMER_H__


#define LogSoftTimer(l,args...) petite_log(l, "SLST", NULL,__FUNCTION__, __LINE__, ##args);
#define LogSoftTimerE(args...)  LogSoftTimer(LOG_ERR, ##args)
#define LogSoftTimerW(args...)  LogSoftTimer(LOG_WAR, ##args)
#define LogSoftTimerI(args...)  LogSoftTimer(LOG_INFO, ##args)
#define LogSoftTimerD(args...)  LogSoftTimer(LOG_DEBUG, ##args)

#define LogSoftTimerFunIn LogSoftTimerD("IN\r\n");
#define LogSoftTimerFunOut LogSoftTimerD("OUT\r\n");

typedef void (*softtimer_cb_t)(void *userdata);

/**
 * @brief   定义变量，决定了溢出时间。
 *          方便用于8位、16位、32位、64位系统
 *          与获取系统时间的位数一致
 *          
 */
//typedef unsigned int SOFTTIMER_V; //uint32
typedef unsigned short int SOFTTIMER_V; //uint16
//typedef unsigned char SOFTTIMER_V;  //uint8

typedef enum{
    SOFTTIMER_TYPE_NO_LOOP = 0,
    SOFTTIMER_TYPE_ONCE,
    SOFTTIMER_TYPE_PERIODIC,

    SOFTTIMER_TYPE_STOP = 0x50,
    SOFTTIMER_TYPE_DELETE,

}SoftTimerType_e;
/**
 * @brief   最大定时
 */
#define  SLST_MAX    ((~((SOFTTIMER_V)0)))

void *slst_create_loop(void);
void *slst_create(char *name, softtimer_cb_t cb);
int slst_start(void *loop, void *timer, SOFTTIMER_V timeout_us, void *userdata, SoftTimerType_e type);
int slst_stop(void *loop, void *timer);
int slst_delete(void *loop, void *timer);
int slst_loop(void *loop);
void slst_task(void *loop);
#endif