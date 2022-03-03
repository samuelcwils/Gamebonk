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
unsigned long totalInstructions = 0;


int main()
{     
    int frames = 0;
    cart* cartridge = new cart( (uint8_t*)gRomData, (uint8_t*) gBootData, gRomSize);
   
    ppu* PPU = new ppu();

    bus* Bus = new bus(cartridge, PPU);
    cpu* CPU = new cpu(Bus);

    IO* io = new IO(Bus, PPU->frameBuffer);
    io->createWindow(1024, 512, 160, 144);
    
    PPU->connectBus(Bus);
    Bus->connectCPU(CPU);

    cartridge->printCart();

    while(true)
    {
            if(!(PPU->regs.bytes.LCDC & 0b10000000)) //doesn't tick ppu while not enabled
            {
                CPU->checkInterrupts();
                CPU->execOP();

                while(CPU->cycles > 0)
                {         
                    CPU->updateTimers(1);
                    CPU->cycles--;
                }

                totalInstructions++;

                if(totalInstructions % 100)
                {
                    totalInstructions = 0;
                    io->keyInput();
                }
        
            } else {
                
                auto start = high_resolution_clock::now(); 

            // PPU->regs.bytes.STAT |= 0b00000010; //Gets ppu ready to oam
                
                while(PPU->frameDone == false) //stops after every frame
                {
                    
                    CPU->checkInterrupts();
                    CPU->execOP();
                    
                    while(CPU->cycles > 0)
                    {
                        PPU->tick();

                        CPU->updateTimers(1);
                        
                        CPU->cycles--;
                    }

                    totalInstructions++;

                    if(totalInstructions % 100)
                    {
                        totalInstructions = 0;
                        io->keyInput();
                    }

                }

                io->updateDisplay();

                auto stop = high_resolution_clock::now(); 
                auto waitTime = std::chrono::duration_cast<microseconds>(stop - start);

                frameDone = true;
                
            // std::cout << waitTime.count() << std::endl;

                std::this_thread::sleep_for(16666us - waitTime);

                PPU->frameDone = false;
                
                frames++;
            }

    }

    return 0;   
}