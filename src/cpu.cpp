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

    void cpu::Zflag(uint16_t a, uint16_t b)
    {
        if(!(a+b)){ //Z flag
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;
        }     
    }

    void cpu::Hflag(uint8_t a, uint8_t b)
    {
        if(((a & 0xf) + (b & 0xf)) & 0x10){
            af.bytes.f |= 0b01000000;
        } else {
            af.bytes.f &= 0b11011111;
        }
    }
    
    void cpu::Hflag(uint16_t a, uint16_t b)
    {
        if((((a >> 8) & 0xf) + ((b >> 8) & 0xf)) & 0x10){ 
            af.bytes.f |= 0b01000000;
        } else {
            af.bytes.f &= 0b11011111;
        }
    }

    void cpu::Cflag(uint8_t a, uint8_t b)
    {
        int temp = (a + b) >> 8;
        if(temp){
            af.bytes.f |= 0b00010000;
        } else{
            af.bytes.f &= 0b11101111;
        }
    }

    void cpu::Cflag(uint16_t a, uint16_t b)
    {
        int temp = (a + b) >> 16;
        if(temp){
            af.bytes.f |= 0b00010000;
        } else{
            af.bytes.f &= 0b11101111;
        }
    }

    void cpu::LD_d16(uint8_t &high, uint8_t &low)
    {
        low = Bus->read(pc.pc+1);
        high = Bus->read(pc.pc+2);
        pc.pc+=3;
        cycles+=12;       
    }

    void cpu::LD_d8(uint8_t &byte)
    {
        byte = Bus->read(pc.pc+1);
        pc.pc+=2;
        cycles+=8;
    }

    void cpu::LD_REG1_REG2(uint8_t &a, uint8_t b)
    {
        a = b;
        pc.pc+=1;
        cycles+=4;
    }
    
    void cpu::LD_HL_REG(uint8_t reg)
    {
        Bus->write(hl.hl, reg);
        pc.pc+=1;
        cycles+=8;
    }

    void cpu::LD_REG_HL(uint8_t &reg)
    {
        reg = Bus->read(hl.hl);
        pc.pc+=1;
        cycles+=8;
    }
    
    void cpu::LD_ADDRESS_A(uint16_t address)
    {
        Bus->write(address, af.bytes.a);
        pc.pc+=1;
        cycles+=8;
    }

    void cpu::LD_A_ADDRESS(uint16_t address)
    {
        af.bytes.a = Bus->read(address);
        pc.pc+=1;
        cycles+=8;
    }

    void cpu::JP_a16()
    {
        uint16_t temp = pc.pc;
        pc.bytes.c = Bus->read(temp+1);
        pc.bytes.p = Bus->read(temp+2);
        cycles+=16;
    }

    void cpu::JR()
    {
        pc.pc += (signed char)((Bus->read(pc.pc+2)+(pc.pc+1)));
        pc.pc+=2;
        cycles+=12;  
    }

    void cpu::JR_cond(bool flag)
    {
        if(flag)
        {
            JR();
        } else {
            pc.pc+=2;
            cycles+=8;
        }
    }

    void cpu::JP_cond(bool flag)
    {
        if(flag)
        {
            JP_a16();
        } else {
            pc.pc+=3;
            cycles+=12;
        }
    }

    void cpu::POP_16b(uint8_t &high, uint8_t &low)
    {
        low = Bus->read(sp.sp);
        high = Bus->read(sp.sp+=1);
        sp.sp+=1;
        pc.pc+=1;
        cycles+=12;
    }

    void cpu::PUSH_16b(uint8_t high, uint8_t low)
    {
        sp.sp--;
        Bus->write(sp.sp, high);
        sp.sp--;
        Bus->write(sp.sp, low); 
        pc.pc+=1;
        cycles+=16;    
    }

    void cpu::CALL()
    {
        sp.sp--;
        Bus->write(sp.sp, pc.bytes.p);
        sp.sp--;
        Bus->write(sp.sp, pc.bytes.c);
        
        uint16_t temp = pc.pc;

        pc.bytes.c = Bus->read(temp+1);
        pc.bytes.p = Bus->read(temp+2);
        cycles+=24;
    }

    void cpu::CALL_cond(bool flag)
    {
        if(flag)
        {
            CALL();
        } else {
            pc.pc+=3;
            cycles+=12;
        }
    }

    void cpu::RST(uint8_t H)
    {
        sp.sp--;
        Bus->write(sp.sp, pc.bytes.p);
        sp.sp--;
        Bus->write(sp.sp, pc.bytes.c);  
        pc.pc = 0x0000+H;
        cycles+=16;
    }

    void cpu::RET()
    {
        pc.bytes.c = Bus->read(sp.sp);
        pc.bytes.p = Bus->read(sp.sp+=1);
        sp.sp+=1;
        cycles+=16;
    }

    void cpu::RET_cond(bool flag)
    {
        if(flag)
        {
            RET();
            cycles+=4;//extra cycles for condition
        } else {
            cycles +=8;
        }

    }

    void cpu::ADD(uint16_t &a, uint16_t b)
    {
  
        af.bytes.f &= 0b10111111; //N flag
        Hflag(a, b);
        Cflag(a, b);
        a += b;
        pc.pc+=1;
        cycles+=8;
    }
    
    void cpu::ADD(uint8_t &a, uint8_t b)
    {
        Zflag(a, b);
        af.bytes.f &= 0b10111111; //N flag
        Hflag(a, b);
        Cflag(a, b);
        a += b;
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::ADC(uint8_t &a, uint8_t b)
    {
        uint8_t temp = (b + (af.bytes.f & 0x00010000));
        Zflag(a, temp);
        af.bytes.f &= 0b10111111; //N flag
        Hflag(a, temp);
        Cflag(a, temp);
        a += temp;
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::SUB(uint8_t &a, uint8_t b)
    {
        Zflag(a, -b);
        af.bytes.f |= 0b01000000; //N flag
        Hflag(a, -b);
        Cflag(a, -b);
        a -= b;
        pc.pc+=1;
        cycles+=4;      
    }

    void cpu::SBC(uint8_t &a, uint8_t b)
    {
        uint8_t temp = (b + (af.bytes.f & 0x00010000));
        Zflag(a, -temp);
        af.bytes.f |= 0b01000000; //N flag
        Hflag(a, -temp);
        Cflag(a, -temp);
        a -= temp; 
        pc.pc+=1;
        cycles+=4;       
    }

    void cpu::AND(uint8_t &a, uint8_t b)
    {
        a&=b;
        
        if(a==0)
        {
            af.bytes.f |= 0b10000000;//Z Flag
        }
        af.bytes.f |= 0b00100000;//H Flag
        af.bytes.f &= 0b10101111;//N and C flag
        
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::XOR(uint8_t &a, uint8_t b)
    {
        a^=b;
        
        if(a==0)
        {
            af.bytes.f |= 0b10000000;//Z Flag
        }
        af.bytes.f &= 0b10001111;//N and H and C flag
        
        pc.pc+=1;
        cycles+=4;
    }
    
    void cpu::OR(uint8_t &a, uint8_t b)
    {
        a|=b;        
        
        if(a==0) 
        {
            af.bytes.f |= 0b10000000;//Z Flag
        }
        af.bytes.f &= 0b10001111;//N and H and C flag
        
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::CP(uint8_t &a, uint8_t b)
    {
        Zflag(a, -b);
        af.bytes.f |= 0b01000000; //N flag
        Hflag(a, -b);
        Cflag(a, -b);
        pc.pc+=1;
        cycles+=4;
    }


    void cpu::INC(uint16_t &value)
    {
        value+=1;
        pc.pc+=1;
        cycles+=8;
    }

    void cpu::INC(uint8_t &byte)
    {
        Zflag(byte, 1);
        af.bytes.f &= 0b10111111; //N flag
        Hflag(byte, 1);
        byte+=1;
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::DEC(uint16_t &value)
    {
        value-=1;
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::DEC(uint8_t &byte)
    {
        Zflag(byte, -1);
        af.bytes.f |= 0b01000000;
        Cflag(byte, -1);
        byte-=1;
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::execOP()
    {
        uint8_t opcode = Bus->read(pc.pc);
        uint8_t opcodeH = (opcode & 0xF0) >> 4;
        uint8_t opcodeL = opcode & 0x0F;

        switch (opcodeH)
        {
        case 0x0:
            switch(opcodeL)
            {
                case 0x0: //NOP
                    pc.pc+=1;
                    cycles+=4;
                    break;
                case 0x1: //LD BC,d16
                    LD_d16(bc.bytes.b, bc.bytes.c);
                    break;                
                case 0x2: //LD (BC),A
                    LD_A_ADDRESS(bc.bc);
                    break;
                case 0x3: //INC BC
                    INC(bc.bc);
                    break;
                case 0x4: //INC B (Z 0 H -) 
                    INC(bc.bytes.b);
                    break;
                case 0x5: //DEC B (Z 1 H -)
                    DEC(bc.bytes.b);
                    break;
                case 0x6: //LD B,d8
                    LD_d8(bc.bytes.b);
                    break;
                case 0x7: //RLCA (0 0 0 A7)
                { 
                    bool carry = (((af.bytes.a << 1) & 0xff00));
                    af.bytes.a <<= 1;
                    af.bytes.a += carry;
                    
                    if(carry){
                        af.bytes.f |= 0b00010000;
                    } else {
                        af.bytes.f &= 0b11101111;
                    }                    
                    
                    pc.pc+=1;
                    cycles+=4;
                    break; 
                }
                case 0x8: //LD a16, SP
                    Bus->write(pc.pc+1, sp.sp & 0x00ff);
                    Bus->write(pc.pc+2, (sp.sp & 0xff00) >> 8);
                    pc.pc+=3;
                    cycles+=20;
                    break;
                case 0x9: //ADD HL, BC (- 0 H C)
                    ADD(hl.hl, bc.bc);
                    break;
                case 0xa: //LD A, (BC)
                    LD_A_ADDRESS(bc.bc);
                    break;
                case 0xb: //DEC BC
                    DEC(bc.bc);
                    break;
                case 0xc: //INC C (Z 0 H -)
                    INC(bc.bytes.c);
                    break;
                case 0xd: //DEC C (Z 1 H -)
                    DEC(bc.bytes.c);
                    break;
                case 0xe: //LD C, d8
                    LD_d8(bc.bytes.c);
                case 0xf: //RRCA (0 0 0 A0)
                { 
                    bool carry = (af.bytes.a & 0x01);
                    af.bytes.a >>= 1;
                    af.bytes.a += (carry << 8);
                    
                    if(carry){
                        af.bytes.f |= 0b00010000;
                    } else {
                        af.bytes.f &= 0b11101111;
                    }                    
                    
                    pc.pc+=1;
                    cycles+=4;
                    break; 
                }

            }
            break;
        
        case 0x1:
            switch(opcodeL)
            {
                case 0x0://STOP
                    //TODO. Need buttons.

                    //Halts and turns off screen/sound.

                case 0x1://LD DE, d16
                    LD_d16(de.bytes.d, de.bytes.e);
                    break;
                case 0x2://LD DE, a
                    LD_ADDRESS_A(de.bytes.d);
                case 0x3://INC DE
                   INC(de.de);
                    break;
                case 0x4://INC D (Z 0 H -)
                    INC(de.bytes.d);
                    break;
                case 0x5:// DEC D (Z 1 H -)
                    DEC(de.bytes.d);
                    break;
                case 0x6://LD D,d8
                    LD_d8(de.bytes.d);
                    break;
                case 0x7://RLA (0 0 0 A7)
                { 
                    bool carry = (((af.bytes.a << 1) & 0xff00));
                    af.bytes.a <<= 1;
                    af.bytes.a += ((af.bytes.f & 0b00010000) >> 5);
                    
                    if(carry){
                        af.bytes.f |= 0b00010000;
                    } else {
                        af.bytes.f &= 0b11101111;
                    }                    
                    
                    pc.pc+=1;
                    cycles+=4;
                    break; 
                }
                case 0x8://JR s8
                    JR();
                case 0x9://ADD HL, DE
                    ADD(hl.hl, bc.bc);
                    break;
                case 0xa://LD A, (DE)
                    LD_A_ADDRESS(bc.bc);
                    break;
                case 0xb://DEC DE
                    DEC(de.de);
                    break;
                case 0xc://INC E
                    INC(de.bytes.e);
                    break;
                case 0xd://DEC E
                    DEC(de.bytes.e);
                    break;
                case 0xe://LDE E,d8
                    LD_d8(de.bytes.e);
                    break;
                case 0xf://RRA (0 0 0 A7)
                    bool carry = (((af.bytes.a >> 1) & 0x00ff));
                    af.bytes.a >>= 1;
                    af.bytes.a += ((af.bytes.f & 0b00010000) << 3);
                    
                    if(carry){
                        af.bytes.f |= 0b00010000;
                    } else {
                        af.bytes.f &= 0b11101111;
                    }                    
                    
                    pc.pc+=1;
                    cycles+=4;  
                    break;
            }
            break;
    
        case 0x2:
            switch(opcodeL)
            {
                case 0x0://JR NZ,r8
                    JR_cond(af.bytes.f & 0b01111111);
                    break;
                case 0x1://LD HL, d16
                    LD_d16(de.bytes.d, de.bytes.e);
                    break;
                case 0x2://LD (HL+), A
                    LD_ADDRESS_A(hl.hl);
                    hl.hl++;
                    break;
                case 0x3://INC HL (Z 0 H -)
                    INC(hl.hl);
                    cycles+=8; //extra cycles for this instruction
                    break;
                case 0x4://INC H (Z 0 H -)
                    INC(hl.bytes.h);
                    break;
                case 0x5://DEC H (Z 1 H -)
                    DEC(hl.bytes.h);
                    cycles+=8; //extra cycles for this instruction
                    break;
                case 0x6://LD H, d8
                    LD_d8(hl.bytes.h);
                    break;
                case 0x7://DAA (Z - 0 C) //TODO
                    if((af.bytes.f & 0b01000000) || ((af.bytes.a && 0b11110000) > 9))
                    {
                        Zflag(af.bytes.a, 0x06);
                        af.bytes.f &= 0b10111111; //N flag
                        Cflag(af.bytes.a, 0x06);
                        af.bytes.a+=0x06;
                    } else if ((af.bytes.f & 0b00010000) || ((af.bytes.a >> 4) > 9)) {
                        Zflag(af.bytes.a, 0x60); 
                        af.bytes.a+=0x60; //N flag
                        Cflag(af.bytes.a, 0x60);
                        
                    }
                    pc.pc+=1;
                    pc.pc+=4;
                    break;
                case 0x8: //JR Z, r8
                    JR_cond(af.bytes.f & 0b10000000);
                    break; 
                case 0x9://ADD HL, HL (- 0 H C)
                    ADD(hl.hl, hl.hl);
                    break;
                case 0xa://LD A,(HL+)
                    LD_A_ADDRESS(hl.hl);
                    hl.hl++;
                    break;
                case 0xb://DEC HL
                    DEC(hl.hl);
                    break;
                case 0xc://INC L
                    INC(hl.bytes.l);
                    break;
                case 0xd://DEC L
                    DEC(hl.bytes.l);
                    break;
                case 0xe://LD L,d8
                    LD_d8(hl.bytes.l);
                    break;
                case 0xf://CPL
                    af.bytes.a = ~af.bytes.a;
                    af.bytes.f |= 0b01000000;
                    af.bytes.f |= 0b00100000;
                    break;
            }
            break;

        case 0x3:
            switch(opcodeL)
            {
                case 0x0://LD NC,s8
                    if(!(af.bytes.f & 0b00010000))
                    {
                        pc.pc += (signed char)((Bus->read(pc.pc+2)+(pc.pc+1)));
                        pc.pc+=2;
                        cycles+=12;  
                    } else {
                        pc.pc+=2;
                        cycles+=8;
                    }
                case 0x1://LD SP,d16
                    LD_d16(sp.bytes.s, sp.bytes.p);
                    break;
                case 0x2://LD (HL-),d16
                    LD_ADDRESS_A(hl.hl);
                    hl.hl--;
                    break;
                case 0x3://INC SP
                    INC(sp.sp);
                    break;
                case 0x4://INC (HL) (Z 0 H -)
                {
                    uint8_t temp = Bus->read(hl.hl);
                    Zflag(temp, 1);
                    af.bytes.f &= 0b10111111; //N flag
                    Hflag(temp, 1);
                    temp++;
                    Bus->write(hl.hl, temp);
                    pc.pc+=1;
                    cycles+=12;
                }
                case 0x5://DEC (HL) (Z 1 H -)
                {
                    uint8_t temp = Bus->read(hl.hl);
                    Zflag(temp, -1);
                    af.bytes.f |= 0b01000000; //N flag
                    Hflag(temp, -1);
                    temp--;
                    Bus->write(hl.hl, temp);
                    pc.pc+=1;
                    cycles+=12;
                    break;
                }
                case 0x6://LD (HL),d8
                    Bus->write(hl.hl, Bus->read(pc.pc+1));
                    pc.pc+=2;
                    cycles+=12;
                    break;
                case 0x7://SCF (- 0 0 C)
                    af.bytes.f &= 0b10011111; //sets N and H to 0
                    af.bytes.f |= 0b00010000; //sets C to 1
                    pc.pc+=1;
                    cycles+=4;
                    break;
                case 0x8://JR C,r8
                    JR_cond(af.bytes.f & 0b00010000);
                    break; 
                case 0x9://ADD HL,SP (- 0 H C)
                    ADD(hl.hl, sp.sp);
                    break;
                case 0xa://LD A, (HL-)
                    LD_A_ADDRESS(hl.hl);
                    hl.hl--;
                    break;
                case 0xb://DEC SP
                    DEC(sp.sp);
                    break;
                case 0xc://INC A (Z 0 H -)
                    INC(af.bytes.a);
                    break;
                case 0xd://DEC A (Z 1 H -)
                    DEC(af.bytes.a);
                    break;
                case 0xe://LD A,d8
                    LD_d8(af.bytes.a);
                    break;
                case 0xf://CCF (- 0 0 !C)
                    af.bytes.f ^= 0b00010000;
                    af.bytes.f &= 0b10011111; //N and H
                    pc.pc+=1;
                    cycles+=4;
                    break;
            }
            break;

        case 0x4:
            switch(opcodeL)
            {
                case 0x0://LD B,B
                    LD_REG1_REG2(bc.bytes.b, bc.bytes.b);
                    break;
                case 0x1://LD B,C
                    LD_REG1_REG2(bc.bytes.b, bc.bytes.c);
                    break;
                case 0x2://LD B,D
                    LD_REG1_REG2(bc.bytes.b, de.bytes.d);
                    break;
                case 0x3://LD B,E
                    LD_REG1_REG2(bc.bytes.b, de.bytes.e);
                    break;
                case 0x4://LD B,H
                    LD_REG1_REG2(bc.bytes.b, hl.bytes.h);
                    break;
                case 0x5://LD B,L
                    LD_REG1_REG2(bc.bytes.b, hl.bytes.l);
                    break;
                case 0x6://LD B,(HL)
                    LD_REG_HL(bc.bytes.b);
                    break;
                case 0x7://LD B,A
                    LD_REG1_REG2(bc.bytes.b, af.bytes.a);
                    break;
                case 0x8://LD C,B
                    LD_REG1_REG2(bc.bytes.c, bc.bytes.b);
                    break;
                case 0x9://LD C,C
                    LD_REG1_REG2(bc.bytes.c, bc.bytes.c);
                    break;
                case 0xa://LD C,D
                    LD_REG1_REG2(bc.bytes.c, de.bytes.d);
                    break;
                case 0xb://LD C,E
                    LD_REG1_REG2(bc.bytes.c, de.bytes.e);
                    break;
                case 0xc://LD C,H
                    LD_REG1_REG2(bc.bytes.c, hl.bytes.h);
                    break;
                case 0xd://LD C,L
                    LD_REG1_REG2(bc.bytes.c, hl.bytes.l);
                    break;
                case 0xe://LD C,(HL)
                    LD_REG_HL(bc.bytes.c);
                    break;
                case 0xf://LD C,A
                    LD_REG1_REG2(bc.bytes.c, af.bytes.a);
                    break;
            }
            break;

        case 0x5:
            switch(opcodeL)
            {
                case 0x0://LD D,B
                    LD_REG1_REG2(de.bytes.d, bc.bytes.b);
                    break;
                case 0x1://LD D,C
                    LD_REG1_REG2(de.bytes.d, bc.bytes.c);
                    break;
                case 0x2://LD D,D
                    LD_REG1_REG2(de.bytes.d, de.bytes.d);
                    break;
                case 0x3://LD D,E
                    LD_REG1_REG2(de.bytes.d, de.bytes.e);
                    break;
                case 0x4://LD D,H
                    LD_REG1_REG2(de.bytes.d, hl.bytes.h);
                    break;
                case 0x5://LD D,L
                    LD_REG1_REG2(de.bytes.d, hl.bytes.l);
                    break;
                case 0x6://LD D,(HL)
                    LD_REG_HL(de.bytes.d);
                    break;
                case 0x7://LD D,A
                    LD_REG1_REG2(de.bytes.d, af.bytes.a);
                    break;
                case 0x8://LD E,B
                    LD_REG1_REG2(de.bytes.e, bc.bytes.b);
                    break;
                case 0x9://LD E,C
                    LD_REG1_REG2(de.bytes.e, bc.bytes.c);
                    break;
                case 0xa://LD E,D
                    LD_REG1_REG2(de.bytes.e, de.bytes.d);
                    break;
                case 0xb://LD E,E
                    LD_REG1_REG2(de.bytes.e, de.bytes.e);
                    break;
                case 0xc://LD E,H
                    LD_REG1_REG2(de.bytes.e, hl.bytes.h);
                    break;
                case 0xd://LD E,L
                    LD_REG1_REG2(de.bytes.e, hl.bytes.l);
                    break;
                case 0xe://LD E,(HL)
                    LD_REG_HL(de.bytes.e);
                    break;
                case 0xf://LD E,A
                    LD_REG1_REG2(de.bytes.e, af.bytes.a);
                    break;
            }
            break;
        
        case 0x6:
            switch(opcodeL)
            {
                case 0x0://LD H,B
                    LD_REG1_REG2(hl.bytes.h, bc.bytes.b);
                    break;
                case 0x1://LD H,C
                    LD_REG1_REG2(hl.bytes.h, bc.bytes.c);
                    break;
                case 0x2://LD H,D
                    LD_REG1_REG2(hl.bytes.h, de.bytes.d);
                    break;
                case 0x3://LD H,E
                    LD_REG1_REG2(hl.bytes.h, de.bytes.e);
                    break;
                case 0x4://LD H,H
                    LD_REG1_REG2(hl.bytes.h, hl.bytes.h);
                    break;
                case 0x5://LD H,L
                    LD_REG1_REG2(hl.bytes.h, hl.bytes.l);
                    break;
                case 0x6://LD H,(HL)
                    LD_REG_HL(hl.bytes.h);
                    break;
                case 0x7://LD H,A
                    LD_REG1_REG2(hl.bytes.h, af.bytes.a);
                    break;
                case 0x8://LD L,B
                    LD_REG1_REG2(hl.bytes.l, bc.bytes.b);
                    break;
                case 0x9://LD L,C
                    LD_REG1_REG2(hl.bytes.l, bc.bytes.c);
                    break;
                case 0xa://LD L,D
                    LD_REG1_REG2(hl.bytes.l, de.bytes.d);
                    break;
                case 0xb://LD L,E
                    LD_REG1_REG2(hl.bytes.l, de.bytes.e);
                    break;
                case 0xc://LD L,H
                    LD_REG1_REG2(hl.bytes.l, hl.bytes.h);
                    break;
                case 0xd://LD L,L
                    LD_REG1_REG2(hl.bytes.l, hl.bytes.l);
                    break;
                case 0xe://LD L,(HL)
                    LD_REG_HL(hl.bytes.l);
                    break;
                case 0xf://LD L,A
                    LD_REG1_REG2(hl.bytes.l, af.bytes.a);
                    break;
            }
            break;
        
        case 0x7:
            switch(opcodeL)
            {
                case 0x0://LD (HL),B
                    LD_HL_REG(bc.bytes.b);
                    break;
                case 0x1://LD (HL),C
                    LD_HL_REG(bc.bytes.c);
                    break;
                case 0x2://LD (HL),D
                    LD_HL_REG(de.bytes.d);
                    break;
                case 0x3://LD (HL),E
                    LD_HL_REG(de.bytes.e);
                    break;
                case 0x4://LD (HL),H
                    LD_HL_REG(hl.bytes.h);
                    break;
                case 0x5://LD (HL),L
                    LD_HL_REG(hl.bytes.l);
                    break;
                case 0x6://HALT   TODO: INPLEMENT INTERRUPTS
                    break;
                case 0x7://LD (HL),A
                    LD_HL_REG(af.bytes.a);
                case 0x8://LD A,B
                    LD_REG1_REG2(af.bytes.a, bc.bytes.b);
                    break;
                case 0x9://LD A,C
                    LD_REG1_REG2(af.bytes.a, bc.bytes.c);
                    break;
                case 0xa://LD A,D
                    LD_REG1_REG2(af.bytes.a, de.bytes.d);
                    break;
                case 0xb://LD A,E
                    LD_REG1_REG2(af.bytes.a, de.bytes.e);
                    break;
                case 0xc://LD A, H
                    LD_REG1_REG2(af.bytes.a, hl.bytes.h);
                    break;
                case 0xd://LD A,L
                    LD_REG1_REG2(af.bytes.a, hl.bytes.l);
                    break;
                case 0xe://LD A,(HL)
                    LD_REG_HL(af.bytes.a);
                    break;
                case 0xf://LD A,A
                    LD_REG1_REG2(af.bytes.a, af.bytes.a);
                    break;
            }
            break;

        case 0x8:
            switch(opcodeL)
            {
                case 0x0://ADD A,B (Z 0 H C)
                    ADD(af.bytes.a, bc.bytes.b);
                    break;
                case 0x1://ADD A,C (Z 0 H C)
                    ADD(af.bytes.a, bc.bytes.c);
                    break;
                case 0x2://ADD A,D (Z 0 H C)
                    ADD(af.bytes.a, de.bytes.d);
                    break;
                case 0x3://ADD A,E (Z 0 H C)
                    ADD(af.bytes.a, de.bytes.e);
                    break;
                case 0x4://ADD A,H (Z 0 H C)
                    ADD(af.bytes.a, hl.bytes.h);
                    break;
                case 0x5://ADD A,L (Z 0 H C)
                    ADD(af.bytes.a, hl.bytes.l);
                    break;
                case 0x6://ADD A,(HL) (Z 0 H C)
                    ADD(af.bytes.a, Bus->read(hl.hl));
                    cycles+=4; //extra time for hl
                    break;
                case 0x7://ADD A,A (Z 0 H C)
                    ADD(af.bytes.a, af.bytes.a);
                    break;
                case 0x8://ADC A,B (Z 0 H C)
                    ADC(af.bytes.a, bc.bytes.b);
                    break;
                case 0x9://ADC A,C (Z 0 H C)
                    ADC(af.bytes.a, bc.bytes.c);
                    break;
                case 0xa://ADC A,D (Z 0 H C)
                    ADC(af.bytes.a, de.bytes.d);
                    break;
                case 0xb://ADC A,E (Z 0 H C)
                    ADC(af.bytes.a, de.bytes.e);
                    break;
                case 0xc://ADC A,H (Z 0 H C)
                    ADC(af.bytes.a, hl.bytes.h);
                    break;
                case 0xd://ADC A,L (Z 0 H C)
                    ADC(af.bytes.a, hl.bytes.l);
                    break;
                case 0xe://ADC A,(HL) (Z 0 H C)
                    ADC(af.bytes.a, Bus->read(hl.hl));
                    cycles+=4; //extra time for hl
                    break;
                case 0xf://ADC A,A (Z 0 H C)
                    ADC(af.bytes.a, af.bytes.a);
            }
            break;
        
        case 0x9:
            switch(opcodeL)
            {
                case 0x0://SUB B (Z 1 H C)
                    SUB(af.bytes.a, bc.bytes.b);
                    break;
                case 0x1://SUB C (Z 1 H C)
                    SUB(af.bytes.a, bc.bytes.c);
                    break;
                case 0x2://SUB D (Z 1 H C)
                    SUB(af.bytes.a, de.bytes.d);
                    break;
                case 0x3://SUB E (Z 1 H C)
                    SUB(af.bytes.a, de.bytes.e);
                    break;
                case 0x4://SUB H (Z 1 H C)
                    SUB(af.bytes.a, hl.bytes.h);
                    break;
                case 0x5://SUB L (Z 1 H C)
                    SUB(af.bytes.a, hl.bytes.l);
                    break;
                case 0x6://SUB (HL) (Z 1 H C)
                    SUB(af.bytes.a, Bus->read(hl.hl));
                    cycles+=4; //extra time for hl
                    break;
                case 0x7://SUB A (Z 1 H C)
                    SUB(af.bytes.a, af.bytes.a);
                    break;
                case 0x8://SBC A,B (Z 1 H C)
                    SBC(af.bytes.a, bc.bytes.b);
                    break;
                case 0x9://SBC A,C (Z 1 H C)
                    SBC(af.bytes.a, bc.bytes.c);
                    break;
                case 0xa://SBC A,D (Z 1 H C)
                    SBC(af.bytes.a, de.bytes.d);
                    break;
                case 0xb://SBC A,E (Z 1 H C)
                    SBC(af.bytes.a, de.bytes.e);
                    break;
                case 0xc://SBC A,H (Z 1 H C)
                    SBC(af.bytes.a, hl.bytes.h);
                    break;
                case 0xd://SBC A,L (Z 1 H C)
                    SBC(af.bytes.a, hl.bytes.l);
                    break;
                case 0xe://SBC A,(HL) (Z 1 H C)
                    SBC(af.bytes.a, Bus->read(hl.hl));
                    break;
                case 0xf://SBC A,A (Z 1 H C)
                    SBC(af.bytes.a, af.bytes.a);
                    break;
            }
            break;
        
        case 0xa:
            switch(opcodeL)
            {
                case 0x0://AND B (Z 0 1 0)
                    AND(af.bytes.a, bc.bytes.b);
                    break;
                case 0x1://AND C (Z 0 1 0)
                    AND(af.bytes.a, bc.bytes.c);
                    break;
                case 0x2://AND D (Z 0 1 0)
                    AND(af.bytes.a, de.bytes.d);
                    break;
                case 0x3://AND E (Z 0 1 0)
                    AND(af.bytes.a, de.bytes.e);
                    break;
                case 0x4://AND H (Z 0 1 0)
                    AND(af.bytes.a, hl.bytes.h);
                    break;
                case 0x5://AND L (Z 0 1 0)
                    AND(af.bytes.a, hl.bytes.l);
                    break;
                case 0x6://AND (HL) (Z 0 1 0)
                    AND(af.bytes.a, Bus->read(hl.hl));
                    cycles+=4; //extra time for hl
                    break;
                case 0x7://AND A (Z 0 1 0)
                    AND(af.bytes.a, af.bytes.a);
                    break;
                case 0x8://XOR B (Z 0 0 0)
                    XOR(af.bytes.a, bc.bytes.b);
                    break;
                case 0x9://XOR C (Z 0 0 0)
                    XOR(af.bytes.a, bc.bytes.c);
                    break;
                case 0xa://XOR D (Z 0 0 0)
                    XOR(af.bytes.a, de.bytes.d);
                    break;
                case 0xb://XOR E (Z 0 0 0)
                    XOR(af.bytes.a, de.bytes.e);
                    break;
                case 0xc://XOR H (Z 0 0 0)
                    XOR(af.bytes.a, hl.bytes.h);
                    break;
                case 0xd://XOR L (Z 0 0 0)
                    XOR(af.bytes.a, hl.bytes.l);
                    break;
                case 0xe://XOR (HL) (Z 0 0 0)
                    XOR(af.bytes.a, Bus->read(hl.hl));
                    break;
                case 0xf://XOR A (Z 0 0 0)
                    XOR(af.bytes.a, af.bytes.a);
                    break;
            }
            break;
                    
        case 0xb:
            switch(opcodeL)
            {
                case 0x0://OR B (Z 0 0 0)
                    OR(af.bytes.a, bc.bytes.b);
                    break;
                case 0x1://OR C (Z 0 0 0)
                    OR(af.bytes.a, bc.bytes.c);
                    break;
                case 0x2://OR D (Z 0 0 0)
                    OR(af.bytes.a, de.bytes.d);
                    break;
                case 0x3://OR E (Z 0 0 0)
                    OR(af.bytes.a, de.bytes.e);
                    break;
                case 0x4://OR H (Z 0 0 0)
                    OR(af.bytes.a, hl.bytes.h);
                    break;
                case 0x5://OR L (Z 0 0 0)
                    OR(af.bytes.a, hl.bytes.l);
                    break;
                case 0x6://OR (HL) (Z 0 0 0)
                    OR(af.bytes.a, Bus->read(hl.hl));
                    cycles+=4; //extra time for hl
                    break;
                case 0x7://OR A (Z 0 0 0)
                    OR(af.bytes.a, af.bytes.a);
                    break;
                case 0x8://CP B (Z 1 H C)
                    CP(af.bytes.a, bc.bytes.b);
                    break;
                case 0x9://CP C (Z 1 H C)
                    CP(af.bytes.a, bc.bytes.c);
                    break;
                case 0xa://CP D (Z 1 H C)
                    CP(af.bytes.a, de.bytes.d);
                    break;
                case 0xb://CP E (Z 1 H C)
                    CP(af.bytes.a, de.bytes.e);
                    break;
                case 0xc://CP H (Z 1 H C)
                    CP(af.bytes.a, hl.bytes.h);
                    break;
                case 0xd://CP L (Z 1 H C)
                    CP(af.bytes.a, hl.bytes.l);
                    break;
                case 0xe://CP (HL) (Z 1 H C)
                    CP(af.bytes.a, Bus->read(hl.hl));
                    break;
                case 0xf://CP A (Z 1 H C)
                    CP(af.bytes.a, af.bytes.a);
                    break;
            }
            break;
        
        case 0xc:
            switch(opcodeL)
            {
                case 0x0://RET NZ
                    if(af.bytes.f &= 0b10000000)
                    {
                        pc.bytes.c = Bus->read(sp.sp);
                        pc.bytes.p = Bus->read(sp.sp+=1);
                        sp.sp+=1;
                        cycles+=20;
                    } else {
                        pc.pc+=1;
                        cycles+=8;
                    }
                    break;
                case 0x1://POP BC
                    POP_16b(bc.bytes.b, bc.bytes.c);
                    break;
                case 0x2://JP NZ,a16
                    JP_cond(!(af.bytes.f & 0b10000000));
                    break;
                case 0x3://JP a16
                    pc.bytes.c = Bus->read(pc.pc+1);
                    pc.bytes.p = Bus->read(pc.pc+2);
                    pc.pc+=3;
                    cycles+=4;
                    break;
                case 0x4://CALL NZ,a16
                    CALL_cond(!(af.bytes.f & 0b10000000));
                    break;
                case 0x5://PUSH BC
                    PUSH_16b(bc.bytes.b, bc.bytes.c);
                    break;
                case 0x6://ADD A,d8
                    ADD(af.bytes.a, Bus->read(pc.pc+1));
                    pc.pc+=1;//Extra time and length for this one
                    cycles+=4;
                    break;
                case 0x7://RST 00H
                    RST(0x00);
                    break;
                case 0x8://RET Z
                    RET_cond(af.bytes.f & 0b10000000);
                    break;
                case 0x9://RET
                    RET();
                    break;
                case 0xa://JP Z,a16
                    JP_cond(af.bytes.f & 0b10000000);
                    break;
                case 0xb://PREFIX CB TODO LATER
                    break;
                case 0xc://CALL Z,a16
                    CALL_cond(af.bytes.f & 0b10000000);
                    break;
                case 0xd://CALL a16
                    CALL();
                    break;
                case 0xe://ADC A,d8
                    ADC(af.bytes.a, Bus->read(pc.pc+1));
                    pc.pc+=1; //extra byte for this instruction
                    break;
                case 0xf://RST 08H
                    RST(0x08);
                    break;
            }
            break;
        
        case 0xd:
            switch(opcodeL)
            {
                case 0x0://RET NC
                    RET_cond(!(af.bytes.f & 0b10000000));
                    break;
                case 0x1://POP DE
                    POP_16b(de.bytes.d, de.bytes.e);
                    break;
                case 0x2://JP NC,a16
                    JP_cond(!(af.bytes.f & 0b00010000));
                    break;
                case 0x3://NO OP
                    break;
                case 0x4://CALL NC,a16
                    CALL_cond(!(af.bytes.f & 0b00010000));
                    break;
                case 0x5://PUSH_ DE
                    PUSH_16b(bc.bytes.b, bc.bytes.c);
                    break;
                case 0x6://SUB d8 (Z 1 H C)
                    SUB(af.bytes.a, Bus->read(pc.pc+1));
                    pc.pc+=1;//extra byte for this instruction
                    break;
                case 0x7://RST 10H
                    RST(0x10);
                    break;
                case 0x8://RET C
                    RET_cond(bc.bytes.c);
                    break;
                case 0x9://RETI TODO NEED INTERRUPTS
                    RET();
                    break;
                case 0xa://JP C,a16
                    JP_cond(af.bytes.f & 0b00010000);
                    break;
                case 0xb://NO OP
                    break;
                case 0xc://CALL C,a16
                    CALL_cond(bc.bytes.c & 0b00010000);
                    break;
                case 0xd://NO OP
                    break;
                case 0xe://SBC A,d8 (Z 1 H C)
                    SBC(af.bytes.a, Bus->read(pc.pc+1));
                    pc.pc+=1; //extra byte for this instruction
                    break;
                case 0xf://RST 18H
                    RST(0x18);
                    break;
            }
            break;
        
        case 0xe:
            switch(opcodeL)
            {
                case 0x0://LD (a8), A
                    Bus->write((Bus->read(pc.pc+1))+0xff00, af.bytes.a);
                    pc.pc+=2;
                    cycles+=12;
                    break;
                case 0x1://POP HL
                    POP_16b(hl.bytes.h, hl.bytes.l);
                    break;
                case 0x2://LD (C),A
                    Bus->write((0xff00+bc.bytes.c), af.bytes.a);
                    break;
                case 0x3://NO OP
                    break;
                case 0x4://NO OP
                    break;
                case 0x5://PUSH HL
                    PUSH_16b(hl.bytes.h, hl.bytes.l);
                    break;
                case 0x6://AND d8
                    AND(af.bytes.a, Bus->read(pc.pc+1));
                    break;
                case 0x7://RST 00h
                    RST(0x20);
                    break;
                case 0x8://ADD SP, r8
                    ADD(sp.sp, (int)(Bus->read(pc.pc+1)));
                    pc.pc+=1; // extra for this instruction
                    cycles+=8; //
                    break;
                case 0x9://JP HL
                    pc.pc = hl.hl;
                    cycles+=4;
                    break;
                case 0xa://LD (a16),A
                    uint16_t a16;
                    a16 |= Bus->read(pc.pc+1);//get low byte
                    a16 |= (Bus->read(pc.pc+2) << 8); // get high byte
                    Bus->write(a16, af.bytes.a);
                    pc.pc+=2;
                    cycles+=12;
                    break;
                case 0xb://NO OP
                    break;
                case 0xc://NO OP
                    break;
                case 0xd://NO OP
                    break;
                case 0xe://XOR d8
                    XOR(af.bytes.a, Bus->read(pc.pc+1));
                    pc.pc+=1; //extra
                    break;
                case 0xf://RST 28H
                    RST(0x28);
                    break;
            }
            break;
        
        case 0xf:
            switch(opcodeL)
            {
                case 0x0://LDH A,(a8)
                    af.bytes.a = (Bus->read(pc.pc+1)) + 0xff00;
                    pc.pc+=2;
                    cycles+=12;
                    break;
                case 0x1://POP AF
                    POP_16b(af.bytes.a, af.bytes.f);
                    break;
                case 0x2://LD A,(C)
                    af.bytes.a = (Bus->read(0xff00 + bc.bytes.c));
                    break;
                case 0x3://DI disables interrupts after one instruction after itself TODO needs interrupts
                    break;
                case 0x4:
                    break;//NO OP
                case 0x5://PUSH AF
                    PUSH_16b(af.bytes.a, af.bytes.f);
                    break;
                case 0x6://OR d8
                    OR(af.bytes.a, Bus->read(pc.pc+1));
                    pc.pc+=1;
                    break;
                case 0x7://RST 30H
                    RST(0x30);
                    break;
                case 0x8://LD HL,SP+r8
                    hl.hl = sp.sp + Bus->read(pc.pc+1);
                    pc.pc+=2;
                    cycles+=12;
                    break;
                case 0x9://LD SP,HL
                    sp.sp = hl.hl;
                    pc.pc+=1;
                    cycles+=8;
                    break;
                case 0xa://LD A,(a16)
                    uint16_t a16;
                    a16 |= Bus->read(pc.pc+1);//get low byte
                    a16 |= (Bus->read(pc.pc+2) << 8); // get high byte
                    af.bytes.a = Bus->read(a16);
                    pc.pc+=3;
                    cycles+=16;
                    break;
                case 0xb://EI enables interrupts after one instruction after itself TODO needs interrupts
                    break;
                case 0xc://NO OP
                    break;
                case 0xd://NO OP
                    break;
                case 0xe:
                    CP(af.bytes.a, Bus->read(pc.pc+1));
                    pc.pc+=1;
                    cycles+=4; // extra for this instruction
                    break;
                case 0xf:
                    RST(0x38);
                    break;       
            }
            break;
            
        default:
            printf("INVALID OPCODE\n");
            break;
        }

    }
   
    void cpu::emulateCycles()
    {
        

    }