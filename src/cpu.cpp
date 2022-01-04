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
                    bc.bytes.b = Bus->read(pc+1);
                    bc.bytes.c = Bus->read(pc+2);
                    pc+=3;
                    cycles+=12;
                    break;                
                case 0x2: //LD BC,A
                    bc.bc = af.bytes.a;
                    pc+=1;
                    break;
                case 0x3: //INC BC
                    bc.bc+=1;
                    pc+=1;
                    cycles+=8;
                    break;
                case 0x4: //INC B (Z 0 H -) 
                    Zflag(bc.bytes.b, 1);
                    af.bytes.f &= 0b01000000; //N flag
                    Hflag(bc.bytes.b, 1);
                    bc.bytes.b+=1;
                    pc+=1;
                    cycles+=4;
                    break;
                case 0x5: //DEC B (Z 1 H -)
                    Zflag(bc.bytes.b, -1);
                    af.bytes.f &= 0b01000000; //N flag
                    Hflag(bc.bytes.b, -1);
                    bc.bytes.b-=1;
                    pc+=1;
                    cycles+=4;
                    break;
                case 0x6: //LD B,d8
                    bc.bytes.b = Bus->read(pc+1);
                    pc+=2;
                    cycles+=8;
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
                    Hflag(hl.hl, bc.bc);
                    af.bytes.f &= 0b00000000; //N flag
                    Cflag(hl.hl, bc.bc);
                    hl.hl += bc.bc;
                    pc+=1;
                    cycles+=8;
                    break;
                case 0xa: //LD A, BC
                    af.bytes.a = Bus->read(bc.bc);
                    pc+=1;
                    cycles+=8;
                    break;
                case 0xb: //DEC BC
                    bc.bc-=1;
                    pc+=1;
                    cycles+=4;
                    break;
                case 0xc: //INC C
                    bc.bytes.c+=1;
                    pc+=1;
                    cycles+=4;





            




                    





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