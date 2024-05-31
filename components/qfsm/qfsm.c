/**
 * @file    qfsm.c
 * @brief   qep fsm
 * 
 * @author  pochard(email@xxx.com)
 * @version 0.1
 * @date    2023-12-10
 * @copyright Copyright (c) 2023..
 */
#include "mcu.h"
//#include "petite_config.h"
#include "log.h"
/**
 * QFSM（QEP FSM）
 * 一种用函数指针作为状态的有限状态机架构
 * 请参考：
 * https://www.state-machine.com/psicc2
 * <PSiCC2-CN.pdf> P106
 * QFSM只是量子架构的一小部分。
 * 
*/
#include "qfsm.h"

static QEvent QEP_reservedEvt_[Q_USER_SIG]={
    {0,0},
    {Q_ENTRY_SIG,0},
    {Q_EXIT_SIG,0},
    {Q_INIT_SIG,0},
};

/* file qfsm_ini.c ---------------------------------------------------------*/

void
QFsm_init(QFsm* me, QEvent const* e) {
    (*me->state)(me, e); /* execute the top-most initial transition */
                         /* enter the target */
    (void)(*me->state)(me, &QEP_reservedEvt_[Q_ENTRY_SIG]);
}

/* file qfsm_dis.c ---------------------------------------------------------*/

void
QFsm_dispatch(QFsm* me, QEvent const* e) {
    QStateHandler s = me->state;                                /* save the current state */
    QState r = (*s)(me, e);                                     /* call the event handler */
    if (r == Q_RET_TRAN) {                                      /* transition taken? */
        /*状态切换时, 退出旧状态，进入新状态 */
        (void)(*s)(me, &QEP_reservedEvt_[Q_EXIT_SIG]);          /* exit the source */
        (void)(*me->state)(me, &QEP_reservedEvt_[Q_ENTRY_SIG]); /*enter target*/
    }
}


/*

----------------------- 一个简单的例子 demo -----------------------------------

*/

enum BombSignals { /* all signals for the Bomb FSM */
                   UP_SIG = Q_USER_SIG,
                   DOWN_SIG,
                   ARM_SIG,
                   TICK_SIG
};

typedef struct TickEvtTag {
    QEvent super;      /* derive from the QEvent structure */
    uint8_t fine_time; /* the fine 1/10 s counter */
} TickEvt;

typedef struct Bomb4Tag {
    QFsm super;      /* derive from QFsm */
    uint8_t timeout; /* number of seconds till explosion */
    uint8_t code;    /* currently entered code to disarm the bomb */
    uint8_t defuse;  /* secret defuse code to disarm the bomb */
} Bomb4;

void Bomb4_ctor(Bomb4* me, uint8_t defuse);
QState Bomb4_initial(Bomb4* me, QEvent const* e);
QState Bomb4_setting(Bomb4* me, QEvent const* e);
QState Bomb4_timing(Bomb4* me, QEvent const* e);
/*--------------------------------------------------------------------------*/
/* the initial value of the timeout */
#define INIT_TIMEOUT 10

/*..........................................................................*/
void
Bomb4_ctor(Bomb4* me, uint8_t defuse) {
    QFsm_ctor(&me->super, (QStateHandler)&Bomb4_initial);
    me->defuse = defuse; /* the defuse code is assigned at instantiation */
}

/*..........................................................................*/
QState
Bomb4_initial(Bomb4* me, QEvent const* e) {
    printf("sta:initial,sig:%d\r\n", e->sig);

    (void)e;
    me->timeout = INIT_TIMEOUT;
    return Q_TRAN(&Bomb4_setting);
}

/*..........................................................................*/
QState
Bomb4_setting(Bomb4* me, QEvent const* e) {
    printf("sta:setting,sig:%d\r\n", e->sig);

    switch (e->sig) {
        case UP_SIG: {
            if (me->timeout < 60) {
                ++me->timeout;
                printf("bomb timeout:%d\r\n", me->timeout);
            }
            return Q_HANDLED();
        }
        case DOWN_SIG: {
            if (me->timeout > 1) {
                --me->timeout;
                printf("bomb timeout:%d\r\n", me->timeout);
            }
            return Q_HANDLED();
        }
        case ARM_SIG: {
            return Q_TRAN(&Bomb4_timing); /* transition to "timing" */
        }
    }
    return Q_IGNORED();
}

/*..........................................................................*/
QState
Bomb4_timing(Bomb4* me, QEvent const* e) {
    printf("sta:timing,sig:%d\r\n", e->sig);

    switch (e->sig) {
        case Q_ENTRY_SIG: {
            me->code = 0; /* clear the defuse code */
            return Q_HANDLED();
        }
        case UP_SIG: {
            me->code <<= 1;
            me->code |= 1;
            return Q_HANDLED();
        }
        case DOWN_SIG: {
            me->code <<= 1;
            return Q_HANDLED();
        }
        case ARM_SIG: {
            if (me->code == me->defuse) {
                return Q_TRAN(&Bomb4_setting);
            }
            return Q_HANDLED();
        }
        case TICK_SIG: {
            if (((TickEvt const*)e)->fine_time == 0) {
                if (me->timeout > 0)
                    --me->timeout;

                printf("bomb timeout:%d\r\n", me->timeout);
                if (me->timeout == 0) {
                    printf("bomb finish!\r\n"); /* destroy the bomb */
                }
            }
            return Q_HANDLED();
        }
    }
    return Q_IGNORED();
}



void bomb_test(void)
{
    Bomb4 bomb_fsm;//定义一个状态机实例
    QFsm *p_bomb_qfsm;
    QEvent bomb_key_event;//定义一个qevent

    TickEvt bomb_tick_event;
    QEvent *p_qevent;

    p_bomb_qfsm = &(bomb_fsm.super);

    bomb_tick_event.super.sig = TICK_SIG;
    bomb_tick_event.fine_time = 0;
    p_qevent = &bomb_tick_event.super;

    Bomb4_ctor(&bomb_fsm, 0x55);

    bomb_key_event.sig = DOWN_SIG;
    QFsm_dispatch(p_bomb_qfsm, &bomb_key_event);
    QFsm_dispatch(p_bomb_qfsm, &bomb_key_event);

    bomb_key_event.sig = ARM_SIG;
    QFsm_dispatch(p_bomb_qfsm, &bomb_key_event);


    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);

    bomb_key_event.sig = DOWN_SIG;
    QFsm_dispatch(p_bomb_qfsm, &bomb_key_event);
    QFsm_dispatch(p_bomb_qfsm, &bomb_key_event);

    bomb_key_event.sig = ARM_SIG;
    QFsm_dispatch(p_bomb_qfsm, &bomb_key_event);

    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);
    QFsm_dispatch(p_bomb_qfsm, p_qevent);

    return;
}
/*------------------------end----------------------------*/
