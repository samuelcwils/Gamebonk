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


void pollSDL(IO* io)
{
    while(true)
    {
        io->keyInput();
        std::this_thread::sleep_for(500us); //sleep for clock speed minus delay
    }
}

void renderSDL(IO* io, ppu* PPU)
{
    while(true)
    {
        io->updateDisplay((PPU->frameBuffer), 256);
        std::this_thread::sleep_for(1667us); //sleep for clock speed minus delay
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
    cpu* CPU = new cpu(Bus);
    IO* io = new IO(CPU->IF);
    Bus->connectCPU(CPU);
    PPU->connectCPU(CPU);

    io->createWindow(1024, 512, 256, 256);
    std::thread poll(pollSDL, io);
    std::thread render(renderSDL, io, PPU);

    cartridge->printCart();

    //boot ROM 
    cartridge->romLoad(); //(load rom then overlay bootrom)
    cartridge->bootRomLoad();
    unsigned int cycleAmount = 256;
    
    while(cycleAmount > 0)
    {
        auto Start = high_resolution_clock::now(); 
        
        CPU->checkInterrupts();
        CPU->execOP();

        CPU->cycles /= 4; //get cpu cycles from machine cycles 
        while(CPU->cycles > 0)
        {
            PPU->tick();
            PPU->tick();
                
            //update timers() //TODO
             
            //ppu
            
            CPU->cycles--;
        }

        auto Stop = high_resolution_clock::now(); 
        auto Duration = duration_cast<nanoseconds>(Stop - Start); //get delay of compute time for instruction

       // std::cout << Duration.count() << std::endl;  

        //std::this_thread::sleep_for((2386ns * CPU->cycles) - Duration); //sleep for clock speed minus delay

        cycleAmount--;
    }

    //rest of ROM
    cartridge->romLoad(); //(replace bootrom)
    cycleAmount = 4200000 * 50;
    while(cycleAmount > 0)
    {
        auto Start = high_resolution_clock::now(); 
        
        CPU->checkInterrupts();
        CPU->execOP();

        CPU->cycles /= 4; //get cpu cycles from machine cycles 
        while(CPU->cycles > 0)
        {
            PPU->tick();
            PPU->tick();
                
            //update timers() //TODO
             
            //ppu
            
            CPU->cycles--;
        }

        auto Stop = high_resolution_clock::now(); 
        auto Duration = duration_cast<nanoseconds>(Stop - Start); //get delay of compute time for instruction

        //std::cout << Duration.count() << std::endl;  

        //std::this_thread::sleep_for((2386ns * CPU->cycles) - Duration); //sleep for clock speed minus delay
        
        cycleAmount--;
    }


    PPU->drawTiles();
    while(true)
    {
        printf("done\n");

    }


    return 0;   
}