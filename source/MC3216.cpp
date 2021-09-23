#include "MC3216.h"
#include "CodalConfig.h"
#include "ErrorNo.h"
#include "CodalCompat.h"
#include "CodalFiber.h"
#include "platform/mbed_wait_api.h"

#include "BrainPad.h"
#include "Timer.h"

using namespace codal;

static const KeyValueTableEntry rangeRegisterData[] = {
    { 2, 0x00 },
    { 4, 0x01 },
    { 8, 0x02 },
};

static const KeyValueTableEntry rangeDivisorData[] = {
    { 2, 256 },
    { 4, 128 },
    { 8, 64 },
};

CREATE_KEY_VALUE_TABLE(rangeRegister, rangeRegisterData);
CREATE_KEY_VALUE_TABLE(rangeDivisor, rangeDivisorData);


MC3216::MC3216(codal::I2C& _i2c, Pin& _int1, CoordinateSpace& coordinateSpace, uint16_t address, uint16_t id) : Accelerometer(coordinateSpace, id), i2c(_i2c), int1(_int1) {

    this->address = address;
	this->mylocker = false;
	this->current_ms = system_timer_current_time();

    configure();
}

MC3216::~MC3216() {

}

void MC3216::writeRegister(uint8_t reg, uint8_t val) {
    uint8_t data[2];

    data[0] = reg;
    data[1] = val;

    if (&i2c == nullptr)
		return;
	
	i2c.write(address, data, 2, 0);
}

int MC3216::updateSample() {
    if (this->mylocker) {
		// while (this->mylocker) {
			// wait_ms(1);
		// }

		return DEVICE_OK;
	}
	
		
	if (&i2c == nullptr)
		return DEVICE_OK;

	
	
	
   
	
    

    if (system_timer_current_time() - 100 > this->current_ms) {
		
#ifdef ACCEL_G428
		uint8_t read[6];
		
		i2c.readRegister(address, 1, read, 6);
		
		int32_t x = read[0] << 2 | read[1] >> 6 & 0x3F;
		if (x > 511)
			x = x - 1024;

		int32_t y = read[2] << 2 | read[3] >> 6 & 0x3F;
		if (y > 511)
			y = y - 1024;

		int32_t z = read[4] << 2 | read[5] >> 6 & 0x3F;
		if (z > 511)
			z = z - 1024;
#else		
		uint8_t data[1];

        i2c.readRegister(address, MC3216_XOut, data, 1);		
		int32_t x = data[0];
		
		i2c.readRegister(address, MC3216_YOut, data, 1);		
		int32_t y = data[0];
		
		i2c.readRegister(address, MC3216_ZOut, data, 1);		
		int32_t z = data[0];
#endif
				
		
		if (x > 127) x -= 256;
        if (y > 127) y -= 256;
        if (z > 127) z -= 256;
		

		x = x * 1024 / 64;
		y = y * 1024 / 64;
		z = z * 1024 / 64;
		
		x = x > 1024 ? 1024: x;
		y = y > 1024 ? 1024: y;
		z = z > 1024 ? 1024: z;
		
		
		x = x < -1024 ? -1024: x;
		y = y < -1024 ? -1024: y;
		z = z < -1024 ? -1024: z;
		
		
		this->mylocker = true;
		
        update({ x, y, z }); //To transform to ENU
	   
		this->current_ms = system_timer_current_time();
		
		this->mylocker = false;
    }

    return DEVICE_OK;
}

int MC3216::configure() {
    // writeRegister(CTRL_REG1, CTRL_REG1_SLEEP);

    // writeRegister(XYZ_DATA_CFG, rangeRegister.get(getRange()));
    // writeRegister(CTRL_REG4, CTRL_REG4_INT_DATA);
    // writeRegister(CTRL_REG5, CTRL_REG5_SET_INT_PIN);
    // writeRegister(CTRL_REG1, CTRL_REG1_ACTIVE);
#ifdef ACCEL_G428	

	 writeRegister(0x2A, 1);
#else			
	uint8_t data[1];

	writeRegister(MC3216_Outcfg, 2);
	writeRegister(MC3216_Mode, 1);
	
// wait_wake:
	// i2c.readRegister(address, MC3216_Opstat, data, 1);
	
	// if ((data[0] & 0x1) != 0x1 )
		// goto wait_wake;
#endif
    return DEVICE_OK;
}

int MC3216::requestUpdate() {
    updateSample();

    return DEVICE_OK;
}

void MC3216::idleCallback() {
    updateSample();
}
