#ifndef _QFSM_H_
#define _QFSM_H_

/* qevent.h ----------------------------------------------------------------*/
typedef uint8_t QSignal;

typedef struct QEventTag { /* the event structure */
    QSignal sig;           /* signal of the event */
    uint8_t dynamic_;      /* dynamic attribute of the event (0 for static) */
} QEvent;

/* qep.h -------------------------------------------------------------------*/
typedef uint8_t QState;          /* status returned from a state-handler function */
typedef                          /* pointer to function type definition */
    QState                       /* return type */
    (*QStateHandler)             /* name of the pointer-to-function type */
    (void* me, QEvent const* e); /* argument list */

typedef struct QFsmTag { /* Finite State Machine */
    QStateHandler state; /* current active state */
} QFsm;

#define QFsm_ctor(me_, initial_) ((me_)->state = (initial_))

void QFsm_init(QFsm* me, QEvent const* e);
void QFsm_dispatch(QFsm* me, QEvent const* e);

#define Q_RET_HANDLED   ((QState)0)
#define Q_RET_IGNORED   ((QState)1)
#define Q_RET_TRAN      ((QState)2)
#define Q_HANDLED()     (Q_RET_HANDLED)
#define Q_IGNORED()     (Q_RET_IGNORED)
#define Q_TRAN(target_) (((QFsm*)me)->state = (QStateHandler)(target_), Q_RET_TRAN)

enum QReservedSignals {
    Q_ENTRY_SIG = 1, /* signal for coding entry actions */
    Q_EXIT_SIG,      /* signal for coding exit actions */
    Q_INIT_SIG,      /* signal for coding initial transitions */
    Q_USER_SIG       /* first signal that can be used in user applications */
};

#endif
