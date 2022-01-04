#pragma once
#include "array"
#include "cart.h"
#include "ppu.h"
#include "stdint.h"

class bus {
private:
    uint8_t wRam[0x2000];
    uint8_t hRam[126];

    struct 
    {
        cart* Cart;
        ppu* PPU;
    } memoryMap;
    

public:
    bus(cart* Cart, ppu* PPU);
    void write(uint16_t address, uint8_t byte);
    uint8_t read(uint16_t address);
};

