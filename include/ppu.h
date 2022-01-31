#pragma once
#include "stdint.h"
#include "IO.h"
#include "bus.h"
#include "cpu.h"
#include <queue>

class cpu;
class bus;
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

    uint8_t frameBuffer[160*144] = { };

    union{
        struct {
            uint8_t LCDC; //FF40
            uint8_t STAT; //FF41
            uint8_t SCY;  //FF42
            uint8_t SCX;  //FF43
            uint8_t LY;   //FF44
            uint8_t LYC;  //FF45
            uint8_t dma;
            uint8_t BGP;  //FF47
            uint8_t OBP0; //FF48
            uint8_t OBP1; //FF49
            uint8_t WY;   //FF4A
            uint8_t WX;   //FF4B   
        } bytes;
        uint8_t regs[12];

    } regs;

    ppu();

    void connectCPU(cpu* CPU);

    void connectBus(bus* Bus);

    void fetch();
    
    int getColorID(int colorIndex);

    uint16_t getPixel();

    void tick();

    void drawTile(int x, int y, int index); //debug thing

    void drawTiles(); //debug thing

private:
    
    int xPos;

    enum {
	hBlank   = 0,
	vBlank   = 1,
	OAM      = 2,
	Transfer = 3,
    };

    enum {
    getTile  = 0,
    line0    = 1,
    line1    = 2,
    idle     = 3,
    };

    std::queue<uint8_t> FIFO; //holds queued pixels. each pixel is an index for a color in the palette
   
    struct{
        uint8_t lowLine;
        uint8_t highLine;
        uint8_t fullLine[8];

        int state;
        int tileID;
        int tileLine;
        uint16_t tileRowAddr;
        uint8_t tileCollumn;
    } fetcher;

    uint8_t statusMode;
    int ticks;
    cpu* CPU;
    bus* Bus;

};