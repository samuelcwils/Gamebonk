#pragma once
#include <stdint.h>
#include "bus.h"

class cpu {

private:
    bus* Bus;
    uint16_t pc; 
    uint8_t cycles; //counts up cycles then emulates speed 

    void emulateCycles();
    void Zflag(uint16_t a, uint16_t b); 
    void Hflag(uint8_t a, uint8_t b); 
    void Hflag(uint16_t a, uint16_t b); 
    void Cflag(uint8_t a, uint8_t b);
    void Cflag(uint16_t a, uint16_t b);

    void LD_d16(uint8_t &high, uint8_t &low);
    void LD_d8(uint8_t &byte);
    void LD_a_b(uint8_t &a, uint8_t b);
    void LD_HL_REG(uint8_t reg);
    void LD_ADDRESS_A(uint16_t address);
    void LD_A_ADDRESS(uint16_t address);
    
    void ADD(uint16_t &a, uint16_t b);
    void ADD(uint8_t &a, uint8_t b);

    void INC(uint16_t &value);
    void INC(uint8_t &byte);
    void DEC(uint16_t &value);
    void DEC(uint8_t &byte);


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
    } hl; //register de 

    union {
        struct{ 
        uint8_t p;
        uint8_t s;
        } bytes;
        uint16_t sp;
    } sp; //register de 


    


public:
    cpu(bus* Bus);
    void execOP();




};