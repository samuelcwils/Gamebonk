#pragma once
#include "stdint.h"
#include "IO.h"
#include "bus.h"
#include "cpu.h"
#include <queue>
#include <cstdlib>
#include <cmath>

class cpu;
class bus;
class IO;

class ppu {
public:
    
    union{
        struct{
            uint8_t tileData[0x1800];
            struct{
                uint8_t map1[0x400];
                uint8_t map2[0x400];
            } maps;
        } blocks;
        uint8_t vRam[0x2000];
    } vRam;

    uint8_t oam[160];

    uint16_t frameBuffer[160*144];
    bool frameDone;
    bool OAM_access;
    bool VRAM_access;
    bool disableFlag;

    union{
        struct {
            uint8_t LCDC; //FF40
            uint8_t STAT; //FF41
            uint8_t SCY;  //FF42
            uint8_t SCX;  //FF43
            uint8_t LY;   //FF44
            uint8_t LYC;  //FF45
            uint8_t DMA;  //FF46
            uint8_t BGP;  //FF47
            uint8_t OBP0; //FF48
            uint8_t OBP1; //FF49
            uint8_t WY;   //FF4A
            uint8_t WX;   //FF4B   
        } bytes;
        uint8_t regs[12];

    } regs;

    ppu();

    void connectBus(bus* Bus);

    void DMA(uint16_t nn);

    void tick();

    void drawTile(int x, int y, int index); //debug thing

    void drawTiles(); //debug thing

private:
    
    uint8_t reverseBits(uint8_t n);

    void fetchSprite(int id, bool xFlip, bool yFlip, bool palette, int yPos, int shift); // palette: 0=OBP0, 1=OBP1

    void fetch();

    void fetcherPush();

    void fetcherDrawSprite();

    int getColorID(int colorIndex);

    uint16_t getPixel();
    
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
        uint16_t fullLine[8];
        uint8_t sprite_lowLine;
        uint8_t sprite_highLine;
        uint16_t sprite_fullLine[8];

        int state;
        int tileID;
        int tileLine;
        uint16_t tileRowAddr;
        uint16_t BG_mapBase;
        uint16_t WIN_mapBase;
        uint16_t dataBase; //not for sprites
        uint8_t tileCollumn;
    } fetcher;

    
    uint8_t statusMode;
    int ticks;
    bus* Bus;

};