#include "bus.h"
#include "cart.h"
#include "incbin.h"
#include "Emulator_SDL.h"
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

    cartridge->printCart();

    printf("INITIALIZING\n");

    return 0;   
}