#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "incbin.h"
#include "IO.h"
#include <chrono>
#include <thread>

extern "C" INCBIN(Game, "gameboy.gb"); 

using namespace std::chrono_literals;

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

        using namespace std::chrono;
        
        auto start = high_resolution_clock::now();
        
        CPU->execOP();
        
        auto stop = high_resolution_clock::now(); 
        auto duration = duration_cast<nanoseconds>(stop - start);

        auto waitTime = 2400ns - duration - 140ns;
  
        printf("duration is : %ld\n", duration.count());

       // auto waitTime = CPU->cycles;
        
        for(CPU->cycles; CPU->cycles > 0; CPU->cycles--)
        {
            CPU->checkInterrupts();
            //update timers() //TODO
            ppuClock++;
            SDL_Timer++;
            
            if( (ppuClock % 2) == 0 ){
                //run ppu
            }

            //if(SDL_Timer % 1000)

            auto waitTime = 2400ns - duration;

            //std::cout << waitTime.count() << std::endl;             
        }

       // std::this_thread::sleep_for((waitTime)); //140 is for the time it takes to measure
   
    }

    printf("INITIALIZING\n");

    return 0;   
}