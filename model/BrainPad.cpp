/*
The MIT License (MIT)

Copyright (c) 2016 Lancaster University, UK.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/


#include "BrainPad.h"
#include "Timer.h"

using namespace codal;

static BrainPad *device_instance = NULL;

/**
  * Constructor.
  *
  * Create a representation of a GenuinoZero device, which includes member variables
  * that represent various device drivers used to control aspects of the micro:bit.
  */
BrainPad::BrainPad() :
    timer(),
    messageBus(),
    io(),
    i2c(io.sda, io.scl),
    lcd(i2c),
    buttonA(io.buttonA, ID_PIN_BUTTON_A, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up),
    buttonB(io.buttonB, ID_PIN_BUTTON_B, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up),
    // buttonLeft(io.buttonLeft, ID_PIN_BUTTON_LEFT, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up),
    // buttonRight(io.buttonRight, ID_PIN_BUTTON_RIGHT, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up),
    space(CoordinateSystem::SIMPLE_CARTESIAN, false, COORDINATE_SPACE_ROTATED_0),
    accelerometer(i2c, io.interrupt, space),
    temperatureSensor(io.temperaturePin, DEVICE_ID_THERMOMETER),
    lightSensor(io.lightPin, DEVICE_ID_LIGHT_SENSOR)
{
    // Clear our status
    status = 0;

    device_instance = this;
}

/**
  * Post constructor initialisation method.
  *
  * This call will initialised the scheduler, memory allocator and Bluetooth stack.
  *
  * This is required as the Bluetooth stack can't be brought up in a
  * static context i.e. in a constructor.
  *
  * @code
  * uBit.init();
  * @endcode
  *
  * @note This method must be called before user code utilises any functionality
  *       contained within the GenuinoZero class.
  */
int BrainPad::init()
{
    if (status & DEVICE_INITIALIZED)
        return DEVICE_NOT_SUPPORTED;

    status |= DEVICE_INITIALIZED;

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    for(int i = 0; i < DEVICE_COMPONENT_COUNT; i++)
    {
        if(CodalComponent::components[i])
            CodalComponent::components[i]->init();
    }

    // Seed our random number generator
    //seedRandom();

    codal_dmesg_set_flush_fn(brainpad_dmesg_flush);
    status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;

    return DEVICE_OK;
}

/**
  * A periodic callback invoked by the fiber scheduler idle thread.
  * We use this for any low priority, backgrounf housekeeping.
  *
  */
void BrainPad::idleCallback()
{
    codal_dmesg_flush();
}

void brainpad_dmesg_flush()
{
#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    if (codalLogStore.ptr > 0 && device_instance)
    {
        for (uint32_t i=0; i<codalLogStore.ptr; i++)
            ((BrainPad *)device_instance)->serial.putc(codalLogStore.buffer[i]);

        codalLogStore.ptr = 0;
    }
#endif
#endif
}
