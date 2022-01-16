#include "bus.h"

bus::bus(cart* Cart, ppu* PPU)
{
    memoryMap.Cart = Cart;
    memoryMap.PPU = PPU;
}

void bus::write(uint16_t address, uint8_t byte)
{

}

uint8_t bus::read(uint16_t address)
{
   if(address <= 0x7fff)
    {
        return memoryMap.Cart->romBank[address];

    } else if(address <= 0x9fff){

        return memoryMap.PPU->vRam[address - 0x8000];

    } else if(address <= 0xbfff){

        return memoryMap.Cart->cartRam[address - 0xa000];

    } else if(address <= 0xdfff){

        return wRam[address-0xc000];
    
    } else if(address <= 0xedff){

        return wRam[address-0xe000];//echo of work ram

    } else if(address <= 0xfe9f){

        return memoryMap.PPU->oam[address - 0xee00];

    } else if(address <= 0xff4b){

        return 0; //TODO need IO

    } else if(address <= 0xfffe){

        return hRam[address - 0xff7f];

    } else if(address == 0xffff){
        return CPU->
    }


}