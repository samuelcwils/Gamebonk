#include "ppu.h"

ppu::ppu()
{
    for(int i = 0; i < sizeof(frameBuffer); i++)
    {
        frameBuffer[i] = 0;
    }

    LDC.LY = 147;
}

void ppu::drawTile(int x, int y, int index)
{
    uint8_t tile[8][8];

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