#include "ppu.h"

ppu::ppu(bus* Bus)
{
    for(int i = 0; i < sizeof(frameBuffer); i++)
    {
        frameBuffer[i] = 0;
    }

    statusMode = OAM;
    regs.bytes.LY = 0;
    xPos = 0;
    ticks = 0;
}

void ppu::connectCPU(cpu* CPU)
{
    this->CPU = CPU;
}

void ppu::fetch()
{
    switch(fetcher.state)
    {
        case getTile:
            fetcher.tileID = Bus->read(fetcher.tileRowAddr + fetcher.tileCollumn);
            fetcher.state = line0;
            break;
        
        case line0:
            fetcher.lowLine = Bus->read(0x8000 + (fetcher.tileID * 16) + (fetcher.tileLine * 2));
            fetcher.state = line1;
            break;
        
        case line1:
            fetcher.highLine = Bus->read(0x8000 + (fetcher.tileID * 16) + (fetcher.tileLine * 2) + 1);
            
            for(int i = 0; i < 8; i++)
            {
                fetcher.fullLine[i] = (fetcher.highLine & (0b00000001 << i) << 1) + (fetcher.lowLine & (0b00000001 << i));
            }
            
            if(FIFO.size() == 8){ 
                for(int i = 0; i < 8; i++){
                    fetcher.tileCollumn++;
                    FIFO.push(fetcher.fullLine[i]);
                    fetcher.state = getTile;
                }
            } else {
                fetcher.state = idle;
            }
            break;
        
        case idle:
            if(FIFO.size() == 8){ 
                fetcher.tileCollumn++;
                for(int i = 0; i < 8; i++){
                    FIFO.push(fetcher.fullLine[i]);
                    fetcher.state = getTile;
                }
            }
            break;
    }

}

void ppu::tick()
{
    ticks++;

    statusMode = regs.bytes.STAT & 0b00000011;

    switch(statusMode)
    {
        case OAM:
            if(ticks == 40){
                fetcher.tileLine = regs.bytes.LY + regs.bytes.SCY % 8;
                fetcher.tileRowAddr = 0x9800 + ((regs.bytes.LY + regs.bytes.SCY / 8) * 32);
                fetcher.tileCollumn = 0;
                regs.bytes.STAT |= Transfer;
            }
            break;
        
        case Transfer:
            fetch();

            if(!(ticks % 2)){
                frameBuffer[(regs.bytes.LY * 159) + xPos] = FIFO.front();
                FIFO.pop();
                xPos++;
            }

            if(xPos == 160){
                regs.bytes.STAT |= hBlank;
            }
            break;
        
        case hBlank:
            if(ticks == 456)
            {
                ticks = 0; 
                regs.bytes.LY++;
                regs.bytes.STAT |= OAM;
                
            } else if(regs.bytes.LY == 144)
            {
                regs.bytes.STAT |= vBlank;
            }
            break;
        
        case vBlank:
            if(ticks == 456){ticks = 0; regs.bytes.LY++;}
            if(regs.bytes.LY == 153){
                regs.bytes.LY = 0; 
                regs.bytes.STAT |= OAM;
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