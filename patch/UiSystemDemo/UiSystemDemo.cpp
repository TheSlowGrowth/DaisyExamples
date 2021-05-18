#include "daisy_patch.h"

#include "UiHardware.h"
#include "CustomPages.h"

using namespace daisy;

// This is the event queue that collects and stores events
// from our user interface hardware.
UiEventQueue eventQueue;

// This is the main UI system class. It reads events from our event queue
// and dispatches them to UI pages. Inside this UI, multiple pages can 
// be opened on top of each other. 
// User input events such as button presses will be dispatched to the UiPages
// from top to bottom until a page handles the event.
// Pages are drawn to the canvases from bottom to top.
// This allows to create all sorts of pages like menus, popups, file picker
// dialogs, etc. 
UI ui;

// These are our two menus and the custom page for this example
FullScreenItemMenu mainMenu;
FullScreenItemMenu valueEditMenu;
CustomPage customPage;
PopupInfoBox popup;

// The menus need some items to display. They're stored in these arrays,
// which we will pass to the menus when we initialize them.
const int                kNumMainMenuItems = 3;
AbstractMenu::ItemConfig mainMenuItems[kNumMainMenuItems];
const int                kNumValueEditMenuItems = 6;
AbstractMenu::ItemConfig valueEditMenuItems[kNumValueEditMenuItems];

// In the value edit menu, we want to have some items that edit some values.
// In a real project, we could use this to edit values that we don't provide
// a dedicated hardware control for.
// Each of these values needs some additional information, like ranges,
// mapping, units, etc. We define them here.
// Each of these values implements the `MappedValue` interface. Should you find
// that none of the existing MappedValues fit your needs, you can create your 
// own version and use it in a menu just like the other MappedValues.

// a linearly mapped floating point value
MappedFloatValue floatValueLin(-10.0f, // min
                               10.0f,  // max
                               0.0f,   // initial/default value
                               daisy::MappedFloatValue::Mapping::lin,
                               "s",   // unit
                               1,     // num decimals to display
                               true); // show "+" sign as well
// a logarithmically mapped floating point value
MappedFloatValue floatValueLog(1.0f,   // min
                               100.0f, // max
                               10.0f,  // initial/default value
                               daisy::MappedFloatValue::Mapping::log,
                               "Hz",   // unit
                               1,      // num decimals to display
                               false); // show only the "-" sign if requried.
// an integer value
MappedIntValue intValue(0,   // min
                        10,  // max
                        0,   // default value
                        1,   // step size when editing
                        10); // step size when the function key is pressed
                             // - we don't have one on the patch.

// A list of strings
const char* stringListValues[] = {"Item A", "Item B", "Item C", "Item D"};
MappedStringListValue stringListValue(
    stringListValues, // the list of strings to display for each item
    4,                // the number of items
    0);               // initial/default value

// a simple boolean value that doesn't need any particular settings
bool boolValue = true;


DaisyPatch hw;

void InitUi()
{
    // The UI can handle typical button functions, such as "okay", "cancel",
    // or arrow buttons. We have to tell the UI which IDs should be used for these
    // special functions. Some of these special controls may not be present on your hardware.
    // You don't have to provide them.
    UI::SpecialControlIds specialControlIds;
    specialControlIds.okBttnId
        = bttnEncoder; // Encoder button is our okay button
    specialControlIds.menuEncoderId
        = encoderMain; // Encoder is used as the main menu navigation encoder
	// There are more special controls which we don't have.
	// specialControlIds.valuePotId = ...
	// specialControlIds.funcBttnId = ...

    // The UI can handle multiple canvases. Each canvas could be a text display, some LEDs,
    // a graphics display or any combination. Each canvas is painted at its own refresh rate.
    // Here we prepare CanvasDescriptors that tell our UI that we have two canvases - a display and an LED.
    // The ClearCanvas() and FlushCanvas() functions will be called before and after the painting
    // respectively.

	// This is the canvas for the OLED display.
    UiCanvasDescriptor oledDisplayDescriptor;
    oledDisplayDescriptor.id_     = canvasOledDisplay; // the unique ID
    oledDisplayDescriptor.handle_ = &hw.display; // a pointer to the display
    oledDisplayDescriptor.updateRateMs_  = 50;   // 50ms == 20Hz
    oledDisplayDescriptor.clearFunction_ = &ClearCanvas;
    oledDisplayDescriptor.flushFunction_ = &FlushCanvas;

	// This second canvas is used to update the User LED on the seed.
	// It's a bit overkill to create a second canvas for this 
	// single LED, but let's just imagine we had more LEDs
    // and wanted to update them faster than the main OLED display.
    // Turning them into a separate canvas allows us to have them
    // updated at another update rate. Also, pages on the UI can choose
    // to be transparent on a particular canvas while being opaque on 
    // another one. 
    UiCanvasDescriptor ledDisplayDescriptor;
    ledDisplayDescriptor.id_            = canvasLed; // the unique ID
    ledDisplayDescriptor.handle_        = nullptr;   // no pointer needed for the LED
    ledDisplayDescriptor.updateRateMs_  = 20;        // 20ms == 50Hz
    ledDisplayDescriptor.clearFunction_ = &ClearCanvas;
    ledDisplayDescriptor.flushFunction_ = &FlushCanvas;

    ui.Init(
        // The event queue to read user input from
        eventQueue,
        // the special control configuration we prepared earlier
        specialControlIds,
        // the canvases we want to use
        {oledDisplayDescriptor, ledDisplayDescriptor},
        // the canvas ID of our primary OLED display. This is where the menus 
		// are drawn to by default.
        canvasOledDisplay);
}

