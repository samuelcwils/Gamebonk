#pragma once
#include "cpu.h"
#include "array"
#include "cart.h"
#include "ppu.h"
#include "stdint.h"

class cart;
class ppu;
class cpu;

class bus {
private:
    uint8_t wRam[0x2000];
    uint8_t hRam[126];

    uint8_t serial[2];

    struct 
    {
        cart* Cart;
        ppu* PPU;
        cpu* CPU;
    } memoryMap;
    
public:
    bus(cart* Cart, ppu* PPU);
    void connectCPU(cpu* CPU);
    void write(uint16_t address, uint8_t byte);
    uint8_t read(uint16_t address);
    void interruptFlags(uint8_t flag); //ORs input flags with IF register

    uint8_t joypad;
    uint8_t joypad_state;
    int busTicks; //tick other systems after CPU read
    bool busTickEnable; //don't ticker other systems when not being read by the CPU
    bool PPU_read;
};

