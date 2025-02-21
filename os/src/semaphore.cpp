#include "../h/semaphore.hpp"
#include "../h/scheduler.hpp"
#include "../h/print.hpp"

int KSemaphore::signal() {
    if (this->closed) return -1;
    if(((int)(val++)) < 0){
        CCB * backToWork = waiting->take();
        if (!backToWork) return 0;//u slucaju da su zvali tryWait bice neg iako niko ne ceka
        backToWork->setWaitingOnSem(false);
        Scheduler::put(backToWork);
    }
    return 0;

}

KSemaphore *KSemaphore::createSemaphore(unsigned int initVal) {
    KSemaphore *newSem;
    newSem = new KSemaphore(initVal);
    return newSem;
}

int KSemaphore::wait() {
    if(this->closed){
        return -1;
    }
    if(((int)(val--)) <= 0){
        CCB * goToBlocked = CCB::running;
        goToBlocked->setWaitingOnSem(true);
        waiting->add(goToBlocked);
        CCB::dispatch();
    }//nit se vrati na mjesto poziva ovog dispatch-a zatim provjeri da li je uzrok zatvaranje semafora
    if(this->closed){
        return -1;
    }
    return 0;
}

int KSemaphore::close() {
    if (closed) return -1;
    closed = true;
    CCB * coroutineWaiting = waiting->take();
    while (coroutineWaiting){
        coroutineWaiting->setWaitingOnSem(false);
        Scheduler::put(coroutineWaiting);
        coroutineWaiting = waiting->take();
    }
    delete this; //dodao ovo brisanje semafora
    return 0;

}

int KSemaphore::tryWait() {
    if (this->closed) return -1;
    if(((int)(val--)) <= 0){
        return 0;
    }
    return 1;
}