void InitUiPages()
{
    // Here we initialize our menu pages and given them their items.

    // ====================================================================
    // The main menu
    // ====================================================================

    mainMenuItems[0].type = daisy::AbstractMenu::ItemType::openUiPageItem;
    mainMenuItems[0].text = "Values";
    mainMenuItems[0].asOpenUiPageItem.pageToOpen = &valueEditMenu;

    mainMenuItems[1].type = daisy::AbstractMenu::ItemType::openUiPageItem;
    mainMenuItems[1].text = "Controls";
    mainMenuItems[1].asOpenUiPageItem.pageToOpen = &customPage;

    mainMenuItems[2].type = daisy::AbstractMenu::ItemType::openUiPageItem;
    mainMenuItems[2].text = "Popup!";
    mainMenuItems[2].asOpenUiPageItem.pageToOpen = &popup;

    mainMenu.Init(
        // These are the items to show in this menu
        mainMenuItems,
        // the number of items
        kNumMainMenuItems);

    // ====================================================================
    // The "value edit" menu
    // ====================================================================

    valueEditMenuItems[0].type = daisy::AbstractMenu::ItemType::valueItem;
    valueEditMenuItems[0].text = "Float Lin";
    valueEditMenuItems[0].asMappedValueItem.valueToModify = &floatValueLin;

    valueEditMenuItems[1].type = daisy::AbstractMenu::ItemType::valueItem;
    valueEditMenuItems[1].text = "Float Log";
    valueEditMenuItems[1].asMappedValueItem.valueToModify = &floatValueLog;

    valueEditMenuItems[2].type = daisy::AbstractMenu::ItemType::valueItem;
    valueEditMenuItems[2].text = "Int";
    valueEditMenuItems[2].asMappedValueItem.valueToModify = &intValue;

    valueEditMenuItems[3].type = daisy::AbstractMenu::ItemType::valueItem;
    valueEditMenuItems[3].text = "Enum";
    valueEditMenuItems[3].asMappedValueItem.valueToModify = &stringListValue;

    valueEditMenuItems[4].type = daisy::AbstractMenu::ItemType::checkboxItem;
    valueEditMenuItems[4].text = "Bool";
    valueEditMenuItems[4].asCheckboxItem.valueToModify = &boolValue;

    // We don't have a physical "back/cancel" button, so we can use a "closeMenuItem"
    // to close the menu with the "okay" button (= encoder button in our case)
    valueEditMenuItems[5].type = daisy::AbstractMenu::ItemType::closeMenuItem;
    valueEditMenuItems[5].text = "Back";

    valueEditMenu.Init(
        // These are the items to show in this menu
        valueEditMenuItems,
        // the number of items
        kNumValueEditMenuItems);
}

void GenerateUiEvents()
{
    // The DaisyPatch class already handles reading the
    // encoder and the encoder button.
    // Here, we read the results and push events into the UiEventQueue
    // so that our UI can process them in the main loop.

    // If we had a custom hardware we could use the ButtonMonitor and
    // PotMonitor classes to process the physical controls and generate
    // the events.

    // We're being a little lazy here by omitting some of the events we
    // could generate, e.g.
    // - Button retriggering when held down
    // - Tracking double clicks
    // - Tracking "activity-changed" events (user starts or stops using a control)
    // The ButtonMonitor and PotMonitor clases would generate these as well.

    if(hw.encoder.RisingEdge())
        eventQueue.AddButtonPressed(
            // the ID of the button
            bttnEncoder,
            // the number of successive button presses.
            // If we were interested in double clicks /
            // triple clicks etc. we could track the time between
            // presses and pass a real value here.
            1);

    if(hw.encoder.FallingEdge())
        eventQueue.AddButtonReleased(bttnEncoder);

    const auto increments = hw.encoder.Increment();
    if(increments != 0)
        eventQueue.AddEncoderTurned(
            // the ID of the encoder
            encoderMain,
            // the number of increments that the encoder was turned
            increments,
            // the number of increments per revolution
            12);
}

void AudioCallback(float** in, float** out, size_t size)
{
    hw.ProcessAllControls();
    GenerateUiEvents();

    for(size_t i = 0; i < size; i++)
    {
        out[0][i] = in[0][i];
        out[1][i] = in[1][i];
        out[2][i] = in[2][i];
        out[3][i] = in[3][i];
    }
}

int main(void)
{
    hw.Init();
    hw.StartAdc();

    // open the UI
    InitUi();
    InitUiPages();

    // Now that the UI is initialised, we can open our first page.
    // Without any page, the UI would still update the canvases, but they would
    // be empty.
    ui.OpenPage(mainMenu);

    hw.StartAudio(AudioCallback);

    // This is the main loop - what happens here will be executed with
    // whatever CPU time is left after the AudioCallback() is done.
    while(1)
    {
        // here we let the UI update itself. This will read any new user input
        // that's in the UiEventQueue and dispatch it to the pages, top to bottom.
        // If any of the canvases need to be redrawn, this will also be triggered.
        ui.Process();
    }
}
