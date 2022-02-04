#include "cart.h"

cart::cart(uint8_t* rom, uint8_t* bootRom, uint32_t romSize)
{
    this->rom = rom;
    this->bootRom = bootRom;
    this->romSize = romSize;
    
    for(int i = 0; i < 16; i++) //sets title
    {
        title[i] = rom[i+0x134];    
    }

    cartType = rom[0x147]; //finds which hardware is used in cartidge

    if(rom[0x149]) { //finds size of cartridge ram, but only if there is ram
        cartRamSize =  1 << (rom[0x149]); 
    } else {
        cartRamSize = 0;
    }

    bank0();
    bank1();
    bootRomLoad();

}

void cart::printCart()
{
    printf("The rom size is: %d\n", romSize);
    printf("The rom's title is: %s\n", title);
    printf("The cart type is: %i\n", cartType);
    printf("The cart's ram size is: %d\n", cartRamSize);
}

void cart::bank0()
{
    for(int i = 0; i < 0x4000; i++){
        staticBank[i] = rom[i];
    }
}

void cart::bank1()
{
        for(int i = 0; i < 0x4000; i++){
        variableBank[i] = rom[i + 0x4000];
    }
}

void cart::bootRomLoad()
{
    for(int i = 0; i < 256; i++)
    {
        staticBank[i] = bootRom[i];
    }
}

