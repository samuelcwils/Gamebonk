#pragma once
#include "stdint.h"
#include "IO.h"
#include "bus.h"

class Bus;
class IO;

class ppu {
public:
    uint8_t vRam[0x2000];
    uint8_t oam[160];

    uint8_t frameBuffer[256*256];

    struct 
    {
        uint8_t LCDC;
        uint8_t STAT;
        uint8_t SCY;
        uint8_t LY;
        uint8_t LYC;
        uint8_t WY;
        uint8_t BGP;
    } LDC;

    void drawTile();

    void drawTiles(); //debug thing

};