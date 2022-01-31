#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "incbin.h"
#include "IO.h"
#include <chrono>
#include <thread>

extern "C" INCBIN(Rom, "gameboy.gb"); 
extern "C" INCBIN(Boot, "dmg_boot.bin"); 

using namespace std::chrono_literals;
using namespace std::chrono;

bool frameDone = false;
unsigned long totalCycles = 1;

void renderSDL(IO* io, ppu* PPU)
{
    while(true)
    {
        io->keyInput();
        if(frameDone){
            PPU->drawTiles();
            io->updateDisplay((PPU->frameBuffer), 256);
            frameDone = false;
        }
        
    } 
}

int main()
{      
    for(int i = 0; i < 100000; i++)
    {
        printf("INITIALIZING\n");
    }
    
    cart* cartridge = new cart( (uint8_t*)gRomData, (uint8_t*) gBootData, gRomSize);
    ppu* PPU = new ppu();
    bus* Bus = new bus(cartridge, PPU);
    PPU->connectBus(Bus);
    cpu* CPU = new cpu(Bus);
    IO* io = new IO(CPU->IF);
    Bus->connectCPU(CPU);
    PPU->connectCPU(CPU);

    io->createWindow(1024, 512, 256, 256);
    std::thread render(renderSDL, io, PPU);

    cartridge->printCart();

    ////////////////////////////////////////////////////////////////
    //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM 

    cartridge->romLoad(); //(load rom then overlay bootrom)
    cartridge->bootRomLoad();
    
    while(CPU->bootromDone == false)
    {
        if(!(PPU->regs.bytes.LCDC & 0b10000000))
        {

            CPU->checkInterrupts();
            CPU->execOP();
            
            CPU->cycles /= 4; //get cpu cycles from machine cycles 
            totalCycles += CPU->cycles;

            while(CPU->cycles > 0)
            {
                if(PPU->regs.bytes.LCDC & 0b10000000)
                {
                    PPU->tick();
                    PPU->tick();
                }
                
                //update timers() //TODO
                
                CPU->cycles--;
            }
       
        } else {
            
            auto start = high_resolution_clock::now(); 
            
            while(totalCycles < 70224)
            {

                
                CPU->checkInterrupts();
                CPU->execOP();
                
                CPU->cycles /= 4; //get cpu cycles from machine cycles 
                totalCycles += CPU->cycles;

                while(CPU->cycles > 0)
                {
                    if(PPU->regs.bytes.LCDC & 0b10000000)
                    {
                        PPU->tick();
                        PPU->tick();
                    }
                    
                    //update timers() //TODO
                    
                    //ppu
                    
                    CPU->cycles--;
                }
            }

            auto stop = high_resolution_clock::now(); 
            auto waitTime = std::chrono::duration_cast<microseconds>(stop - start);

            std::cout << waitTime.count() << std::endl;

            frameDone = true;
            std::this_thread::sleep_for(16666us - waitTime);
            totalCycles = 0;
        }

    }


    while(true)
    {
        printf("done\n");

    }
    
    //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM 
    /////////////////////////////////////////////////////////////////////////////

  
  
    //////////////////////////////////////////////////////////////////////////////
    //rest of ROM //rest of ROM //rest of ROM //rest of ROM //rest of ROM
    cartridge->romLoad(); //(replace bootrom)
    unsigned long long cycleAmount = 4200000 * 50;
    while(cycleAmount > 0)
    {
        auto Start = high_resolution_clock::now(); 
        
        CPU->checkInterrupts();
        CPU->execOP();

        CPU->cycles /= 4; //get cpu cycles from machine cycles 
        while(CPU->cycles > 0)
        {
            if(PPU->regs.bytes.LCDC & 0b10000000)
            {
                PPU->tick();
                PPU->tick();
            }
            
            //update timers() //TODO
             
            //ppu
            
            CPU->cycles--;
        }
        
        cycleAmount--;
    }
    //rest of ROM //rest of ROM //rest of ROM //rest of ROM //rest of ROM
    //////////////////////////////////////////////////////////////////////////////

    return 0;   
}