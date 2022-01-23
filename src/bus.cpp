#include "bus.h"

bus::bus(cart* Cart, ppu* PPU)
{
    memoryMap.Cart = Cart;
    memoryMap.PPU = PPU;
}

void bus::connectCPU(cpu* CPU)
{
    this->memoryMap.CPU = CPU;
}

void bus::write(uint16_t address, uint8_t byte)
{
    if(address <= 0x7fff)
    {
        memoryMap.Cart->romBank[address] = byte;

    } else if(address <= 0x9fff){

        memoryMap.PPU->vRam.vRam[address - 0x8000] = byte;

    } else if(address <= 0xbfff){

        memoryMap.Cart->cartRam[address - 0xa000] = byte;

    } else if(address <= 0xdfff){

        wRam[address-0xc000] = byte;
    
    } else if(address <= 0xedff){

        wRam[address-0xe000] = byte;//echo of work ram

    } else if(address <= 0xfe9f){

        memoryMap.PPU->oam[address - 0xee00] = byte;
    
    } else if(address <= 0xff4b){

        memoryMap.PPU->regs.regs[(address - 0xff39)]; //TODO need IO

    } else if(address <= 0xff7e){

        0; //TODO need IO

    } else if(address <= 0xfffe){

        hRam[address - 0xff7f] = byte;

    } else if(address == 0xffff){
        
        memoryMap.CPU->IE = byte;
    
    }

}

uint8_t bus::read(uint16_t address)
{
   
   if(address <= 0x7fff)
    {
        return memoryMap.Cart->romBank[address];

    } else if(address <= 0x9fff){

        return memoryMap.PPU->vRam.vRam[address - 0x8000];

    } else if(address <= 0xbfff){

        return memoryMap.Cart->cartRam[address - 0xa000];

    } else if(address <= 0xdfff){

        return wRam[address-0xc000];
    
    } else if(address <= 0xedff){

        return wRam[address-0xe000];//echo of work ram

    } else if(address <= 0xfe9f){

        return memoryMap.PPU->oam[address - 0xee00];
    
    } else if(address <= 0xff4b){

        return memoryMap.PPU->regs.regs[(address - 0xff39)]; //TODO need IO

    } else if(address <= 0xff7e){

        return 0; //TODO need IO

    } else if(address <= 0xfffe){

        return hRam[address - 0xff7f];

    } else if(address == 0xffff){
        
        return memoryMap.CPU->IE;
    
    }

    return 0;

}