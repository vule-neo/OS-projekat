#include "../h/syscall_c.hpp"
#include "../lib/hw.h"
#include "../lib/console.h"
#include "../h/TrapUtil.hpp"


void* mem_alloc (size_t size){

    size_t numOfBlocks;
    numOfBlocks= (size / MEM_BLOCK_SIZE) + ((size%MEM_BLOCK_SIZE==0)?0:1);

    __asm__ volatile("mv a1, %0" : : "r"(numOfBlocks));
    __asm__ volatile("li a0, 0x01");

    __asm__ volatile("ecall");

    void * allocatedAddr;

    __asm__ volatile("mv %0, a0" : "=r"(allocatedAddr));

    return allocatedAddr;

};

int mem_free (void* addr){
    __asm__ volatile("mv a1, %0" : : "r"(addr));
    __asm__ volatile("li a0, 0x02");

    __asm__ volatile("ecall");

    int status;

    __asm__ volatile("mv %0, a0" : "=r"(status));

    return status;
};


int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg){

    void* stack = nullptr;
    if (start_routine){
        stack = mem_alloc(DEFAULT_STACK_SIZE);//za main ne treba stack
    }
    //nema memory leaka, adresa steka uspjesno prenijeta daljem nivou
    //kompajler vrsi optimizacije i ne cuva vrijednost mem_alloc-a
    // u stack nego u a0 do npr poziva neke f-je sa parametrom stack kada ga prepise u stack???

    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("mv a6, %0" : : "r"(start_routine));
    __asm__ volatile("mv a2, %0" : : "r"(arg));
    __asm__ volatile("mv a7, %0" : : "r"(stack));
    __asm__ volatile("li a0, 0x11");

    __asm__ volatile("ecall");

    int status;

    __asm__ volatile("mv %0, a0" : "=r"(status));

    return status;

};


int thread_exit (){
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");
    int status;
    __asm__ volatile("mv %0, a0" : "=r"(status));

    return status;
};

void thread_dispatch (){
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
};



int sem_open (sem_t* handle, unsigned init ){

    __asm__ volatile("mv a2, %0" : : "r"(handle));
    __asm__ volatile("mv a1, %0" : : "r"(init));
    __asm__ volatile("li a0, 0x21");

    __asm__ volatile("ecall");

    int status;

    __asm__ volatile("mv %0, a0" : "=r"(status));

    return status;
};

int sem_close (sem_t handle){
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x22");

    __asm__ volatile("ecall");

    int status;

    __asm__ volatile("mv %0, a0" : "=r"(status));

    return status;
};


int sem_wait (sem_t id){
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x23");

    __asm__ volatile("ecall");

    int status;

    __asm__ volatile("mv %0, a0" : "=r"(status));

    return status;
};

int sem_signal (sem_t id){
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x24");

    __asm__ volatile("ecall");

    int status;

    __asm__ volatile("mv %0, a0" : "=r"(status));

    return status;
};


int sem_trywait(sem_t id){
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x26");

    __asm__ volatile("ecall");

    int status;

    __asm__ volatile("mv %0, a0" : "=r"(status));

    return status;
};

int time_sleep (time_t){
    return 0;
};

char getc(){

    __asm__ volatile("li a0, 0x41");
    __asm__ volatile("ecall");

    uint64 ret;

    __asm__ volatile("mv %0, a0" : "=r"(ret));

    return (int)ret;
}

void putc(char chr){
    __asm__ volatile("mv a1, %0" : : "r"(chr));
    __asm__ volatile("li a0, 0x42");
    __asm__ volatile("ecall");
}




