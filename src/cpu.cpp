#include "cpu.h"
    
    cpu::cpu(bus* Bus)
    {
        this->Bus = Bus;
        cycles = 0;
        af.af = 0;
        bc.bc = 0;
        de.de = 0;
        sp.sp = 0;
        pc.pc = 0;
        IF = 0;
        IE = 0;
        IME = 0;
        bootromDone = false;

    }

    void cpu::checkInterrupts()
    {
        if(IME)
        {
            if(IE & (0b00000001))
            {
                if(IF & (0b00000001))
                {
                    IF &= 0b11111110;
                    sp.sp--;
                    Bus->write(sp.sp, pc.bytes.p);
                    sp.sp--;
                    Bus->write(sp.sp, pc.bytes.c);
                    
                    uint16_t temp = pc.pc;

                    pc.pc = 0x0040;
                    cycles+=20;
                }
            }
        }

    }
