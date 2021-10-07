#include "BrainPadTemperatureSensor.h"

using namespace codal;

BrainPadTemperatureSensor::BrainPadTemperatureSensor(BrainPadPin& pin, uint16_t id) : codal::AnalogSensor(pin, id) {

}

int BrainPadTemperatureSensor::getValue() {
   
	volatile uint32_t* ts_reg1 = (uint32_t*)0x1FFF75A8;
	volatile uint32_t* ts_reg2 = (uint32_t*)0x1FFF75CA;
	volatile uint32_t* enable_reg = (uint32_t*)(0x40000000U + 0x08000000U + 0x08040300U + 8);
	
	volatile uint32_t enable_val = *enable_reg;
	volatile uint32_t ts1;
	volatile uint32_t ts2; 
	volatile double v;
	volatile double t1;
	volatile double t2;
	volatile double t3;
	volatile uint32_t value;

	
	if ((enable_val & 0x00800000) == 0) {
		enable_val |= 0x00800000;
		*enable_reg = enable_val;
	}
	
	ts1 = *ts_reg1;
	ts2 = *ts_reg2;
	
	v = codal::AnalogSensor::getValue() * 1.0;
	
	ts1 &= 0xFFFF;
	ts2 &= 0xFFFF;
	v *= 1.1;
	
	t1 = (110 - 30) * 1.0;
	t2 = (ts2 - ts1) * 1.0;
	t3 = (v - ts1) * 1.0;

	value = t1 / t2 * t3 + 30;
	
	return value;
}
