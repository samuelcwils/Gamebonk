#include "cpu.h"
    
    cpu::cpu(bus* Bus)
    {
        this->Bus = Bus;
        cycles = 0;
        af.af = 0;
        bc.bc = 0;
        de.de = 0;
        sp = 0;
        pc = 0x0150;
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
        if(((a & 0xf) + (b & 0xf)) & 0x10){ //H flag
            af.bytes.f |= 0b01000000;
        } else {
            af.bytes.f &= 0b11011111;
        }
    }
    
    void cpu::Hflag(uint16_t a, uint16_t b)
    {
        if((((a >> 8) & 0xf) + ((b >> 8) & 0xf)) & 0x10){ //H flag
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
        high = Bus->read(pc+1);
        low = Bus->read(pc+2);
        pc+=3;
        cycles+=12;       
    }

    void cpu::LD_d8(uint8_t &byte)
    {
        byte = Bus->read(pc+1);
        pc+=2;
        cycles+=8;
    }

    void cpu::LD_a_b(uint8_t &a, uint8_t b)
    {
        a = b;
        pc+=1;
        cycles+=4;
    }
    
    void cpu::LD_HL_REG(uint8_t reg)
    {
        Bus->write(hl.hl, reg);
        pc+=1;
        cycles+=2;
    }

    void cpu::ADD(uint16_t &a, uint16_t b)
    {
  
        af.bytes.f &= 0b10111111; //N flag
        Hflag(a, b);
        Cflag(a, b);
        a += b;
        pc+=1;
        cycles+=8;
    }
    
    void cpu::ADD(uint8_t &a, uint8_t b)
    {
        Zflag(a, b);
        af.bytes.f &= 0b10111111; //N flag
        Hflag(a, b);
        Cflag(a, b);
        a += b;
        pc+=1;
        cycles+=8;
    }

    void cpu::INC(uint16_t &value)
    {
        value+=1;
        pc+=1;
        cycles+=8;
    }

    void cpu::INC(uint8_t &byte)
    {
        Zflag(byte, 1);
        af.bytes.f &= 0b10111111; //N flag
        Hflag(byte, 1);
        byte+=1;
        pc+=1;
        cycles+=4;
    }

    void cpu::DEC(uint16_t &value)
    {
        value-=1;
        pc+=1;
        cycles+=4;
    }

    void cpu::DEC(uint8_t &byte)
    {
        Zflag(byte, -1);
        af.bytes.f |= 0b01000000;
        Cflag(byte, -1);
        byte-=1;
        pc+=1;
        cycles+=4;
    }

    void cpu::LD_ADDRESS_A(uint16_t address)
    {
        Bus->write(address, af.bytes.a);
        pc+=1;
        cycles+=8;
    }

        void cpu::LD_A_ADDRESS(uint16_t address)
    {
        af.bytes.a = Bus->read(address);
        pc+=1;
        cycles+=8;
    }

    void cpu::execOP()
    {
        uint8_t opcode = Bus->read(pc);
        uint8_t opcodeH = (opcode & 0xF0) >> 4;
        uint8_t opcodeL = opcode & 0x0F;

        switch (opcodeH)
        {
        case 0x0:
            switch(opcodeL)
            {
                case 0x0: //NOP
                    pc+=1;
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
                    
                    pc+=1;
                    cycles+=4;
                    break; 
                }
                case 0x8: //LD a16, SP
                    Bus->write(pc+1, sp & 0x00ff);
                    Bus->write(pc+2, (sp & 0xff00) >> 8);
                    pc+=3;
                    cycles+=20;
                    break;
                case 0x9: //ADD HL, BC (- 0 H C)
                    ADD(hl.hl, bc.bc);
                    break;
                case 0xa: //LD A, BC
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
                case 0xf: //RRCA
                { 
                    bool carry = (af.bytes.a & 0x01);
                    af.bytes.a >>= 1;
                    af.bytes.a += (carry << 8);
                    
                    if(carry){
                        af.bytes.f |= 0b00010000;
                    } else {
                        af.bytes.f &= 0b11101111;
                    }                    
                    
                    pc+=1;
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

                case 0x1://LD DE, d16
                    LD_d16(de.bytes.d, de.bytes.e);
                    break;
                case 0x2://LD DE, a
                    LD_ADDRESS_A(de.bytes.d, )
                case 0x3://INC BC
                    de.de+=1;
                    pc+=1;
                    cycles+=8;
                    break;
                case 0x4://INC B
                    





            }

        default:
            printf("INVALID OPCODE\n");
            pc++;
            break;
        }


    }
   
    void cpu::emulateCycles()
    {
        

    }