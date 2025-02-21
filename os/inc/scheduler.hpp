
#ifndef PROJECT_BASE_SCHEDULER_HPP
#define PROJECT_BASE_SCHEDULER_HPP
#include "../h/queue.hpp"

class CCB;

class Scheduler{
private:
    static Queue<CCB> *ccbsQueue;
public:
    ~Scheduler(){
        delete ccbsQueue;
    }
    //static void idleFunc(void *);
    static CCB * get();
    static void put(CCB* ccb);
    static void schedulerBegin();
    static void schedulerEnd();
};





//vidjecemo dal radi ovaj idle
//CCB * Scheduler::idle = CCB::createCoroutine(idle, Scheduler::idleFunc(), nullptr, nullptr);


#endif //PROJECT_BASE_SCHEDULER_HPP
