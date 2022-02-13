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
unsigned long totalCycles = 0;


int main()
{     
    int frames = 0;
    cart* cartridge = new cart( (uint8_t*)gRomData, (uint8_t*) gBootData, gRomSize);
   
    ppu* PPU = new ppu();

    bus* Bus = new bus(cartridge, PPU);
    cpu* CPU = new cpu(Bus);

    IO* io = new IO(CPU->IF, PPU->frameBuffer);
    io->createWindow(1024, 512, 160, 144);
    
    PPU->connectBus(Bus);
    Bus->connectCPU(CPU);
    PPU->connectCPU(CPU);

    cartridge->printCart();

    ////////////////////////
    //////////////////////////////////////////boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM

    CPU->bootRomDone = false; 
    
    while(!(CPU->bootRomDone)) //does bootrom then ends
    {
        if(!(PPU->regs.bytes.LCDC & 0b10000000)) //doesn't tick ppu while not enabled
        {

            CPU->checkInterrupts();
            CPU->execOP();
            
            CPU->cycles /= 4; //get cpu cycles from machine cycles 

            while(CPU->cycles > 0)
            {
                // if(PPU->regs.bytes.LCDC & 0b10000000)
                // {
                //     PPU->tick();
                //     PPU->tick();
                // }
                
                //update timers() //TODO
                
                CPU->cycles--;
            }
       
        } else {
            
            auto start = high_resolution_clock::now(); 

            PPU->regs.bytes.STAT |= 0b00000010; //Gets ppu ready to oam
            
            while(!(PPU->frameDone) && (CPU->bootRomDone == false)) //stops after every frame
            {
                CPU->checkInterrupts();
                CPU->execOP();

                while(CPU->cycles > 0)
                {
                    PPU->tick();
       
                    //update timers() //TODO
                    
                    //ppu
                    
                    CPU->cycles--;
                }

            }

            io->keyInput();
            io->updateDisplay();

            auto stop = high_resolution_clock::now(); 
            auto waitTime = std::chrono::duration_cast<microseconds>(stop - start);

            totalCycles = 0;
            frameDone = true;
            
           // std::cout << waitTime.count() << std::endl;

            std::this_thread::sleep_for(16666us - waitTime);

            PPU->frameDone = false;
            
            frames++;
            totalCycles = 0;
        }

    }
    
    //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM 
    /////////////////////////////////////////////////////////////////////////////

    for(int i = 2; i > 0; i--)
    {
        printf("REST OF ROM\n");
    }
  
    //////////////////////////////////////////////////////////////////////////////
    //rest of ROM //rest of ROM //rest of ROM //rest of ROM //rest of ROM
    //  
        cartridge->staticBankLD(); //(replace bootrom)
        CPU->pc.pc = 0x100;

while(true)
{
        if(!(PPU->regs.bytes.LCDC & 0b10000000)) //doesn't tick ppu while not enabled
        {
            CPU->checkInterrupts();
            CPU->execOP();
            
            CPU->cycles /= 4; //get cpu cycles from machine cycles 

            while(CPU->cycles > 0)
            {         
                CPU->cycles--;
            }
       
        } else {
            
            auto start = high_resolution_clock::now(); 

            PPU->regs.bytes.STAT |= 0b00000010; //Gets ppu ready to oam
            
            while(PPU->frameDone == false) //stops after every frame
            {
                
                CPU->checkInterrupts();
                CPU->execOP();
                
                while(CPU->cycles > 0)
                {
                    PPU->tick();

                    //update timers() //TODO
                    
                    CPU->cycles--;
                }

            }

            io->keyInput();
            io->updateDisplay();

            auto stop = high_resolution_clock::now(); 
            auto waitTime = std::chrono::duration_cast<microseconds>(stop - start);

            totalCycles = 0;
            frameDone = true;
            
           // std::cout << waitTime.count() << std::endl;

            std::this_thread::sleep_for(16666us - waitTime);

            PPU->frameDone = false;
            
            frames++;
            totalCycles = 0;
        }

}

    //rest of ROM //rest of ROM //rest of ROM //rest of ROM //rest of ROM
    //////////////////////////////////////////////////////////////////////////////

    return 0;   
}