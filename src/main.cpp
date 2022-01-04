#include "pico/stdlib.h"
#include "pico/time.h"
#include "mmu.h"
#include "cart.h"
#include "incbin.h"

extern "C" INCBIN(Game, "tetris.gb"); 

int main()
{  
    
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    
    stdio_init_all();
    
    for(int i = 0; i < 100000; i++)
    {
        printf("INITIALIZING\n");
    }
    
    sleep_ms(5000);

    cart* cartridge = new cart((uint8_t*)gGameData, gGameSize);
    mmu* memCont = new mmu(cartridge);

    cartridge->printCart();

    printf("INITIALIZING\n");

    return 0;   
}