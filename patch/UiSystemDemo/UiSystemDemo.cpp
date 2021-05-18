#include "daisy_seed.h"
using namespace daisy;

// In this file we define the UI hardware and how it can be accessed.
#include "UiHardware.h"

// This file defines the various UiPages in this demo
#include "UiPages.h"

DaisySeed seed;

// This is the event queue that collects and stores events
// from our user interface hardware.
UiEventQueue eventQueue;

// This backend provides the state of each of our buttons.
ButtonMonitorBackend buttonMonitorBackend;
// This ButtonMonitor processes the raw button states from the
// buttonMonitorBackend. It debounces them and generates events
// such as "button pressed" or "button released" which it then
// posts to the eventQueue.
ButtonMonitor<ButtonMonitorBackend, NUM_BUTTONS> buttonMonitor;

// This backend provides the analog value of each of our knobs.
PotMonitorBackend potMonitorBackend;
// This PotMonitor processes the raw values from the
// potMonitorBackend. It generates events like "pot moved",
// "pot activity changed", etc. and posts them to the eventQueue.
PotMonitor<PotMonitorBackend, NUM_POTS> potMonitor;

// This is our OLED display
OledDisplayType oledDisplay;

// This is the main UI system class. It reads events from our event queue
// and dispatches them to UI pages.
UI ui;

void InitUiHardware()
{
    // initialise the buttons
    buttonMonitorBackend.Init();
    buttonMonitor.Init(
        eventQueue, // the event queue to which the monitor will post events
        buttonMonitorBackend, // the backend from which it will request the raw button state
        50,                   // the debounce time in milliseconds
        500);                 // the double-click time in milliseconds

    // initialise the potentiometers
    potMonitorBackend.Init();
    potMonitor.Init(
        eventQueue, // the event queue to which the monitor will post events
        potMonitorBackend, // the backend from which it will request the raw potentiometer position
        1000); // the time in milliseconds before a pot will enter the "idle" state

    // initialises the OledDisplay
    OledDisplay<SSD130x4WireSpi128x64Driver>::Config display_config;
    display_config.driver_config.transport_config.pin_config.dc
        = DaisySeed::GetPin(9);
    display_config.driver_config.transport_config.pin_config.reset
        = DaisySeed::GetPin(30);
    oledDisplay.Init(display_config);
}

// These will be called from the UI system. @see InitUi()
void FlushCanvas(const UiCanvasDescriptor& canvasDescriptor)
{
    if(canvasDescriptor.id_ == canvasOledDisplay)
    {
        OledDisplayType& display
            = *((OledDisplayType*)(canvasDescriptor.handle_));
        display.Update();
    }
    // TODO
}
void ClearCanvas(const UiCanvasDescriptor& canvasDescriptor)
{
    if(canvasDescriptor.id_ == canvasOledDisplay)
    {
        OledDisplayType& display
            = *((OledDisplayType*)(canvasDescriptor.handle_));
        display.Fill(false);
    }
    // TODO
}

void InitUi()
{
    // The UI can handle typical button functions, such as "okay", "cancel",
    // or arrow buttons. We have to tell the UI whic IDs should be used for these
    // special functions. Some of these special buttons may not be present on our hardware.
    // We don't have to provide them.
    UI::SpecialButtonIds specialButtonIds;
    specialButtonIds.okBttnId     = bttnOkay;
    specialButtonIds.cancelBttnId = bttnCancel;
    specialButtonIds.leftBttnId   = bttnLeft;
    specialButtonIds.rightBttnId  = bttnRight;

    // The UI can handle multiple canvases. Each canvas could be a text display, some LEDs,
    // a graphics display or any combination. Each canvas is painted at its own refresh rate.
    // Here we prepare CanvasDescriptors that tell our UI that we have two canvases - a display and an LED.
    // The ClearCanvas() and FlushCanvas() functions will be called before and after the painting
    // respectively.
    UiCanvasDescriptor oledDisplayDescriptor;
    oledDisplayDescriptor.id_     = canvasOledDisplay; // a unique ID
    oledDisplayDescriptor.handle_ = &oledDisplay; // a pointer to the display
    oledDisplayDescriptor.updateRateMs_  = 50;    // 50ms == 20Hz
    oledDisplayDescriptor.clearFunction_ = &ClearCanvas;
    oledDisplayDescriptor.flushFunction_ = &FlushCanvas;

    UiCanvasDescriptor ledDisplayDescriptor;
    ledDisplayDescriptor.id_            = canvasLed; // a unique ID
    ledDisplayDescriptor.handle_        = nullptr;   // TODO
    ledDisplayDescriptor.updateRateMs_  = 20;        // 20ms == 50Hz
    ledDisplayDescriptor.clearFunction_ = &ClearCanvas;
    ledDisplayDescriptor.flushFunction_ = &FlushCanvas;

    ui.Init(eventQueue, // The event queue to read user input from
            specialButtonIds,
            {oledDisplayDescriptor, ledDisplayDescriptor});

    // Now that the UI is initialised, we can add our first page to it.
    // Without any page, the UI would still update the canvases, but they would
    // be empty.
    ui.OpenPage(uiBasePage);
}

void AudioCallback(float** in, float** out, size_t size)
{
    // Note that we're not processing any hardware here.
    // This callback is running at the highest priority and
    // we can handle our user interface from the lower priority
    // main() loop to leave more processing "power" to the audio
    // processing.

    for(size_t i = 0; i < size; i++)
    {
        out[0][i] = in[0][i];
        out[1][i] = in[1][i];
    }
}


int main(void)
{
    seed.Init();

    InitUiHardware();
    InitUi();

    seed.StartAudio(AudioCallback);

    /* This is the main loop - what happens here will be executed with 
	   whatever CPU time is left after the AudioCallback() is done.
	*/
    while(1)
    {
        // First: Process the hardware and generate new events for the UiEventQueue
        buttonMonitor.Process();
        potMonitor.Process();

        // Second: trigger the UI system to dispatch the events and redraw the UI pages
        // should it be required.
        ui.Process();
    }
}