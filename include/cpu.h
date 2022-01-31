#pragma once
#include <stdint.h>
#include "bus.h"

class bus;
class cpu {

private:
    bus* Bus; 

    void emulateCycles();
    void Zflag(uint16_t a, int b); 
    void Hflag(uint8_t a, uint8_t b); 
    void Hflag(uint16_t a, uint16_t b); 
    void Hflag_sub(uint8_t a, uint8_t b);
    void Cflag(uint8_t a, uint8_t b);
    void Cflag(uint16_t a, uint16_t b);
    void Cflag_sub(uint8_t a, uint8_t b);

    void LD_d16(uint8_t &high, uint8_t &low);
    void LD_d8(uint8_t &byte);
    void LD_REG1_REG2(uint8_t &a, uint8_t b);
    void LD_HL_REG(uint8_t reg);
    void LD_REG_HL(uint8_t &reg);
    void LD_ADDRESS_A(uint16_t address);
    void LD_A_ADDRESS(uint16_t address);
    
    void JP_a16();
    void JR();
    void JP_cond(bool flag);
    void JR_cond(bool flag);
    
    void POP_16b(uint8_t &high, uint8_t &low);
    void PUSH_16b(uint8_t high, uint8_t low);
    void CALL();
    void CALL_cond(bool flag);

    void RST(uint8_t H);
    void RET();
    void RET_cond(bool flag);
    
    void ADD(uint16_t &a, uint16_t b);
    void ADD(uint8_t &a, uint8_t b);
    void ADC(uint8_t &a, uint8_t b);
    void SUB(uint8_t &a, uint8_t b);
    void SBC(uint8_t &a, uint8_t b);
    
    void AND(uint8_t &a, uint8_t b);
    void XOR(uint8_t &a, uint8_t b);
    void OR(uint8_t &a, uint8_t b);
    void CP(uint8_t &a, uint8_t b);

    void INC(uint16_t &value);
    void INC(uint8_t &byte);
    void DEC(uint16_t &value);
    void DEC(uint8_t &byte);
    
    void RLC(uint8_t &byte);
    void RRC(uint8_t &byte);
    void RL(uint8_t &byte);
    void RR(uint8_t &byte);
    void SLA(uint8_t &byte);
    void SRA(uint8_t &byte);
    void SWAP(uint8_t &byte);
    void SRL(uint8_t &byte);
    
    void BIT(uint8_t &byte, uint8_t bitNum); //bitNum should be entered as binary (ex: bit 5 is 0b00100000 )
    
    void RES(uint8_t &byte, uint8_t bitNum); //bitNum should be entered as binary (ex: bit 5 is 0b00100000 )
    void SET(uint8_t &byte, uint8_t bitNum); //bitNum should be entered as binary (ex: bit 5 is 0b00100000 )

    union {
        struct{ 
        uint8_t f;
        uint8_t a;
        } bytes;
        uint16_t af;
    } af; //register af
    
    union {
        struct{ 
        uint8_t c;
        uint8_t b;
        } bytes;
        uint16_t bc;
    } bc; //register bc
    
    union {
        struct{ 
        uint8_t e;
        uint8_t d;
        } bytes;
        uint16_t de;
    } de; //register de 

    union {
        struct{ 
        uint8_t l;
        uint8_t h;
        } bytes;
        uint16_t hl;
    } hl; //register hl 

    union {
        struct{ 
        uint8_t p;
        uint8_t s;
        } bytes;
        uint16_t sp;
    } sp; //register sp
    
    union {
        struct{ 
        uint8_t c;
        uint8_t p;
        } bytes;
        uint16_t pc;
    } pc; //register pc 

public:
    
    bool IME;
    uint8_t IE;
    uint8_t IF;

    bool bootromDone;

    uint8_t cycles; //counts up cycles then emulates speed

    cpu(bus* Bus);
    void checkInterrupts();
    void execOP();

};