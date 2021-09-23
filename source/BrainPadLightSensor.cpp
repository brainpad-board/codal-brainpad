#include "BrainPadLightSensor.h"

using namespace codal;

BrainPadLightSensor::BrainPadLightSensor(BrainPadPin& pin, uint16_t id) : codal::AnalogSensor(pin, id) {

}
