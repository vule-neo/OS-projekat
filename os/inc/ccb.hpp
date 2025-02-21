
#ifndef PROJECT_BASE_CCB_HPP
#define PROJECT_BASE_CCB_HPP
#include "../lib/hw.h"
#include "../h/TrapUtil.hpp"
#include "../h/print.hpp"
#include "../h/MemoryAllocator.hpp"
#include "scheduler.hpp"



//int thread_create ( thread_t* handle, void(*start_routine)(void*), void* arg, void* stack_space ); ABI
class CCB{
public:
    typedef void (*Body)(void*);
private:
    static int idd;
    struct Context{
        uint64 ra;
        uint64 sp;
    };
    bool waitingOnSem;
    int id;
    Body body;
    void * args;
    char * stack;
    bool isFinished;
    Context context;

    CCB(Body body, void* args, void* stack_space):
            waitingOnSem(false),
            id(idd++),
            body(body != nullptr?body: 0),
            args(args),
            stack(body != nullptr?(char*)stack_space: 0),
            isFinished(false),
            context({
                            body!=nullptr?(uint64)&wrapper:0,
                            body != nullptr?(uint64)stack + DEFAULT_STACK_SIZE - 1:0,
                    }){
        if (body!=nullptr)Scheduler::put(this);
        else CCB::running = this;
    }


public:

    ~CCB(){delete stack;}
    void setWaitingOnSem(bool val){waitingOnSem = val;}

    static void wrapper();

    static CCB* createCoroutine(Body body, void *arg, void* stack_space);

    bool getIsFinished(){return isFinished;}
    void setIsFinished(bool val){isFinished = val;}

    static void dispatch();

    static CCB * running;

    static void contextSwitch(Context * oldContext, Context * newContext);

};

#endif //PROJECT_BASE_CCB_HPP
