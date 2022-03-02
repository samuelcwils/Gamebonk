#include "cart.h"

    void cart::MBC1writeRom(uint16_t address, uint8_t value)
    {
        if((address <= 0x3fff) && (address >= 0x2000)) //ROM bank first 5 bits
        {
            if(value == 0)
            {
                romBankNum &= 0b01100000; // sets to 1 on zero write
                romBankNum += 1;
            } else {
                romBankNum &= 0b01100000;
                romBankNum |= ( (value & 0b00011111) & (bankBits) ); 
            } 

        } else if(address <= 0x5fff) // Ram bank num / upper 2 bits of ROM
        {
            if((ramSize >= 0x8000) && (ramBanking))
            {
                ramBankNum = value;
                ramBank += (ramBankNum * 0x2000);

            } else if(romSize >= 0x100000)
            {
                romBankNum |= (value << 5);
            }

        } else if(address <= 0x7fff) //rank banking register
        {
            ramBanking = value;
        }

    }

    void cart::MBC1writeRam(uint16_t address, uint8_t value)
    {
        ramBank[address] = value;
    }