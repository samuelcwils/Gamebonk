#include "cpu.h"
    
    cpu::cpu(bus* Bus)
    {
        this->Bus = Bus;
        cycles = 0;
        af.af = 0;
        bc.bc = 0;
        de.de = 0;
        sp.sp = 0;
        pc.pc = 0x0150;
    }

    void cpu::checkInterrupts()
    {

    }

    void cpu::emulateCycles()
    {
        

    }