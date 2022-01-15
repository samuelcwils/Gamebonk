#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "incbin.h"
#include "IO.h"
extern "C" INCBIN(Game, "gameboy.gb"); 

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

    CPU->execOP();

    cartridge->printCart();

    printf("INITIALIZING\n");

    return 0;   
}