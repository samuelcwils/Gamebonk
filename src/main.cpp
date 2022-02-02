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

void doSDL(IO* io)
{
    if(frameDone){
        io->updateDisplay();
    }
}


int main()
{     
    int frames = 0;
    cart* cartridge = new cart( (uint8_t*)gRomData, (uint8_t*) gBootData, gRomSize);
   
    ppu* PPU = new ppu();
    uint16_t framebuffer[160*144];
    PPU->frameBuffer = framebuffer;

    bus* Bus = new bus(cartridge, PPU);
    cpu* CPU = new cpu(Bus);

    IO* io = new IO(CPU->IF, framebuffer);
    io->createWindow(1024, 512, 160, 144);
    
    PPU->connectBus(Bus);
    Bus->connectCPU(CPU);
    PPU->connectCPU(CPU);

   std::thread SDL(doSDL, io);

    cartridge->printCart();

    //////////////////////////////////////////////////////////////////boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM 

    cartridge->romLoad(); //(load rom then overlay bootrom)
    cartridge->bootRomLoad();
    
    while(CPU->bootromDone == false) //does bootrom then ends
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
            
            while(PPU->frameDone == false) //stops after every frame
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

                if(!(totalCycles % 1000))
                {
                    io->keyInput();
                }
            }

            io->updateDisplay();
            auto stop = high_resolution_clock::now(); 
            auto waitTime = std::chrono::duration_cast<microseconds>(stop - start);

            frameDone = true;
            
            std::cout << waitTime.count() << std::endl;

            //std::this_thread::sleep_for(16666us - waitTime);

            PPU->frameDone = false;
            
            frames++;
            totalCycles = 0;
        }

    }
    
    //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM //boot ROM 
    /////////////////////////////////////////////////////////////////////////////

  
  
    //////////////////////////////////////////////////////////////////////////////
    //rest of ROM //rest of ROM //rest of ROM //rest of ROM //rest of ROM
        cartridge->romLoad(); //(replace bootrom)
        CPU->pc.pc = 0x0100;
        CPU->debug = true;
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
            
            while(PPU->frameDone == false) //stops after every frame
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

                if(!(totalCycles % 1000))
                {
                    io->keyInput();
                }
            }

            io->updateDisplay();
            auto stop = high_resolution_clock::now(); 
            auto waitTime = std::chrono::duration_cast<microseconds>(stop - start);

            frameDone = true;
            
            std::cout << waitTime.count() << std::endl;

            std::this_thread::sleep_for(16666us - waitTime);

            PPU->frameDone = false;
            
            frames++;
            totalCycles = 0;
        }

    //rest of ROM //rest of ROM //rest of ROM //rest of ROM //rest of ROM
    //////////////////////////////////////////////////////////////////////////////

    return 0;   
}