
#ifndef PROJECT_BASE_SEMAPHORE_HPP
#define PROJECT_BASE_SEMAPHORE_HPP
#include "../h/queue.hpp"
#include "../h/ccb.hpp"
#include "../h/syscall_c.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../lib/hw.h"
#include "../h/print.hpp"

class KSemaphore{
private:
    unsigned int val;
    bool closed;
    Queue<CCB> * waiting;
    KSemaphore(unsigned int initVal): val(initVal), closed(false){
        waiting = new Queue<CCB>();
    }

public:
    ~KSemaphore(){ delete waiting; }
    int signal();

    int getVal(){return val;};

    int wait();
    int tryWait();
    bool getClosed(){return closed;}
    int close();
    static KSemaphore * createSemaphore(unsigned int initVal);

};

#endif //PROJECT_BASE_SEMAPHORE_HPP
