
#include "../h/scheduler.hpp"

Queue<CCB> *Scheduler::ccbsQueue;


CCB *Scheduler::get() {
    CCB * ret = ccbsQueue->take();
    return ret;
}

void Scheduler::put(CCB *ccb) {
    ccbsQueue->add(ccb);
}

void Scheduler::schedulerBegin() {
    ccbsQueue = new Queue<CCB>();
}

void Scheduler::schedulerEnd() {
    delete ccbsQueue;
}
