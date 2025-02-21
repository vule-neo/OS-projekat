#include "../h/TrapUtil.hpp"
#include "../h/print.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/semaphore.hpp"
#include "../h/ccb.hpp"
#include "../lib/console.h"

bool TrapUtil::userModeOn = false;

inline uint64 TrapUtil::r_sepc(){
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline uint64 TrapUtil::r_scause() {
    uint64 scauseVar;
    __asm__ volatile("csrr %[sc], scause":[sc] "=r" (scauseVar));
    return scauseVar;
}

inline void TrapUtil::w_sepc(uint64 newSepc) {
    __asm__ volatile ("csrw sepc, %[ns]" : : [ns] "r"(newSepc));
}

inline uint64 TrapUtil::r_sstatus(){
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void TrapUtil::w_sstatus(uint64 newSstatus){
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(newSstatus));
}

void TrapUtil::handleSupervisorTrap() {
    bool skip = false;
    uint64 switchStatement;
    __asm__ volatile("mv %0, a0" : "=r" (switchStatement));
    uint64 scause = r_scause();

    //razgranati skok sistemski pozivi
    if(scause == ecallIf::ECALL_USER || scause == ecallIf::ECALL_SUPERVISOR){
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus = r_sstatus();
        switch(switchStatement){
            case 0x01: //mem_alloc
                uint64 size;

                __asm__ volatile("mv %0, a1" : "=r" (size));

                MemoryAllocator::kmem_alloc((size_t)size);
                break;
            case 0x02: //mem_free
                void * addrFree;
                __asm__ volatile("mv %0, a1" : "=r" (addrFree));

                MemoryAllocator::kmem_free(addrFree);

                break;
            case 0x11: //thread_create
                CCB** handle;
                CCB::Body startRoutine;
                void* arg;
                void* stackSpace;
                __asm__ volatile ("mv %0, a1" : "=r" (handle));
                __asm__ volatile ("mv %0, a6" : "=r" (startRoutine));
                __asm__ volatile ("mv %0, a2" : "=r" (arg));
                __asm__ volatile ("mv %0, a7" : "=r" (stackSpace));

                *handle = CCB::createCoroutine(startRoutine, arg, stackSpace);
                if(*handle != nullptr) {
                    __asm__ volatile ("li a0, 0");
                }
                else {
                    __asm__ volatile ("li a0, -1");
                }
                break;
            case 0x12: //thread_exit
                if(CCB::running->getIsFinished()){
                    __asm__ volatile ("li a0, -1");//ako je vec gotovo onda je valjda greska??
                }
                CCB::running->setIsFinished(true);
                //CCB * forDelete;//dodao ovo
                //forDelete = CCB::running;
                CCB::dispatch();//da obrisem ovdje poslije
                //delete forDelete;
                __asm__ volatile ("li a0, 0");
                break;
            case 0x13: //thread_dispatch
                CCB::dispatch();
                break;
            case 0x21: //sem_open
                KSemaphore** semHandle;
                unsigned int val;
                __asm__ volatile ("mv %0, a2" : "=r" (semHandle));
                __asm__ volatile ("mv %0, a1" : "=r" (val));

                *semHandle = KSemaphore::createSemaphore(val);
                if(*semHandle != nullptr) {
                    __asm__ volatile ("li a0, 0");
                }
                else {
                    __asm__ volatile ("li a0, -1");
                }
                break;
            case 0x22: //sem_close
                KSemaphore* handleSem;
                __asm__ volatile ("mv %0, a1" : "=r" (handleSem));
                handleSem->close();

                break;
            case 0x23: //sem_wait
                KSemaphore* id;
                __asm__ volatile ("mv %0, a1" : "=r" (id));
                id->wait();

                break;
            case 0x24: //sem_signal
                KSemaphore* idSem;
                __asm__ volatile ("mv %0, a1" : "=r" (idSem));
                idSem->signal();

                break;
            case 0x26: //sem_trywait
                KSemaphore* semId;
                __asm__ volatile ("mv %0, a1" : "=r" (semId));
                semId->tryWait();

                break;
            case 0x41: //getc
                char c;
                c =__getc();
                __asm__ volatile("mv a0, %0" : : "r"(c));
                break;
            case 0x42: //putc
                uint64 ch;
                __asm__ volatile ("mv %0, a1" : "=r" (ch));
                __putc((char)ch);

                break;

            default:
                if(userModeOn){
                    CCB::dispatch();
                }else{
                    ms_sstatus(TrapUtil::SSTATUS_SPP);
                    skip = true;
                }

                break;
        }
        w_sepc(sepc);
        if(!skip)w_sstatus(sstatus);
    }else if(scause == ecallIf::CONSOLE_INTERRUPT){
        console_handler();
    }else if(scause == ecallIf::TIMER_INTERRUPT){
        mc_sip(SIP_SSIP);
    }else{
        printStringg("SCAUSE: ");
        printInteger(scause);
        printStringg("\nSEPC: ");
        printInteger((uint64)r_sepc());
        printStringg("\n");
    }
}


void TrapUtil::popSppSpie(){
    if (userModeOn) {
        mc_sstatus(TrapUtil::SSTATUS_SPP);
    }
    else {
        ms_sstatus(TrapUtil::SSTATUS_SPP);
    }
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}