#pragma once
#include "stdint.h"
#include "IO.h"
#include "bus.h"
#include "cpu.h"

class cpu;
class Bus;
class IO;

class ppu {
public:
    
    union{
        struct{
            uint8_t tileData[0x17ff];
            struct{
                uint8_t map1[0x3ff];
                uint8_t map2[0x3ff];
            } maps;
        } blocks;
        uint8_t vRam[0x2000];
    } vRam;

    uint8_t oam[160];

    uint8_t frameBuffer[256*256] = { };

    union{
        struct {
            uint8_t WX;   //FF4B
            uint8_t WY;   //FF4A
            uint8_t OBP1; //FF49
            uint8_t OBP0; //FF48
            uint8_t BGP;  //FF47
            uint8_t dma;
            uint8_t LYC;  //FF45
            uint8_t LY;   //FF44
            uint8_t SCX;  //FF43
            uint8_t SCY;  //FF42
            uint8_t STAT; //FF41
            uint8_t LCDC; //FF40
        } bytes;
        uint8_t regs[12];

    } regs;

    ppu();

    void connectCPU(cpu* CPU);

    void tick();

    void drawTile(int x, int y, int index);

    void drawTiles(); //debug thing

private:
    
    enum {
	hBlank   = 0,
	vBlank   = 1,
	OAM      = 2,
	Transfer = 3,
    } statusMode;

    int ticks;
    cpu* CPU;

};