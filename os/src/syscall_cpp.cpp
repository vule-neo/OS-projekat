#include "../h/syscall_cpp.hpp"
#include "../h/syscall_c.hpp"


//thread class

Thread::Thread(void (*body)(void *), void *arg) {
    this->body = body;
    this->arg = arg;//prvi argument da li valja??
}

Thread::~Thread() {}

int Thread::start() {
    thread_create(&this->myHandle, body, this);
    if (!myHandle) return -1;
    return 0;
}

Thread::Thread() {
    body = &wrapper;
    arg = this;
}

void Thread::wrapper(void *x) {
    ((Thread*)x)->run();
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t) {
    return 0;
}

//semaphore class

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t) {
    return 0;
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

//periodicthread class

void PeriodicThread::terminate() {
    return;
}

PeriodicThread::PeriodicThread(time_t period) {

}

//console class

char Console::getc() {
    return PROJECT_BASE_SYSCALL_C_HPP::getc();
}

void Console::putc(char c) {
    PROJECT_BASE_SYSCALL_C_HPP::putc(c);
}
