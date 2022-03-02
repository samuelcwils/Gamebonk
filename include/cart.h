#pragma once
#include "stdint.h"
#include "stdio.h"

class cart{


public:
    cart(uint8_t* rom, uint8_t* bootRom, uint32_t romSize);

    uint32_t romSize;
    char title[16];
    uint8_t cartType;
    uint32_t ramSize;
 
    uint8_t* bootRom;
    uint8_t* rom;
    uint8_t* cartRam; // holds cart ram. different amounts depending on cart
    uint8_t* ramBank; //increment and decrement by 0x2000 to switch banks for read. Points to start of RAM
    int bankBits; //amount of bits in the number of total banks. Used for masking rom bank register

    int romBankNum;
    int ramBankNum;
    bool ramBanking;

    void printCart();

    void writeRom(uint16_t address, uint8_t value);
    void writeRam(uint16_t address, uint8_t value);

private:

    void MBC1writeRom(uint16_t address, uint8_t value);
    void MBC1writeRam(uint16_t address, uint8_t value);
    
};