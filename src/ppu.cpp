#include "ppu.h"

ppu::ppu()
{
    for(int i = 0; i < sizeof(frameBuffer); i++)
    {
        frameBuffer[i] = 0;
    }

    statusMode = OAM;
    regs.bytes.LY = 0;
    ticks = 0;
}

void ppu::connectCPU(cpu* CPU)
{
    this->CPU = CPU;
}

void ppu::tick()
{
    ticks++;

    switch(statusMode)
    {
        case OAM:
            if(ticks == 40){
                statusMode = Transfer;
            }
            break;
        
        case Transfer:
            if(ticks == 160){
                statusMode = hBlank;
            }
            break;
        
        case hBlank:
            if(ticks == 456)
            {
                ticks = 0; 
                regs.bytes.LY++;
                statusMode = OAM;
           
            } else if(regs.bytes.LY == 144)
            {
                statusMode = vBlank;
            }
            break;
        
        case vBlank:
            if(ticks == 456){ticks = 0; regs.bytes.LY++;}
            if(regs.bytes.LY == 153){
                regs.bytes.LY = 0; 
                statusMode = OAM;
                CPU->IF |= 0b00000001;
            }
            break;
    }

}

void ppu::drawTile(int x, int y, int index)
{
    uint8_t tile[8][8] = {0};

    for(int tileLine = 0; tileLine < 16; tileLine+=2)
    {
         
        for(int i = 0; i < 8; i++)
        {
            uint8_t lowLine[8];
            uint8_t highLine[8];
            
            uint8_t low = vRam.vRam[index + tileLine];
            uint8_t high = vRam.vRam[index + tileLine + 1];

            uint8_t line[8];
            
            for(int i = 0; i < 8; i++)
            {
                lowLine[i] = low & (0b00000001 << i);
            }

            for(int i = 0; i < 8; i++)
            {
                highLine[i] = high & (0b00000001 << i);
            }

            for(int i = 0; i < 8; i++)
            {
                line[i] = lowLine[i] + highLine[i];
            }
            
            for(int pixel; pixel < 8; pixel++)
            {
                (tile[tileLine][pixel]) = line[pixel];
            }

        }

    }

    for(int tileLine = 0; tileLine < 8; tileLine++)
    {
        for(int tilePixel = 0; tilePixel < 8; tilePixel++)
        {
            frameBuffer[(tileLine + y * 256) + tilePixel + x] = tile[tileLine][tilePixel];
        }
        
    }

    
}

void ppu::drawTiles()
{
    for(int index = 0; index < 0x17ff; index += 16)
    {
        int tileNum = index / 16;
        int x = tileNum * 8;
        int y = x / 32; 
        drawTile(x, y, index);
    }
    
}