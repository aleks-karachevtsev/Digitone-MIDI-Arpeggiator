#include <stdint.h>
extern void live_poll(void);
extern int fw_task_create(void *,void (*)(void),int,void *,int);
extern void fw_task_start(void *);
extern void fw_sem_init(void *,int);
extern void fw_sem_pend(void *);
extern void fw_sem_post(void *);
static uint32_t tcb[32];
static uint32_t stack[4096];
/* Count plus waiter-list pointers: keep this distinct from the task stack. */
static uint32_t semaphore[4];
static volatile int active,pending;
static volatile uint32_t due_tick;
static void fast_loop(void){
    for(;;){
        fw_sem_pend(semaphore);
        pending=0;
        live_poll();
    }
}
void fast_start(void *ctrl){
    (void)ctrl;
    if(active)return;
    fw_sem_init(semaphore,0);
    due_tick=0;
    active=1;
    fw_task_create(tcb,fast_loop,1,stack,sizeof(stack));
    fw_task_start(tcb);
}
int fast_active(void){return active;}
void fast_schedule(uint32_t tick){due_tick=tick;}
void fast_wake(void){
    if(active && !pending){pending=1;fw_sem_post(semaphore);}
}
void fast_wake_at(uint32_t tick){
    if((int32_t)(tick-due_tick)>=0)fast_wake();
}
