#ifndef BPLIGHTSENSOR_H
#define BPLIGHTSENSOR_H

#include "BrainPadPin.h"
#include "AnalogSensor.h"

namespace codal {
    class BrainPadLightSensor : public codal::AnalogSensor {
    public:
        BrainPadLightSensor(BrainPadPin& pin, uint16_t id);
    };
}

#endif
