#include "ppu.h"

ppu::ppu()
{  
    regs.bytes.LY = 0;
    OAM_access = true;
    VRAM_access = true;
}

void ppu::connectBus(bus* Bus)
{
    this->Bus = Bus;
}

void ppu::DMA(uint16_t nn)
{
    for(int i = 0; i < 0xa0; i++)
    {
        oam[i] = Bus->read(nn + i);
    }
}

uint8_t ppu::reverseBits(uint8_t n)
{
    uint8_t rev = 0;

    while (n > 0)
    {
        rev <<= 1;

        if (n & 1 == 1)
            rev ^= 1;

        n >>= 1;

    }

    return rev;
}

void ppu::fetchSprite(int id, bool xFlip, bool yFlip, bool palette, int yPos, int shift)
{
    int spriteLine;
    int highline_offset;

    if(yFlip)
    {
       spriteLine = (yPos - 16) - regs.bytes.LY; 
       highline_offset = -1;
    } else {
        spriteLine = regs.bytes.LY - (yPos - 16);
        highline_offset = 1;
    }

    fetcher.sprite_lowLine = Bus->read(0x8000 + (id * 16) + (spriteLine * 2));
    fetcher.sprite_highLine = Bus->read(0x8000 + (id * 16) + (spriteLine * 2) + highline_offset);
    
    if(xFlip){
        fetcher.sprite_lowLine = reverseBits(fetcher.sprite_lowLine);
        fetcher.sprite_highLine = reverseBits(fetcher.sprite_lowLine);
    }

    if(shift > 0)
    {
        fetcher.sprite_lowLine = fetcher.sprite_lowLine << shift;
        fetcher.sprite_highLine = fetcher.sprite_highLine << shift;
    } else {
        shift = abs(shift);
        fetcher.sprite_lowLine = fetcher.sprite_lowLine >> shift;
        fetcher.sprite_highLine = fetcher.sprite_highLine >> shift;
    }

    for(int i = 0; i < 8; i++)
    {
        fetcher.sprite_fullLine[7 - i] = (( (fetcher.sprite_highLine & ((1 << i))) << 1) + (fetcher.sprite_lowLine & (1 << i))) >> i;
    }
    
}

void ppu::fetch()
{
    switch(fetcher.state)
    {
        case getTile:
            fetcher.tileID = Bus->read(fetcher.tileRowAddr + fetcher.tileCollumn);
            
            if(regs.bytes.LCDC & 0b00010000)
            {
                fetcher.dataBase = 0x8000;    
            } else {
                fetcher.dataBase = 0x9000;
                fetcher.tileID = (int8_t)fetcher.tileID;
            }

            fetcher.state = line0;
            break;
        
        case line0:
            fetcher.lowLine = Bus->read(fetcher.dataBase + (fetcher.tileID * 16) + (fetcher.tileLine * 2));
            fetcher.state = line1;
            break;
        
        case line1:
        {
            fetcher.highLine = Bus->read(fetcher.dataBase + (fetcher.tileID * 16) + (fetcher.tileLine * 2) + 1);
            
            for(int i = 0; i < 8; i++)
            {
                fetcher.fullLine[7 - i] = (( (fetcher.highLine & ((1 << i))) << 1) + (fetcher.lowLine & (1 << i))) >> i;
            }
            
            fetcherPush();
            
            break;
        }
        case idle:
        {
            fetcherPush();

            break;
        }
    }

}

void ppu::fetcherDrawSprite()
{
    if(regs.bytes.LCDC & 0b00000010) //if sprite enable
    {
        bool spriteSize = regs.bytes.LCDC & 0b00000100;
        
        for(int i = 156; i >= 0; i-=4) //scan through to see if sprites are visible
        {
            bool yVisibility;

            if(spriteSize)
            {
                yVisibility = ((int)oam[i] - regs.bytes.LY - 16) < 17 && ((int)oam[i] - regs.bytes.LY - 16) > 0;
            } else {
                yVisibility = ((int)oam[i] - regs.bytes.LY - 8) < 9 && ((int)oam[i] - regs.bytes.LY - 8) > 0;
            }

            if(yVisibility)
            {
                int shift = (xPos + 16 - ((int)oam[i+1])); // or xVisibility. Finds how much a line in a sprite will be shifted (left) in order to fit in it's proper location. 
                
                if((shift < 8) && (shift > - 8))
                {
                    fetchSprite(oam[i + 2], oam[i + 3] & 0b00100000, oam[i + 3] & 0b01000000, oam[i + 3] & 0b10000000, oam[i], shift);
                    for(int i = 0; i < 8; i++)
                    {
                        if(fetcher.sprite_fullLine[i] == 0)
                        {
                            //do not write pixel if transparent
                        } else {
                            fetcher.fullLine[i] = fetcher.sprite_fullLine[i];
                        }
                        
                    }
                }


                
            }
        }
    }
            
}

