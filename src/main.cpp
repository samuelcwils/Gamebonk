#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "incbin.h"
#include "IO.h"
#include <chrono>
#include <thread>

extern "C" INCBIN(Game, "gameboy.gb"); 

using namespace std::chrono_literals;
using namespace std::chrono;


int main()
{      
    for(int i = 0; i < 100000; i++)
    {
        printf("INITIALIZING\n");
    }
    
    cart* cartridge = new cart((uint8_t*)gGameData, gGameSize);
    
    ppu* PPU = new ppu();

    bus* Bus = new bus(cartridge, PPU);

    cpu* CPU = new cpu(Bus);

    IO* io = new IO(CPU->IF);

    io->createWindow(1024, 512, 160, 140);

    Bus->connectCPU(CPU);

    cartridge->printCart();

    uint8_t ppuClock = 0;
    int SDL_Timer = 0;

    while(true)
    {
        auto Start = high_resolution_clock::now(); 
        
        CPU->execOP();

        auto Stop = high_resolution_clock::now(); 

        CPU->cycles /= 4; //get cpu cycles from machine cycles
        
        while(CPU->cycles > 0)
        {
            CPU->checkInterrupts();
            
            //update timers() //TODO
            
            ppuClock++;
            SDL_Timer++;
            
            //ppu
            //sdl

            CPU->cycles--;
        }
        
        

        auto Duration = duration_cast<nanoseconds>(Stop - Start); //get delay of compute time for instruction

        std::cout << Duration.count() << std::endl;  

        std::this_thread::sleep_for(2386ns - Duration); //sleep for clock speed minus delay
   
    }

    printf("INITIALIZING\n");

    return 0;   
}