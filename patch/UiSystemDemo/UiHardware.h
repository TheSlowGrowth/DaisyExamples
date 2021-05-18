#pragma once

#include "daisy_patch.h"

/** To make it easier to refer to specific potentiometers in the 
 *  code, we use this enum to give each potentiometer ID a specific name.
 */
enum PotentiometerIds
{
    potValueSlider = 0,
    pot1,
    pot2,
    NUM_POTS
};

/** This class serves as a "backend" for the PotMonitor:
 *  In different projects, reading the potentiometers could happen in 
 *  different ways - and the PotMonitor must be agnostic to the hardware.
 *  This class connects the PotMonitor to the actual hardware by
 *  providing the position of the potentiometers to the PotMonitor via its
 *  GetPotValue() function.
 *  
 *  This class initialises two analog inputs in Daisy Seed pin 15 and 16
 *  and provides their value to the PotMonitor.
 *  In other projects, scanning pots could involve more complex tasks such
 *  as multiplexing or reading values from SPI. This can all be stuffed in
 *  this class.
 */
class PotMonitorBackend
{
  public:
    void Init()
    {
        // Init ADC channels with Pins
        daisy::AdcChannelConfig cfg[NUM_POTS];
        cfg[potValueSlider].InitSingle(daisy::DaisySeed::GetPin(15));
        cfg[pot1].InitSingle(daisy::DaisySeed::GetPin(16));
        cfg[pot2].InitSingle(daisy::DaisySeed::GetPin(17));
        adc_.Init(cfg, NUM_POTS);

        // start scanning ADC in the background
        adc_.Start();
    }

    float GetPotValue(uint16_t potId)
    {
        if(potId < NUM_POTS)
            return adc_.GetFloat(potId);
        return 0.0f;
    }

  private:
    daisy::AdcHandle adc_;
};

/** To make it easier to refer to specific buttons in the 
 *  code, we use this enum to give each button ID a specific name.
 */
enum ButtonIds
{
    bttnOkay = 0,
    bttnCancel,
    bttnLeft,
    bttnRight,
    bttnUp,
    bttnDown,
    NUM_BUTTONS
};

/** This class serves as a "backend" for the ButtonMonitor:
 *  In different projects, reading the buttons could happen in 
 *  different ways - and the ButtonMonitor must be agnostic to the hardware.
 *  This class connects the ButtonMonitor to the actual hardware by
 *  providing the state of the buttons to the ButtonMonitor via its
 *  IsButtonPressed() function.
 *  
 *  In our case, the buttons are directly connected to Daisy Seed pins 0-3.
 *  As the ButtonMonitor is configured to do debouncing for us, all we have to
 *  do here is to provide the state of each of the analog inputs directly.
 */
class ButtonMonitorBackend
{
  public:
    void Init()
    {
        gpio_[bttnOkay].mode = DSY_GPIO_MODE_INPUT;
        gpio_[bttnOkay].pull = DSY_GPIO_PULLDOWN;
        gpio_[bttnOkay].pin  = daisy::DaisySeed::GetPin(0);
        dsy_gpio_init(&gpio_[bttnOkay]);

        gpio_[bttnCancel].mode = DSY_GPIO_MODE_INPUT;
        gpio_[bttnCancel].pull = DSY_GPIO_PULLDOWN;
        gpio_[bttnCancel].pin  = daisy::DaisySeed::GetPin(1);
        dsy_gpio_init(&gpio_[bttnCancel]);

        gpio_[bttnLeft].mode = DSY_GPIO_MODE_INPUT;
        gpio_[bttnLeft].pull = DSY_GPIO_PULLDOWN;
        gpio_[bttnLeft].pin  = daisy::DaisySeed::GetPin(2);
        dsy_gpio_init(&gpio_[bttnLeft]);

        gpio_[bttnRight].mode = DSY_GPIO_MODE_INPUT;
        gpio_[bttnRight].pull = DSY_GPIO_PULLDOWN;
        gpio_[bttnRight].pin  = daisy::DaisySeed::GetPin(3);
        dsy_gpio_init(&gpio_[bttnRight]);

        gpio_[bttnUp].mode = DSY_GPIO_MODE_INPUT;
        gpio_[bttnUp].pull = DSY_GPIO_PULLDOWN;
        gpio_[bttnUp].pin  = daisy::DaisySeed::GetPin(4);
        dsy_gpio_init(&gpio_[bttnUp]);

        gpio_[bttnDown].mode = DSY_GPIO_MODE_INPUT;
        gpio_[bttnDown].pull = DSY_GPIO_PULLDOWN;
        gpio_[bttnDown].pin  = daisy::DaisySeed::GetPin(5);
        dsy_gpio_init(&gpio_[bttnDown]);
    }

    bool IsButtonPressed(uint16_t buttonId)
    {
        if(buttonId < NUM_BUTTONS)
            return dsy_gpio_read(&gpio_[buttonId]);
        return false;
    }

  private:
    dsy_gpio gpio_[NUM_BUTTONS];
};

/** To make it easier to refer to specific canvases in the 
 *  code, we use this enum to give each canvas ID a specific name.
 */
enum CanvasIds
{
    canvasOledDisplay = 0,
    canvasLed,
    NUM_CANVASES
};

/** This is the type of display we're using in this example. */
using OledDisplayType = daisy::OledDisplay<daisy::SSD130x4WireSpi128x64Driver>;