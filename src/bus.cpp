#include "bus.h"

bus::bus(cart* Cart, ppu* PPU)
{
    memoryMap.Cart = Cart;
    memoryMap.PPU = PPU;
    joypad = 0xcf;
    joypad_state = 0xff;
    busTickEnable = true;
}

void bus::connectCPU(cpu* CPU)
{
    this->memoryMap.CPU = CPU;
}

void bus::write(uint16_t address, uint8_t byte)
{
    if(address <= 0x7fff)
    {
        memoryMap.Cart->writeRom(address, byte);
        
    } else if(address <= 0x9fff){

        if(memoryMap.PPU->VRAM_access)
        {
            memoryMap.PPU->vRam.vRam[address - 0x8000] = byte;
        }

    } else if(address <= 0xbfff){

        memoryMap.Cart->ramBank[address - 0xa000] = byte;

    } else if(address <= 0xdfff){

        wRam[address-0xc000] = byte;
    
    } else if(address <= 0xedff){

        wRam[address-0xe000] = byte; //echo of work ram

    } else if(address <= 0xfe9f){

        if(memoryMap.PPU->OAM_access)
        {
            memoryMap.PPU->oam[address - 0xee00] = byte;
        }

    } else if(address == 0xff00){

        joypad = (byte |= (joypad & 0xcf));

    } else if(address == 0xff01){

        serial[0] = byte;

    } else if(address == 0xff02){

        if(byte & 0b10000000)
        {
            interruptFlags(0b10000000);
        }

        serial[1] = byte;

       // printf("%c", serial[0]) ;


    } else if(address == 0xff04){

        memoryMap.CPU->DIV = 0;
        memoryMap.CPU->totalTicks_DIV = 0;
        memoryMap.CPU->totalTicks_TIMA = 0;
        memoryMap.CPU->TIMA = 0;
    
    } else if(address == 0xff05){

        memoryMap.CPU->TIMA = byte;

    } else if(address == 0xff06){

        memoryMap.CPU->TMA = byte;

    } else if(address == 0xff07){

        memoryMap.CPU->TAC = byte;
        int lookup[] = {1024,16, 64, 256};
        memoryMap.CPU->TIMA_speed = lookup[(memoryMap.CPU->TAC & 0b00000011)];

    } else if(address == 0xff0f){

        memoryMap.CPU->IF = byte;

    
    } else if(address >= 0xff40 && address <= 0xff4b){

        if(address == 0xff46)
        {
            memoryMap.PPU->DMA(((uint16_t)byte) << 8);
        }

        memoryMap.PPU->regs.regs[(address - 0xff40)] = byte;

    } else if(address <= 0xfffe){

        hRam[address - 0xff81] = byte;

    } else if(address == 0xffff){
        
        memoryMap.CPU->IE = byte;
    
    }

}

uint8_t bus::read(uint16_t address)
{   
    if(address <= 0x00ff)
    {
        if(memoryMap.CPU->bootRomDone)
        {
            return memoryMap.Cart->rom[address];
        } else {
            
            return memoryMap.Cart->bootRom[address];
        }
    
    } else if(address <= 0x3fff)
    {

        return memoryMap.Cart->rom[address];

    } else if(address <= 0x7fff)
    {
        return memoryMap.Cart->rom[address + ((memoryMap.Cart->romBankNum - 1) * 0x4000)];

    } else if(address <= 0x9fff){

        if(memoryMap.PPU->VRAM_access || (PPU_read))
        {
            return memoryMap.PPU->vRam.vRam[address - 0x8000];
        } else {
            return 0xff;
        }

    } else if(address <= 0xbfff){

        return memoryMap.Cart->cartRam[address - 0xa000];

    } else if(address <= 0xdfff){

        return wRam[address-0xc000];
    
    } else if(address <= 0xedff){

        return wRam[address-0xe000];//echo of work ram

    } else if(address <= 0xfe9f){

        if(memoryMap.PPU->OAM_access || (PPU_read))
        {
            return memoryMap.PPU->oam[address - 0xee00];
        } else {
            return 0xff;
        }
        
    } else if(address == 0xff00){

        joypad &= 0xf0;

        if(!(joypad & 0b00010000))
        {
            return joypad | (joypad_state & 0x0f);
        }

        if(!(joypad & 0b00100000))
        {
            return joypad | ((joypad_state & 0xf0) >> 4);
        }

    } else if(address == 0xff01){

        return serial[0];

    } else if(address == 0xff02){

        return serial[1];

    } else if(address == 0xff04){

        return memoryMap.CPU->DIV;
    
    } else if(address == 0xff05){

        return memoryMap.CPU->TIMA;

    } else if(address == 0xff06){

        return memoryMap.CPU->TMA;

    } else if(address == 0xff07){

        return memoryMap.CPU->TAC;

    } else if(address == 0xff0f){

        return memoryMap.CPU->IF;
      
    } else if(address >= 0xff40 && address <= 0xff4b){

        if(address == 0xff41)
        {
            if(!(memoryMap.PPU->regs.bytes.LCDC & 0b10000000))
            {
                return (memoryMap.PPU->regs.regs[(address - 0xff41)]) & 0b11111100;
            } else {
                return memoryMap.PPU->regs.bytes.STAT;
            }

           int x = 5 + 5;
        }

        return memoryMap.PPU->regs.regs[(address - 0xff40)];

    } else if(address <= 0xfffe){

        return hRam[address - 0xff81];

    } else if(address == 0xffff){
        
        return memoryMap.CPU->IE;

    }

    return 0xff;
}

void bus::interruptFlags(uint8_t flag)
{
    if(!((memoryMap.CPU->servicingInterrupt) && flag == 2))
    {
        memoryMap.CPU->IF |= flag;
    }
    
}