void ppu::fetcherPush()
{
    int fifoSize = FIFO.size();
            if(fifoSize == 8 || fifoSize == 0){ 

                fetcherDrawSprite();

                for(int i = 0; i < 8; i++){
                    FIFO.push(fetcher.fullLine[i]);
                }
                fetcher.state = getTile;
                fetcher.tileCollumn++;
            } else {
                fetcher.state = idle;
            }
}

int ppu::getColorID(int colorIndex) //returns a color ID for a given pixel
{    
    return (((regs.bytes.BGP & (0b00000011 << (colorIndex * 2))) >> (colorIndex * 2)));
}

uint16_t ppu::getPixel()
{
    int fifoSize = FIFO.size();
    int fifoFront = FIFO.front();
    int colorID = getColorID(FIFO.front());
 
    switch(colorID)
    {
        case 0:
            return 0x6c93; //white
            break;

        case 1:
            return 0x432e; //light gray
            break;
        
        case 2:
            return 0x2a0a; //dark gray
            break;
        
        case 3:
            return 0x1105; //black
            break;

        default:
            return 0;
            break;
    }

}

void ppu::tick()
{

    statusMode = regs.bytes.STAT & 0b00000011;

    if(regs.bytes.LY == regs.bytes.LYC)
    {
        regs.bytes.STAT |= 0b00000100;
        Bus->interruptFlags(0b00000010);
        
    } else 
    {
        regs.bytes.STAT &= 0b11111011;
    }

    switch(statusMode)
    {
        case OAM:
            OAM_access = false;
            if(ticks == 80){
                if(regs.bytes.LCDC & 0b00001000)
                {
                    fetcher.BG_mapBase = 0x9c00;
                } else {
                    fetcher.BG_mapBase = 0x9800;
                }

                fetcher.tileLine = (regs.bytes.LY + regs.bytes.SCY) % 8;
                fetcher.tileRowAddr = fetcher.BG_mapBase + ( ( (  ( (regs.bytes.LY + regs.bytes.SCY) % 256 )/8) ) * 32);
                
                for(int i = FIFO.size(); i > 0; i--) //clear FIFO from last line
                {
                    FIFO.pop();
                }

                fetcher.state = 0;
                fetcher.tileCollumn = 0;
                OAM_access = true;
                regs.bytes.STAT &= 0b11111100;
                regs.bytes.STAT |= Transfer;
            }
            break;
        
        case Transfer:
            if(ticks % 2)
            {
                fetch();
            }
            

            VRAM_access = false;
            OAM_access = false;

            if((FIFO.size() > 8)){
                frameBuffer[(regs.bytes.LY * 160) + xPos] = getPixel();
                FIFO.pop();
                xPos++;

                if(regs.bytes.WY == regs.bytes.LY)
                {
                    if(xPos == regs.bytes.WX)
                    {
                        for(int i = 0; i < FIFO.size(); i++)
                        {
                            FIFO.pop();
                        }
                        fetcher.tileRowAddr = fetcher.WIN_mapBase + (((regs.bytes.LY) ) / 8) * 32;
                        fetcher.state = getTile;
                    }
                }
            }

            if(xPos == 160){
                xPos = 0;
                ticks+=2;
                regs.bytes.STAT &= 0b11111100;
                regs.bytes.STAT |= hBlank;
                regs.bytes.STAT |= 0b00001000; //interrupt source
                Bus->interruptFlags(0b00000010);
                VRAM_access = true;
                OAM_access = true;
            }
            break;
       
        case hBlank:
            VRAM_access = true;
            OAM_access = true;
            if(ticks == 456)
            {
                ticks = 0; 
                regs.bytes.LY++;

                if(regs.bytes.LY == 144)
                {
                    Bus->interruptFlags(0b00000001); //sends vblank interrupt
                    
                    regs.bytes.STAT &= 0b11111100;
                    regs.bytes.STAT |= vBlank;
                    regs.bytes.STAT |= 0b00010000; //interrupt source
                    Bus->interruptFlags(0b00000010);
                
                } else {
                        regs.bytes.STAT &= 0b11111100;
                        regs.bytes.STAT |= OAM;
                        regs.bytes.STAT |= 0b00100000; //interrupt source
                        Bus->interruptFlags(0b00000010);
                }

            }
            break;
        
        case vBlank:
            VRAM_access = true;
            OAM_access = true;
            
            if(ticks == 456)
            {
                ticks = 0; 
                regs.bytes.LY++;
                
                if(regs.bytes.LY == 153)
                {
                    regs.bytes.LY = 0; 
                    
                    regs.bytes.STAT &= 0b11111100;
                    regs.bytes.STAT |= OAM;
                    regs.bytes.STAT |= 0b00100000; //interrupt source
                    Bus->interruptFlags(0b00000010);
                    
                    frameDone = true;
                }
            }

            break;
    }

    ticks++; 

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