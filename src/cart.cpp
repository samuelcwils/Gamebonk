#include "cart.h"

cart::cart(uint8_t* rom, uint32_t romSize)
{
    this->rom = rom;
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
   
    switch(cartType) //no mappers yet
    {
        case 0x01:
            noMapperLoad();
    }

}

void cart::printCart()
{
    printf("The rom size is: %d\n", romSize);
    printf("The rom's title is: %s\n", title);
    printf("The cart type is: %i\n", cartType);
    printf("The cart's ram size is: %d\n", cartRamSize);
}

void cart::noMapperLoad()
{
    for(int i = 0; i < 0x4000; i++){
        staticBank[i] = rom[i];
    }

    for(int i = 0x4000; i < 0x8000; i++){
        variableBank[i] = rom[i];
    }

}