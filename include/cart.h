#pragma once
#include "stdint.h"
#include "stdio.h"

class cart{

    uint8_t* rom;

public:
    cart(uint8_t* rom, uint32_t romSize);


    uint32_t romSize;
    char title[16];
    uint8_t cartType;
    uint16_t cartRamSize;
    uint8_t cartRam[0x2000]; //switchable bank, if any

    union{
        struct{
            uint8_t staticBank[0x4000];
            uint8_t variableBank[0x4000];
        };
        uint8_t romBank[0x8000]; //allows both banks to be accesses as one
    };

    void printCart();

private:
    void noMapperLoad();
    
};