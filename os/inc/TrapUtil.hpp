
#ifndef PROJECT_BASE_TRAPUTIL_HPP
#define PROJECT_BASE_TRAPUTIL_HPP
#include "../lib/hw.h"


class TrapUtil{
public:
    static bool userModeOn;

    static uint64 r_sepc();
    static uint64 r_scause();
    static uint64 r_sstatus();

    static void w_sepc(uint64 newSepc);
    static void w_sstatus(uint64 newSstatus);

    static inline void userMode(){
        userModeOn = true;
    }

    static inline void systemMode(){
        userModeOn = false;
    }

    enum BitMaskSstatus {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };
    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };


    static void ms_sstatus(uint64 mask);
    static void mc_sstatus(uint64 mask);
    static uint64 r_stvec();
    static void w_stvec(uint64 stvec);
    static void mc_sip(uint64 mask);

    static void popSppSpie();

    static void handleSupervisorTrap();
    static void supervisorTrap();

private:
    enum ecallIf{
        ECALL_USER =  0x0000000000000008UL,
        ECALL_SUPERVISOR =  0x0000000000000009UL,
        CONSOLE_INTERRUPT = 0x8000000000000009UL,
        TIMER_INTERRUPT = 0x8000000000000001UL,
    };
};

inline void TrapUtil::ms_sstatus(uint64 mask) {
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void TrapUtil::mc_sstatus(uint64 mask) {
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 TrapUtil::r_stvec(){
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void TrapUtil::w_stvec(uint64 stvec){
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}


inline void TrapUtil::mc_sip(uint64 mask){
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

#endif //PROJECT_BASE_TRAPUTIL_HPP
