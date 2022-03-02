#include "cart.h"

cart::cart(uint8_t* rom, uint8_t* bootRom, uint32_t romSize)
{
    this->rom = rom;
    this->bootRom = bootRom;
    this->romSize = romSize;

    int ramSizeLookup[6] = {0, 0, 0x2000, 0x8000, 0x20000, 0x10000};
    
    for(int i = 0; i < 16; i++) //sets title
    {
        title[i] = rom[i+0x134];    
    }

    cartType = rom[0x147]; //finds which hardware is used in cartidge

    ramSize = ramSizeLookup[rom[0x149]];
    cartRam = new uint8_t[ramSize];
    ramBank = cartRam; //sets to bank 0 
    romBankNum = 1;

    ramBanking = false;


    switch(romSize / 0x4000) //finds bank bits
    {
        case 2:
            bankBits = 0b0000000000000011;
            break;
        case 4:
            bankBits = 0b0000000000000111;
            break;
        case 8:
            bankBits = 0b0000000000001111;
            break;
        case 16:
            bankBits = 0b0000000000011111;
            break;
        case 32:
            bankBits = 0b0000000000111111;
            break;
        case 64:
            bankBits = 0b0000000001111111;
            break;
        case 128:
            bankBits = 0b0000000011111111;
            break;
        case 256:
            bankBits = 0b0000000111111111;
            break;
        case 512:
            bankBits = 0b0000001111111111;
            break;
    }

}

void cart::printCart()
{
    printf("The rom size is: %d\n", romSize);
    printf("The rom's title is: %s\n", title);
    printf("The cart type is: %i\n", cartType);
    printf("The cart's ram size is: %d\n", ramSize);
}


void cart::writeRom(uint16_t address, uint8_t value)
{
    switch(cartType)
    {
        case 01:
            MBC1writeRom(address, value);
            break;
        case 02:
            MBC1writeRom(address, value);
            break;
        case 03:
            MBC1writeRom(address, value);
            break;

    }

}

void cart::writeRam(uint16_t address, uint8_t value)
{
    switch(cartType)
    {
        case 01:
            MBC1writeRam(address, value);
            break;
        case 02:
            MBC1writeRam(address, value);
            break;
        case 03:
            MBC1writeRam(address, value);
            break;

    } 
}


