#pragma once

#include "daisy_patch.h"
#include "UiHardware.h"

extern daisy::DaisyPatch hw;

/** A custom page that displays the state of the control knobs. */
class CustomPage : public daisy::UiPage
{
  public:
    virtual ~CustomPage() {}

    bool OnOkayButton(uint8_t numberOfPresses, bool isRetriggering) override
    {
        if(numberOfPresses == 0) // ignore button release events
            return true;

        // close the page on a button down event and return to the page below
        Close();
        // We return true to indicate that we "handled" the event.
        // If we'd return false, the event would be passed to the page below.
        return true;
    }

    void Draw(const daisy::UiCanvasDescriptor& canvasDescriptor) override
    {
        // First, find out which canvas is supposed to be redrawn here.
        if(canvasDescriptor.id_ == canvasOledDisplay)
        {
            // Now we know that this canvas is our OLED display.
            // We can cast the handle to the correct type and start
            // drawing on it.
            OledDisplayType* display
                = (OledDisplayType*)canvasDescriptor.handle_;

            // get the full screen size
            auto remainingBounds = display->GetBounds();

            // remove a slice from the top to draw a text there
            const auto textRect = remainingBounds.RemoveFromTop(12);
            display->WriteStringAligned("Controls",
                                        Font_7x10,
                                        textRect,
                                        daisy::Alignment::centered,
                                        true);

            // split the remaining screen in 4 equal parts to draw
            // some bargraphs on
            const auto barWidth = remainingBounds.GetWidth() / 4;
            for(int i = 0; i < 4; i++)
            {
                const auto barRect
                    = remainingBounds.RemoveFromLeft(barWidth).Reduced(4, 0);
                const float controlValue = hw.controls[i].Value();
                const auto  barHeightInPx
                    = int(roundf(controlValue * barRect.GetHeight()));
                const auto pxToTrim = barRect.GetHeight() - barHeightInPx;
                display->DrawRect(barRect.WithTrimmedTop(pxToTrim), true, true);
            }
        }
        else if(canvasDescriptor.id_ == canvasLed)
        {
            // Now we know that this canvas is our LED display.

            // let's just turn the LED on when the first control is larger than 0.5f.
            hw.seed.SetLed(hw.controls[0].Value() > 0.5f);
        }
    }
};

/** A custom page that displays a small popup info box
 *  and is totally transparent to user input. */
class PopupInfoBox : public daisy::UiPage
{
  public:
    virtual ~PopupInfoBox() {}

    bool IsOpaque(const daisy::UiCanvasDescriptor& canvasDescriptor) override
    {
        // By returning false in this function, we can make this page transparent
        // so that the page below will be drawn first.
        return false;
    }

    // We return false in all of these to indicate that we didn't handle these
    // input events and would like them to be passed on to the page below.
    // This makes our page transparent to user input.
    bool OnOkayButton(uint8_t, bool) override { return false; }
    bool OnCancelButton(uint8_t, bool) override { return false; }
    bool OnArrowButton(daisy::ArrowButtonType, uint8_t, bool) override
    {
        return false;
    }
    bool OnFunctionButton(uint8_t, bool) override { return false; }
    bool OnButton(uint16_t, uint8_t, bool) override { return false; }
    bool OnMenuEncoderTurned(int16_t, uint16_t) override { return false; }
    bool OnValueEncoderTurned(int16_t, uint16_t) override { return false; }
    bool OnEncoderTurned(uint16_t, int16_t, uint16_t) override { return false; }
    bool OnMenuEncoderActivityChanged(bool) override { return false; }
    bool OnValueEncoderActivityChanged(bool) override { return false; }
    bool OnEncoderActivityChanged(uint16_t, bool) override { return false; }
    bool OnValuePotMoved(float) override { return false; }
    bool OnPotMoved(uint16_t, float) override { return false; }
    bool OnValuePotActivityChanged(bool) override { return false; }
    bool OnPotActivityChanged(uint16_t, bool) override { return false; }

    // This will be called when the page is opened. We can remember
    // when that happened so that we can close the popup after some time.
    void OnShow() override { sysTimeWhenOpened_ = daisy::System::GetNow(); }

    void Draw(const daisy::UiCanvasDescriptor& canvasDescriptor) override
    {
        // First, find out which canvas is supposed to be redrawn here.
        if(canvasDescriptor.id_ == canvasOledDisplay)
        {
            // Now we know that this canvas is our OLED display.
            // We can cast the handle to the correct type and start
            // drawing on it.
            OledDisplayType* display
                = (OledDisplayType*)canvasDescriptor.handle_;

            // get the full screen size
            const auto screenBounds = display->GetBounds();

            // centered on the screen
            const auto popupRect = daisy::Rectangle(64, 16).AlignedWithin(
                screenBounds, daisy::Alignment::centered);


            // draw the text
            const auto rectUsedForText
                = display->WriteStringAligned("Yes, Popups!",
                                              Font_7x10,
                                              popupRect,
                                              daisy::Alignment::centered,
                                              true);

            // draw the popup box around the text
            display->DrawRect(rectUsedForText.Reduced(-2), true, false);
        }
        else if(canvasDescriptor.id_ == canvasLed)
        {
            // We want to be transparent on the led canvas.
            // So, nothing to do here.
        }

        // Check if enough time has passed to close this page
        const auto now = daisy::System::GetNow();
        if(now - sysTimeWhenOpened_ > 2000) // 2 sec timeout
            Close();
    }

  private:
    uint32_t sysTimeWhenOpened_;
};
