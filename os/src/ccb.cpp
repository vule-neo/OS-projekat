
#include "../h/ccb.hpp"
#include "../h/syscall_c.hpp"
#include "../h/print.hpp"
#include "../h/TrapUtil.hpp"

CCB *CCB::running = nullptr;
int CCB::idd = 0;

void CCB::dispatch() {
    CCB * old = CCB::running;
    if (!old->isFinished && !old->waitingOnSem) Scheduler::put(old);
    running = Scheduler::get();

    CCB::contextSwitch(&old->context, &running->context);
    if (old->getIsFinished()) delete old;//dodao ovo
}

CCB *CCB::createCoroutine(Body body, void *arg, void *stack_space) {
    CCB * newCCB;
    newCCB = new CCB(body, arg, stack_space);
    return newCCB;
}

void CCB::wrapper(){
    TrapUtil::popSppSpie();
    running->body(running->args);
    thread_exit();
}
