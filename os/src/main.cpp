#include "../h/syscall_c.hpp"
#include "../h/print.hpp"
#include "../h/userMain.hpp"
#include "../h/TrapUtil.hpp"
#include "../h/syscall_cpp.hpp"

int main(){

    MemoryAllocator::memBegin();
    TrapUtil::w_stvec((uint64) &TrapUtil::supervisorTrap);
    Scheduler::schedulerBegin();

    CCB *coroutines[2];

    coroutines[0] = CCB::createCoroutine( nullptr,  nullptr, nullptr);

    TrapUtil::ms_sstatus(TrapUtil::SSTATUS_SIE);
    thread_create(&coroutines[1], reinterpret_cast<void (*)(void *)>(userMain), nullptr);
    TrapUtil::userMode();

    __asm__ volatile("ecall");//default switch skok


    while (!(coroutines[1]->getIsFinished())){
        thread_dispatch();
    }

    TrapUtil::systemMode();
    __asm__ volatile("ecall");//default switch skok
    //asm volatile("csrr t6, sepc");

    Scheduler::schedulerEnd();
    MemoryAllocator::memClean();
    printStringg("Main end\n");

    return 0;
}
