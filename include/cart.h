#pragma once
#include "stdint.h"
#include "stdio.h"

class cart{

    uint8_t* rom;
    uint8_t* bootRom;

public:
    cart(uint8_t* rom, uint8_t* bootRom, uint32_t romSize);


    uint32_t romSize;
    char title[16];
    uint8_t cartType;
    uint16_t cartRamSize;
    uint8_t cartRam[0x2000]; //Accessed as one bank TODO SWITCHABLE BANK

    union{
        struct{
            uint8_t staticBank[0x4000];
            uint8_t variableBank[0x4000];
        };
        uint8_t romBank[0x8000]; //allows both banks to be accesses as one
    };

    void printCart();

    void bootRomLoad();

    void bank0();

    void bank1();
    
};