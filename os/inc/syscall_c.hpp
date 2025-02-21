
#ifndef PROJECT_BASE_SYSCALL_C_HPP
#define PROJECT_BASE_SYSCALL_C_HPP
#include "../lib/hw.h"
#include "../h/ccb.hpp"
#include "../h/semaphore.hpp"

void* mem_alloc (size_t size);

int mem_free (void* addr);


typedef CCB* thread_t;
int thread_create (
        thread_t* handle,
        void(*start_routine)(void*),
        void* arg );

int thread_exit ();
void thread_dispatch ();

class KSemaphore;
typedef KSemaphore* sem_t;
int sem_open (
        sem_t* handle,
        unsigned init );

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);
typedef unsigned long time_t;
int sem_timedwait(sem_t id, time_t timeout );

int sem_trywait(sem_t id);


int time_sleep (time_t);

void putc(char chr);
char getc();



#endif //PROJECT_BASE_SYSCALL_C_HPP
