#ifndef BPDISPLAY_H
#define BPDISPLAY_H

#include "I2C.h"
#include "mbed.h"
#include "CodalConfig.h"
#include "MbedPin.h"
#include "BrainPadPin.h"

#include <cstdint>

#define DISPLAY_DEFAULT_ADDRESS 0x78
#define RESET_PIN_NAME PB_2
#define BRAINPADPULSE 1


namespace codal {
    class BrainPadDisplay {
        static const int vramSize = (128 * 64 / 8) + 1;

        codal::I2C& i2c;
        uint8_t data[2];
        uint8_t vram[vramSize];
        uint16_t address;

        void writeCommand(int command);
        void flush();

    public:
        BrainPadDisplay(codal::I2C& _i2c, uint16_t address = DISPLAY_DEFAULT_ADDRESS);

        void writeScreenBuffer(uint8_t* buffer);
    };
}

#endif
