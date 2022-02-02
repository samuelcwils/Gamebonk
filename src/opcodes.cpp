#include "cpu.h"

void cpu::Zflag(uint16_t a, int b)
    {
        if(!(a+b)){ //Z flag
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;
        }     
    }

    void cpu::Hflag(uint8_t a, uint8_t b)
    {
        if(((a & 0x0f) + (b & 0x0f)) & 0x10){
            af.bytes.f |= 0b00100000;
        } else {
            af.bytes.f &= 0b11011111;
        }
    }
    
    void cpu::Hflag(uint16_t a, uint16_t b)
    {
        if((((a >> 8) & 0xf) + ((b >> 8) & 0xf)) & 0x10){ 
            af.bytes.f |= 0b00100000;
        } else {
            af.bytes.f &= 0b11011111;
        }
    }

    void cpu::Hflag_sub(uint8_t a, uint8_t b)
    {
        if((((a & 0xf) - (b & 0xf)))  < 0){
            af.bytes.f &= 0b11011111;
        } else {
            af.bytes.f |= 0b00100000;
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

    void cpu::Cflag_sub(uint8_t a, uint8_t b)
    {
        int temp = (a - b) >> 8;
        if(temp){
            af.bytes.f &= 0b11101111;
        } else{
            af.bytes.f |= 0b00010000;
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
        pc.pc += (signed char)Bus->read(pc.pc+1) + 2;
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
        uint16_t temp = 0;
        temp = pc.pc + 3;
        
        sp.sp--;
        Bus->write(sp.sp, temp & 0xff00);
        sp.sp--;
        Bus->write(sp.sp, temp & 0x00ff);
        
        temp = pc.pc;

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
        Hflag_sub(a, b);
        Cflag_sub(a, b);
        a -= b;
        pc.pc+=1;
        cycles+=4;      
    }

    void cpu::SBC(uint8_t &a, uint8_t b)
    {
        uint8_t temp = (b + (af.bytes.f & 0x00010000));
        Zflag(a, -temp);
        af.bytes.f |= 0b01000000; //N flag
        Hflag_sub(a, temp);
        Cflag_sub(a, temp);
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
        if(a == b){
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;
        }
        af.bytes.f |= 0b01000000; //N flag
        Hflag_sub(a, b);
        if(a < b)
        {
            af.bytes.f |= 0b00010000;
        }
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
        Hflag_sub(byte, 1);
        byte-=1;
        pc.pc+=1;
        cycles+=4;
    }
    
    void cpu::RLC(uint8_t &byte)
    {
        bool carry = byte & 0b10000000;
        byte <<= 1;
        byte += carry;
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }  

        af.bytes.f &= 0b11011111;  

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }                    
        
        pc.pc+=1;
        cycles+=4;        
    }

    void cpu::RRC(uint8_t &byte)
    {
        bool carry = byte & 0b00000001;
        byte >>= 1;
        byte += (carry << 8);
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }  

        af.bytes.f &= 0b11011111;  

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }                    
        
        pc.pc+=1;
        cycles+=4;
    }
    
    void cpu::RL(uint8_t &byte)
    {
        bool carry = byte & 0b10000000;
        byte <<= 1;
        byte += ((af.bytes.f & 0b00010000) >> 4);
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        } 
       
        af.bytes.f &= 0b11011111;   

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }                   
        
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::RR(uint8_t &byte)
    {
        bool carry = byte & 0b00000001;
        byte >>= 1;
        byte += ((af.bytes.f & 0b00010000) << 3);
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        } 

        af.bytes.f &= 0b11011111;  

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }                   
        
        pc.pc+=1;
        cycles+=4;          
    }

    void cpu::SLA(uint8_t &byte)
    {
        bool carry = byte & 0b10000000;
        byte <<= 1;
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }   

        af.bytes.f &= 0b11010000;  

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }               
        
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::SRA(uint8_t &byte)
    {
        bool carry = byte & 0b00000001;
        byte >>= 1;
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }  

        af.bytes.f &= 0b11011111;    
        
        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }             
        
        pc.pc+=1;
        cycles+=4;       
    }

    void cpu::SWAP(uint8_t &byte)
    {
        uint8_t newLow = (byte & 0xf0) >> 4;
        byte <<= 4;
        byte |= newLow; 
        
        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }

        af.bytes.f &= 0b10001111;  

    }

    void cpu::SRL(uint8_t &byte)
    {
        bool carry = byte & 0b00000001;
        byte >>= 1;
        byte & 0b00000001;
        
        if(carry){
            af.bytes.f |= 0b00010000;
        } else {
            af.bytes.f &= 0b11101111;
        }       
        
        af.bytes.f &= 0b11010000;

        if(!byte)
        {
            af.bytes.f |= 0b10000000;
        }  else {
            af.bytes.f &= 0b01111111;
        }             
        
        pc.pc+=1;
        cycles+=4; 
    }

    void cpu::BIT(uint8_t &byte, uint8_t bitNum)
    {
        if(!(byte & bitNum))
        {
            af.bytes.f |= 0b10000000;
        } else {
            af.bytes.f &= 0b01111111;  
        }

        af.bytes.f &= 0b10111111;
        af.bytes.f |= 0b00100000;  

        pc.pc+=1;
        cycles+=4;
    }

    void cpu::RES(uint8_t &byte, uint8_t bitNum)
    {
        byte &= !bitNum;
        pc.pc+=1;
        cycles+=4;
    }


    void cpu::SET(uint8_t &byte, uint8_t bitNum)
    {
        byte |= bitNum;
        pc.pc+=1;
        cycles+=4;
    }

    void cpu::execOP()
    {
        //////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
        uint8_t opcode = Bus->read(pc.pc);
        uint8_t opcodeH = (opcode & 0xF0) >> 4;
        uint8_t opcodeL = opcode & 0x0F;
        
        if(debug)
        {
            printf("%04x : \t", pc.pc); //print pc
            printf("af: %04x bc: %04x de: %04x hl: %04x sp: %04x", af.af, bc.bc, de.de, hl.hl, sp.sp); //print regs
            printf(" z: %i, n: %i, h: %i, c: %i\n", af.bytes.f & 0b10000000, af.bytes.f & 0b01000000, af.bytes.f & 0b00100000, af.bytes.f & 0b00010000);
            printf("\t %0x\n", opcode);
        }

        if(pc.pc == 0x00fa)
        {
            bootromDone = true;
        }

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
                        RLC(af.bytes.a);
                        break;
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
                        break;
                    case 0xf: //RRCA (0 0 0 A0)
                        RRC(af.bytes.a);
                        break;

                }
                break;
            
            case 0x1:
                switch(opcodeL)
                {
                    case 0x0://STOP
                        for(int i = 0; i == 4; i++)
                        {
                            if(((IE & (0b00000001 << i))) && (IF & (0b00000001 << i)))
                            {
                                pc.pc+=1;
                                cycles+=4;
                            } else {
                                break;
                            }
                        }    

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
                        RL(af.bytes.a);
                        break;
                    case 0x8://JR s8
                        JR();
                        break;
                    case 0x9://ADD HL, DE
                        ADD(hl.hl, de.de);
                        break;
                    case 0xa://LD A, (DE)
                        LD_A_ADDRESS(de.de);
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
                        RL(af.bytes.a);
                        break;
                }
                break;
        
            case 0x2:
                switch(opcodeL)
                {
                    case 0x0://JR NZ,r8
                        JR_cond(!(af.bytes.f & 0b10000000));
                        break;
                    case 0x1://LD HL, d16
                        LD_d16(hl.bytes.h, hl.bytes.l);
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
                        pc.pc+=1;
                        cycles+=4;
                        break;
                }
                break;

            case 0x3:
                switch(opcodeL)
                {
                    case 0x0://JR NC,r8
                        JR_cond(!(af.bytes.f & 0b10000000));
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
                        Hflag_sub(temp, 1);
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
                    case 0x6://HALT
                        for(int i = 0; i == 4; i++)
                        {
                            if(((IE & (0b00000001 << i))) && (IF & (0b00000001 << i)))
                            {
                                pc.pc+=1;
                                cycles+=4;
                            } else {
                                cycles+=4;
                                break;
                            }
                        }

                        break;
                    case 0x7://LD (HL),A
                        LD_HL_REG(af.bytes.a);
                        break;
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
                    {
                        uint16_t temp = pc.pc;
                        pc.bytes.c = Bus->read(temp+1);
                        pc.bytes.p = Bus->read(temp+2);
                        cycles+=4;
                        break;
                    }
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
                        pc.pc+=1;
                        cycles+=4;
                        goto cb;
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
                    case 0x9://RETI
                        IME = 1;
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
                        pc.pc+=1;
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
                    {
                        uint16_t a16 = 0;
                        a16 |= Bus->read(pc.pc+1);//get low byte
                        a16 |= (Bus->read(pc.pc+2) << 8); // get high byte
                        Bus->write(a16, af.bytes.a);
                        pc.pc+=3;
                        cycles+=12;
                        break;
                    }
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
                        af.bytes.a = (Bus->read( (Bus->read(pc.pc+1)) + 0xff00) );
                        pc.pc+=2;
                        cycles+=12;
                        break;
                    case 0x1://POP AF
                        POP_16b(af.bytes.a, af.bytes.f);
                        break;
                    case 0x2://LD A,(C)
                        af.bytes.a = (Bus->read(0xff00 + bc.bytes.c));
                        break;
                    case 0x3://DI
                        IME = 0;
                        pc.pc+=1;
                        cycles+=4;
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
                    case 0xb://EI
                        IME = 1;
                        pc.pc+=1;
                        cycles+=4;
                        break;
                    case 0xc://NO OP
                        break;
                    case 0xd://NO OP
                        break;
                    case 0xe://CP d8
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

        return; // stop executing to prevent from spilling into the CB instructions

        cb:
        opcode = Bus->read(pc.pc);//gets new opcode
        opcodeH = (opcode & 0xF0) >> 4;
        opcodeL = opcode & 0x0F;

        switch(opcodeH)
        {
            case 0x0:
                switch(opcodeL)
                {
                    case 0x0://RLC B (Z 0 0 REG7)
                        RLC(bc.bytes.b);
                        break;
                    case 0x1://RLC C (Z 0 0 REG7)
                        RLC(bc.bytes.c);
                        break;
                    case 0x2://RLC D (Z 0 0 REG7)
                        RLC(de.bytes.d);
                        break;
                    case 0x3://RLC E (Z 0 0 REG7)
                        RLC(de.bytes.e);
                        break;
                    case 0x4://RLC H (Z 0 0 REG7)
                        RLC(hl.bytes.h);
                        break;
                    case 0x5://RLC L (Z 0 0 REG7)
                        RLC(hl.bytes.l);
                        break;
                    case 0x6://RLC (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RLC(temp);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://RLC A (Z 0 0 REG7)
                        RLC(af.bytes.a);
                        break;
                    case 0x8://RRC B (Z 0 0 0)
                        RRC(bc.bytes.b);
                        break;
                    case 0x9://RRC C (Z 0 0 0)
                        RRC(bc.bytes.c);
                        break;
                    case 0xa://RRC D (Z 0 0 0)
                        RRC(de.bytes.d);
                        break;
                    case 0xb://RRC E (Z 0 0 0)
                        RRC(de.bytes.e);
                        break;
                    case 0xc://RRC H (Z 0 0 0)
                        RRC(hl.bytes.h);
                        break;
                    case 0xd://RRC L (Z 0 0 0)
                        RRC(hl.bytes.l);
                        break;
                    case 0xe://RRC (HL) (Z 0 0 0)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RRC(temp);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://RRC A (Z 0 0 0)
                        RRC(af.bytes.a);
                        break;
                }
                break;
                
            case 0x1:
                switch(opcodeL)
                {
                    case 0x0://RL B (Z 0 0 REG7)
                        RL(bc.bytes.b);
                        break;
                    case 0x1://RL C (Z 0 0 REG7)
                        RL(bc.bytes.c);
                        break;
                    case 0x2://RL D (Z 0 0 REG7)
                        RL(de.bytes.d);
                        break;
                    case 0x3://RL E (Z 0 0 REG7)
                        RL(de.bytes.e);
                        break;
                    case 0x4://RL H (Z 0 0 REG7)
                        RL(hl.bytes.h);
                        break;
                    case 0x5://RL L (Z 0 0 REG7)
                        RL(hl.bytes.l);
                        break;
                    case 0x6://RL (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RL(temp);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://RL A (Z 0 0 REG7)
                        RL(af.bytes.a);
                        break;
                    case 0x8://RR B (Z 0 0 REG7)
                        RR(bc.bytes.b);
                        break;
                    case 0x9://RL C (Z 0 0 REG7)
                        RR(bc.bytes.c);
                        break;
                    case 0xa://RR D (Z 0 0 REG7)
                        RR(de.bytes.d);
                        break;
                    case 0xb://RR E (Z 0 0 REG7)
                        RR(de.bytes.e);
                        break;
                    case 0xc://RR H (Z 0 0 REG7)
                        RR(hl.bytes.h);
                        break;
                    case 0xd://RR L (Z 0 0 REG7)
                        RR(hl.bytes.l);
                        break;
                    case 0xe://RR (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RR(temp);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://RR A (Z 0 0 REG7)
                        RR(af.bytes.a);
                        break;
                }
                break;

            case 0x2:
                switch(opcodeL)
                {
                    case 0x0://SLA B (Z 0 0 REG7)
                        SLA(bc.bytes.b);
                        break;
                    case 0x1://SLA C (Z 0 0 REG7)
                        SLA(bc.bytes.c);
                        break;
                    case 0x2://SLA D (Z 0 0 REG7)
                        SLA(de.bytes.d);
                        break;
                    case 0x3://SLA E (Z 0 0 REG7)
                        SLA(de.bytes.e);
                        break;
                    case 0x4://SLA H (Z 0 0 REG7)
                        SLA(hl.bytes.h);
                        break;
                    case 0x5://SLA L (Z 0 0 REG7)
                        SLA(hl.bytes.l);
                        break;
                    case 0x6://SLA (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SLA(temp);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://SLA A (Z 0 0 REG7)
                        SLA(af.bytes.a);
                        break;
                    case 0x8://SRA B (Z 0 0 0)
                        SRA(bc.bytes.b);
                        break;
                    case 0x9://SRA C (Z 0 0 0)
                        SRA(bc.bytes.c);
                        break;
                    case 0xa://SRA D (Z 0 0 0)
                        RR(de.bytes.d);
                        break;
                    case 0xb://SRA E (Z 0 0 0)
                        SRA(de.bytes.e);
                        break;
                    case 0xc://SRA H (Z 0 0 0)
                        SRA(hl.bytes.h);
                        break;
                    case 0xd://SRA L (Z 0 0 0)
                        SRA(hl.bytes.l);
                        break;
                    case 0xe://SRA (HL) (Z 0 0 0)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SRA(temp);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://SRA A (Z 0 0 0)
                        SRA(af.bytes.a);
                        break;
                }
                break;

            case 0x3:
                switch(opcodeL)
                {
                    case 0x0://SWAP B (Z 0 0 REG7)
                        SWAP(bc.bytes.b);
                        break;
                    case 0x1://SWAP C (Z 0 0 REG7)
                        SWAP(bc.bytes.c);
                        break;
                    case 0x2://SWAP D (Z 0 0 REG7)
                        SWAP(de.bytes.d);
                        break;
                    case 0x3://SWAP E (Z 0 0 REG7)
                        SWAP(de.bytes.e);
                        break;
                    case 0x4://SWAP H (Z 0 0 REG7)
                        SWAP(hl.bytes.h);
                        break;
                    case 0x5://SWAP L (Z 0 0 REG7)
                        SWAP(hl.bytes.l);
                        break;
                    case 0x6://SWAP (HL) (Z 0 0 REG7)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SWAP(temp);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://SWAP A (Z 0 0 REG7)
                        SWAP(af.bytes.a);
                        break;
                    case 0x8://SRL B (Z 0 0 0)
                        SRL(bc.bytes.b);
                        break;
                    case 0x9://SRL C (Z 0 0 0)
                        SRL(bc.bytes.c);
                        break;
                    case 0xa://SRL D (Z 0 0 0)
                        SRL(de.bytes.d);
                        break;
                    case 0xb://SRL E (Z 0 0 0)
                        SRL(de.bytes.e);
                        break;
                    case 0xc://SRL H (Z 0 0 0)
                        SRL(hl.bytes.h);
                        break;
                    case 0xd://SRL L (Z 0 0 0)
                        SRL(hl.bytes.l);
                        break;
                    case 0xe://SRL (HL) (Z 0 0 0)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SRL(temp);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://SRL A (Z 0 0 0)
                        SRL(af.bytes.a);
                        break;
                }
                break;

            case 0x4:
                switch(opcodeL)
                {
                    case 0x0://BIT 0,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00000001);
                        break;
                    case 0x1://BIT 0,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00000001);
                        break;
                    case 0x2://BIT 0,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00000001);
                        break;
                    case 0x3://BIT 0,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00000001);
                        break;
                    case 0x4://BIT 0,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00000001);
                        break;
                    case 0x5://BIT 0,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00000001);
                        break;
                    case 0x6://BIT 0,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00000001);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://BIT 0,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00000001);
                        break;
                    case 0x8://BIT 1,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00000010);
                        break;
                    case 0x9://BIT 1,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00000010);
                        break;
                    case 0xa://BIT 1,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00000010);
                        break;
                    case 0xb://BIT 1,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00000010);
                        break;
                    case 0xc://BIT 1,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00000010);
                        break;
                    case 0xd://BIT 1,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00000010);
                        break;
                    case 0xe://BIT 1,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00000010);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://BIT 1,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00000010);
                        break;
                }
                break;

            case 0x5:
                switch(opcodeL)
                {
                    case 0x0://BIT 2,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00000100);
                        break;
                    case 0x1://BIT 2,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00000100);
                        break;
                    case 0x2://BIT 2,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00000100);
                        break;
                    case 0x3://BIT 2,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00000100);
                        break;
                    case 0x4://BIT 2,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00000100);
                        break;
                    case 0x5://BIT 2,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00000100);
                        break;
                    case 0x6://BIT 2,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00000100);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://BIT 2,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00000100);
                        break;
                    case 0x8://BIT 3,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00001000);
                        break;
                    case 0x9://BIT 3,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00001000);
                        break;
                    case 0xa://BIT 3,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00001000);
                        break;
                    case 0xb://BIT 3,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00001000);
                        break;
                    case 0xc://BIT 3,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00001000);
                        break;
                    case 0xd://BIT 3,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00001000);
                        break;
                    case 0xe://BIT 3,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00001000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://BIT 3,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00001000);
                        break;
                }
                break;

            case 0x6:
                switch(opcodeL)
                {
                    case 0x0://BIT 4,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00010000);
                        break;
                    case 0x1://BIT 4,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00010000);
                        break;
                    case 0x2://BIT 4,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00010000);
                        break;
                    case 0x3://BIT 4,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00010000);
                        break;
                    case 0x4://BIT 4,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00010000);
                        break;
                    case 0x5://BIT 4,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00010000);
                        break;
                    case 0x6://BIT 4,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00010000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://BIT 4,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00010000);
                        break;
                    case 0x8://BIT 5,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b00100000);
                        break;
                    case 0x9://BIT 5,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b00100000);
                        break;
                    case 0xa://BIT 5,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b00100000);
                        break;
                    case 0xb://BIT 5,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b00100000);
                        break;
                    case 0xc://BIT 5,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b00100000);
                        break;
                    case 0xd://BIT 5,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b00100000);
                        break;
                    case 0xe://BIT 5,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b00100000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://BIT 5,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b00100000);
                        break;
                }
                break;

            case 0x7:
                switch(opcodeL)
                {
                    case 0x0://BIT 6,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b01000000);
                        break;
                    case 0x1://BIT 6,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b01000000);
                        break;
                    case 0x2://BIT 6,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b01000000);
                        break;
                    case 0x3://BIT 6,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b01000000);
                        break;
                    case 0x4://BIT 6,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b01000000);
                        break;
                    case 0x5://BIT 6,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b01000000);
                        break;
                    case 0x6://BIT 6,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b01000000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://BIT 6,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b01000000);
                        break;
                    case 0x8://BIT 7,B (Z 0 1 -)
                        BIT(bc.bytes.b, 0b10000000);
                        break;
                    case 0x9://BIT 7,C (Z 0 1 -)
                        BIT(bc.bytes.c, 0b10000000);
                        break;
                    case 0xa://BIT 7,D (Z 0 1 -)
                        BIT(de.bytes.d, 0b10000000);
                        break;
                    case 0xb://BIT 7,E (Z 0 1 -)
                        BIT(de.bytes.e, 0b10000000);
                        break;
                    case 0xc://BIT 7,H (Z 0 1 -)
                        BIT(hl.bytes.h, 0b10000000);
                        break;
                    case 0xd://BIT 7,L (Z 0 1 -)
                        BIT(hl.bytes.l, 0b10000000);
                        break;
                    case 0xe://BIT 7,(HL) (Z 0 1 -)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        BIT(temp, 0b10000000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://BIT 7,A (Z 0 1 -)
                        BIT(af.bytes.a, 0b10000000);
                        break;
                }
                break;

            case 0x8:
                switch(opcodeL)
                {
                    case 0x0://RES 0,B
                        RES(bc.bytes.b, 0b00000001);
                        break;
                    case 0x1://RES 0,C
                        RES(bc.bytes.c, 0b00000001);
                        break;
                    case 0x2://RES 0,D
                        RES(de.bytes.d, 0b00000001);
                        break;
                    case 0x3://RES 0,E
                        RES(de.bytes.e, 0b00000001);
                        break;
                    case 0x4://RES 0,H
                        RES(hl.bytes.h, 0b00000001);
                        break;
                    case 0x5://RES 0,L
                        RES(hl.bytes.l, 0b00000001);
                        break;
                    case 0x6://RES 0,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00000001);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://RES 0,A
                        RES(af.bytes.a, 0b00000001);
                        break;
                    case 0x8://RES 1,B
                        RES(bc.bytes.b, 0b00000010);
                        break;
                    case 0x9://RES 1,C
                        RES(bc.bytes.c, 0b00000010);
                        break;
                    case 0xa://RES 1,D
                        RES(de.bytes.d, 0b00000010);
                        break;
                    case 0xb://RES 1,E
                        RES(de.bytes.e, 0b00000010);
                        break;
                    case 0xc://RES 1,H
                        RES(hl.bytes.h, 0b00000010);
                        break;
                    case 0xd://RES 1,L
                        RES(hl.bytes.l, 0b00000010);
                        break;
                    case 0xe://RES 1,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00000010);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://RES 1,A
                        RES(af.bytes.a, 0b00000010);
                        break;
                }

            case 0x9:
                switch(opcodeL)
                {
                    case 0x0://RES 2,B
                        RES(bc.bytes.b, 0b00000100);
                        break;
                    case 0x1://RES 2,C
                        RES(bc.bytes.c, 0b00000100);
                        break;
                    case 0x2://RES 2,D
                        RES(de.bytes.d, 0b00000100);
                        break;
                    case 0x3://RES 2,E
                        RES(de.bytes.e, 0b00000100);
                        break;
                    case 0x4://RES 2,H
                        RES(hl.bytes.h, 0b00000100);
                        break;
                    case 0x5://RES 2,L
                        RES(hl.bytes.l, 0b00000100);
                        break;
                    case 0x6://RES 2,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00000100);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://RES 2,A
                        RES(af.bytes.a, 0b00000100);
                        break;
                    case 0x8://RES 3,B
                        RES(bc.bytes.b, 0b00001000);
                        break;
                    case 0x9://RES 3,C
                        RES(bc.bytes.c, 0b00001000);
                        break;
                    case 0xa://RES 3,D
                        RES(de.bytes.d, 0b00001000);
                        break;
                    case 0xb://RES 3,E
                        RES(de.bytes.e, 0b00001000);
                        break;
                    case 0xc://RES 3,H
                        RES(hl.bytes.h, 0b00001000);
                        break;
                    case 0xd://RES 3,L
                        RES(hl.bytes.l, 0b00001000);
                        break;
                    case 0xe://RES 3,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00001000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://RES 3,A
                        RES(af.bytes.a, 0b00001000);
                        break;
                }

            case 0xa:
                switch(opcodeL)
                {
                    case 0x0://RES 4,B
                        RES(bc.bytes.b, 0b00010000);
                        break;
                    case 0x1://RES 4,C
                        RES(bc.bytes.c, 0b00010000);
                        break;
                    case 0x2://RES 4,D
                        RES(de.bytes.d, 0b00010000);
                        break;
                    case 0x3://RES 4,E
                        RES(de.bytes.e, 0b00010000);
                        break;
                    case 0x4://RES 4,H
                        RES(hl.bytes.h, 0b00010000);
                        break;
                    case 0x5://RES 4,L
                        RES(hl.bytes.l, 0b00010000);
                        break;
                    case 0x6://RES 4,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00010000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://RES 4,A
                        RES(af.bytes.a, 0b00010000);
                        break;
                    case 0x8://RES 5,B
                        RES(bc.bytes.b, 0b00100000);
                        break;
                    case 0x9://RES 5,C
                        RES(bc.bytes.c, 0b00100000);
                        break;
                    case 0xa://RES 5,D
                        RES(de.bytes.d, 0b00100000);
                        break;
                    case 0xb://RES 5,E
                        RES(de.bytes.e, 0b00100000);
                        break;
                    case 0xc://RES 5,H
                        RES(hl.bytes.h, 0b00100000);
                        break;
                    case 0xd://RES 5,L
                        RES(hl.bytes.l, 0b00100000);
                        break;
                    case 0xe://RES 5,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b00100000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://RES 5,A
                        RES(af.bytes.a, 0b00100000);
                        break;
                }

            case 0xb:
                switch(opcodeL)
                {
                    case 0x0://RES 6,B
                        RES(bc.bytes.b, 0b01000000);
                        break;
                    case 0x1://RES 6,C
                        RES(bc.bytes.c, 0b01000000);
                        break;
                    case 0x2://RES 6,D
                        RES(de.bytes.d, 0b01000000);
                        break;
                    case 0x3://RES 6,E
                        RES(de.bytes.e, 0b01000000);
                        break;
                    case 0x4://RES 6,H
                        RES(hl.bytes.h, 0b01000000);
                        break;
                    case 0x5://RES 6,L
                        RES(hl.bytes.l, 0b01000000);
                        break;
                    case 0x6://RES 6,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b01000000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://RES 6,A
                        RES(af.bytes.a, 0b01000000);
                        break;
                    case 0x8://RES 7,B
                        RES(bc.bytes.b, 0b10000000);
                        break;
                    case 0x9://RES 7,C
                        RES(bc.bytes.c, 0b10000000);
                        break;
                    case 0xa://RES 7,D
                        RES(de.bytes.d, 0b10000000);
                        break;
                    case 0xb://RES 7,E
                        RES(de.bytes.e, 0b10000000);
                        break;
                    case 0xc://RES 7,H
                        RES(hl.bytes.h, 0b10000000);
                        break;
                    case 0xd://RES 7,L
                        RES(hl.bytes.l, 0b10000000);
                        break;
                    case 0xe://RES 7,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        RES(temp, 0b10000000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://RES 7,A
                        RES(af.bytes.a, 0b10000000);
                        break;
                }

            case 0xc:
                switch(opcodeL)
                {
                    case 0x0://SET 0,B
                        SET(bc.bytes.b, 0b00000001);
                        break;
                    case 0x1://SET 0,C
                        SET(bc.bytes.c, 0b00000001);
                        break;
                    case 0x2://SET 0,D
                        SET(de.bytes.d, 0b00000001);
                        break;
                    case 0x3://SET 0,E
                        SET(de.bytes.e, 0b00000001);
                        break;
                    case 0x4://SET 0,H
                        SET(hl.bytes.h, 0b00000001);
                        break;
                    case 0x5://SET 0,L
                        SET(hl.bytes.l, 0b00000001);
                        break;
                    case 0x6://SET 0,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00000001);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://SET 0,A
                        SET(af.bytes.a, 0b00000001);
                        break;
                    case 0x8://SET 1,B
                        SET(bc.bytes.b, 0b00000010);
                        break;
                    case 0x9://SET 1,C
                        SET(bc.bytes.c, 0b00000010);
                        break;
                    case 0xa://SET 1,D
                        SET(de.bytes.d, 0b00000010);
                        break;
                    case 0xb://SET 1,E
                        SET(de.bytes.e, 0b00000010);
                        break;
                    case 0xc://SET 1,H
                        SET(hl.bytes.h, 0b00000010);
                        break;
                    case 0xd://SET 1,L
                        SET(hl.bytes.l, 0b00000010);
                        break;
                    case 0xe://SET 1,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00000010);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://SET 1,A
                        SET(af.bytes.a, 0b00000010);
                        break;
                }

            case 0xd:
                switch(opcodeL)
                {
                    case 0x0://SET 2,B
                        SET(bc.bytes.b, 0b00000100);
                        break;
                    case 0x1://SET 2,C
                        SET(bc.bytes.c, 0b00000100);
                        break;
                    case 0x2://SET 2,D
                        SET(de.bytes.d, 0b00000100);
                        break;
                    case 0x3://SET 2,E
                        SET(de.bytes.e, 0b00000100);
                        break;
                    case 0x4://SET 2,H
                        SET(hl.bytes.h, 0b00000100);
                        break;
                    case 0x5://SET 2,L
                        SET(hl.bytes.l, 0b00000100);
                        break;
                    case 0x6://SET 2,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00000100);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://SET 2,A
                        SET(af.bytes.a, 0b00000100);
                        break;
                    case 0x8://SET 3,B
                        SET(bc.bytes.b, 0b00001000);
                        break;
                    case 0x9://SET 3,C
                        SET(bc.bytes.c, 0b00001000);
                        break;
                    case 0xa://SET 3,D
                        SET(de.bytes.d, 0b00001000);
                        break;
                    case 0xb://SET 3,E
                        SET(de.bytes.e, 0b00001000);
                        break;
                    case 0xc://SET 3,H
                        SET(hl.bytes.h, 0b00001000);
                        break;
                    case 0xd://SET 3,L
                        SET(hl.bytes.l, 0b00001000);
                        break;
                    case 0xe://SET 3,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00001000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://SET 3,A
                        SET(af.bytes.a, 0b00001000);
                        break;
                }

            case 0xe:
                switch(opcodeL)
                {
                    case 0x0://SET 4,B
                        SET(bc.bytes.b, 0b00010000);
                        break;
                    case 0x1://SET 4,C
                        SET(bc.bytes.c, 0b00010000);
                        break;
                    case 0x2://SET 4,D
                        SET(de.bytes.d, 0b00010000);
                        break;
                    case 0x3://SET 4,E
                        SET(de.bytes.e, 0b00010000);
                        break;
                    case 0x4://SET 4,H
                        SET(hl.bytes.h, 0b00010000);
                        break;
                    case 0x5://SET 4,L
                        SET(hl.bytes.l, 0b00010000);
                        break;
                    case 0x6://SET 4,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00010000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://SET 4,A
                        SET(af.bytes.a, 0b00010000);
                        break;
                    case 0x8://SET 5,B
                        SET(bc.bytes.b, 0b00100000);
                        break;
                    case 0x9://SET 5,C
                        SET(bc.bytes.c, 0b00100000);
                        break;
                    case 0xa://SET 5,D
                        SET(de.bytes.d, 0b00100000);
                        break;
                    case 0xb://SET 5,E
                        SET(de.bytes.e, 0b00100000);
                        break;
                    case 0xc://SET 5,H
                        SET(hl.bytes.h, 0b00100000);
                        break;
                    case 0xd://SET 5,L
                        SET(hl.bytes.l, 0b00100000);
                        break;
                    case 0xe://SET 5,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b00100000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://SET 5,A
                        SET(af.bytes.a, 0b00100000);
                        break;
                }

            case 0xf:
                switch(opcodeL)
                {
                    case 0x0://SET 6,B
                        SET(bc.bytes.b, 0b01000000);
                        break;
                    case 0x1://SET 6,C
                        SET(bc.bytes.c, 0b01000000);
                        break;
                    case 0x2://SET 6,D
                        SET(de.bytes.d, 0b01000000);
                        break;
                    case 0x3://SET 6,E
                        SET(de.bytes.e, 0b01000000);
                        break;
                    case 0x4://SET 6,H
                        SET(hl.bytes.h, 0b01000000);
                        break;
                    case 0x5://SET 6,L
                        SET(hl.bytes.l, 0b01000000);
                        break;
                    case 0x6://SET 6,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b01000000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0x7://SET 6,A
                        SET(af.bytes.a, 0b01000000);
                        break;
                    case 0x8://SET 7,B
                        SET(bc.bytes.b, 0b10000000);
                        break;
                    case 0x9://SET 7,C
                        SET(bc.bytes.c, 0b10000000);
                        break;
                    case 0xa://SET 7,D
                        SET(de.bytes.d, 0b10000000);
                        break;
                    case 0xb://SET 7,E
                        SET(de.bytes.e, 0b10000000);
                        break;
                    case 0xc://SET 7,H
                        SET(hl.bytes.h, 0b10000000);
                        break;
                    case 0xd://SET 7,L
                        SET(hl.bytes.l, 0b10000000);
                        break;
                    case 0xe://SET 7,(HL)
                    {
                        uint8_t temp = Bus->read(hl.hl);
                        SET(temp, 0b10000000);
                        Bus->write(hl.hl, temp);
                        cycles+=8; //extra time for hl
                        break;
                    }
                    case 0xf://SET 7,A
                        SET(af.bytes.a, 0b10000000);
                        break;
                }
                
                break;
        }

    }