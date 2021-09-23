#ifndef MC3216_H
#define MC3216_H

#include "CoordinateSystem.h"
#include "Pin.h"
#include "I2C.h"
#include "Accelerometer.h"

#ifdef ACCEL_G428
#define MC3216_DEFAULT_ADDR  (0x1C << 1)
#else
#define MC3216_DEFAULT_ADDR  (0x4C<<1)
#endif

const int MC3216_Mode = 0x07;
const int MC3216_Opstat = 0x04;
const int MC3216_Outcfg = 0x20;
const int MC3216_XOut = 0x0D;
const int MC3216_YOut = 0x0F;
const int MC3216_ZOut = 0x11;
const int MC3216_SRTFR = 0x08;


namespace codal {
    class MC3216 : public Accelerometer {
        codal::I2C& i2c;
        Pin& int1;
        uint16_t          address;
        bool 			  mylocker;

        void writeRegister(uint8_t reg, uint8_t val);
		unsigned long current_ms;

    public:
        MC3216(codal::I2C& _i2c, Pin& int1, CoordinateSpace& coordinateSpace, uint16_t address = MC3216_DEFAULT_ADDR, uint16_t id = DEVICE_ID_ACCELEROMETER);
        ~MC3216();

        int updateSample();
        virtual int configure();
        virtual int requestUpdate();
        virtual void idleCallback();
    };
}

#endif
