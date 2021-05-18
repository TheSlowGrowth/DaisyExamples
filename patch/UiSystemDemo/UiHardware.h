#pragma once

#include "daisy_patch.h"
#include "daisy.h"

extern daisy::DaisyPatch hw;

/** To make it easier to refer to specific buttons in the 
 *  code, we use this enum to give each button ID a specific name.
 */
enum ButtonIds
{
    bttnEncoder = 0,
    // We don't have any more buttons on the Patch, but if there were more, you'd add them here
    NUM_BUTTONS
};

/** To make it easier to refer to specific encoders in the 
 *  code, we use this enum to give each encoder ID a specific name.
 */
enum EncoderIds
{
    encoderMain = 0,
    // We don't have any more encoders on the Patch, but if there were more, you'd add them here
    NUM_ENCODERS
};

/** To make it easier to refer to specific canvases in the 
 *  code, we use this enum to give each canvas ID a specific name.
 */
enum CanvasIds
{
    // This canvas is for the OLED display
    canvasOledDisplay = 0,
    // We use this canvas to update the "User LED" on the Seed.
    // It's a little bit overkill to turn a single LED into its 
    // own little canvas, but let's just imagine we had more LEDs
    // and wanted to update them faster than the main OLED display.
    // Turning them into a separate canvas allows us to have them
    // updated at another update rate. Also, pages on the UI can choose
    // to be transparent on a particular canvas while being opaque on 
    // another one.
    canvasLed,

    NUM_CANVASES
};

/** This is the type of display we use on the patch. This is provided here for better readability. */
using OledDisplayType = decltype(daisy::DaisyPatch::display);

// These will be called from the UI system. @see InitUi() in UiSystemDemo.cpp
void FlushCanvas(const daisy::UiCanvasDescriptor& canvasDescriptor)
{
    if(canvasDescriptor.id_ == canvasOledDisplay)
    {
        OledDisplayType& display
            = *((OledDisplayType*)(canvasDescriptor.handle_));
        display.Update();
    }
    else if(canvasDescriptor.id_ == canvasLed)
    {
        // nothing to do here, the LED is udpated directly from a GPIO pin.
    }
}
void ClearCanvas(const daisy::UiCanvasDescriptor& canvasDescriptor)
{
    if(canvasDescriptor.id_ == canvasOledDisplay)
    {
        OledDisplayType& display
            = *((OledDisplayType*)(canvasDescriptor.handle_));
        display.Fill(false);
    }
    else if(canvasDescriptor.id_ == canvasLed)
    {
        hw.seed.SetLed(false);
    }
}
