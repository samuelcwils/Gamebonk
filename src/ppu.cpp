#include "ppu.h"

void ppu::drawTile()
{

}

void ppu::drawTiles()
{
    for(int i = 0; i < 0x2000; i+=2)
    {
        uint8_t low = vRam[i];
        uint8_t high = vRam[i+1];
        
        for(int i = 0; i < 8; i++)
        {
            
        }

    }
    
}