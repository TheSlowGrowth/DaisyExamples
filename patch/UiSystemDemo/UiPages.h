#pragma once
#include "daisy.h"
#include "UiHardware.h"
#include "util/oled_fonts.h"

class PotentiometerValuePopupPage : public daisy::UiPage
{
  public:
    virtual ~PotentiometerValuePopupPage() {}

    // clang-format off
    bool OnOkayButton(uint8_t numberOfPresses) override { return false; }
    bool OnCancelButton(uint8_t numberOfPresses) override { return false; }
    bool OnArrowButton(daisy::ArrowButtonType arrowType,
                       uint8_t                numberOfPresses) override { return false; }
    bool OnFunctionButton(uint8_t numberOfPresses) override { return false; }
    bool OnButton(uint16_t buttonID, uint8_t numberOfPresses) override { return false; }
    bool OnEncoderTurned(uint16_t encoderID,
                         int16_t  turns,
                         uint16_t stepsPerRevolution) override { return false; }
    bool OnEncoderActivityChanged(uint16_t potID,
                                  bool     isCurrentlyActive) override { return false; }
    // clang-format on

    bool OnPotMoved(uint16_t potID, float newPosition) override
    {
        SetCurrentValue(potID, newPosition);
        return true;
    }

    void Draw(const daisy::UiCanvasDescriptor& canvasDescriptor) override
    {
        // first check if we should close this page
        const auto now = daisy::System::GetNow();
        if(now - lastTimePotChangedInMs_ > timeoutInMs_)
        {
            Close();
            return;
        }

        if(canvasDescriptor.id_ == canvasOledDisplay)
        {
            OledDisplayType& display
                = *((OledDisplayType*)(canvasDescriptor.handle_));

            const auto& fontToUse        = Font_7x10;
            const auto  xPadding         = 3;
            const auto  yPadding         = 3;
            const auto  maxNumCharacters = 6 /* "PotA: " */;
            const auto  maxBarWidth      = 64;

            const auto popupWidth = maxNumCharacters * fontToUse.FontWidth
                                    + maxBarWidth + 2 * xPadding;
            const auto popupHeight = fontToUse.FontHeight + 2 * yPadding;
            const auto x           = (display.Width() - popupWidth) / 2;
            const auto y           = (display.Height() - popupHeight) / 2;
            const auto barLeft
                = x + xPadding + maxNumCharacters * fontToUse.FontWidth;
            const auto barRight
                = barLeft + int(float(maxBarWidth) * currentValueShown_);
            const auto barTop    = y + yPadding;
            const auto barBottom = y + popupHeight - yPadding;

            display.DrawRect(
                x, y, x + popupWidth, y + popupHeight, true, false);
            display.SetCursor(x + xPadding, y + yPadding);
            switch(currentPotShownId_)
            {
                case 0:
                    display.WriteString("PotA: ", fontToUse, true);
                    display.DrawRect(
                        barLeft, barTop, barRight, barBottom, true, true);
                    break;
                case 1:
                    display.WriteString("PotB: ", fontToUse, true);
                    display.DrawRect(
                        barLeft, barTop, barRight, barBottom, true, true);
                    break;
                default: display.WriteString("???", fontToUse, true); break;
            }
        }
    }

    void SetCurrentValue(uint32_t potIdTouched, float currentPotPosition)
    {
        currentPotShownId_      = potIdTouched;
        currentValueShown_      = currentPotPosition;
        lastTimePotChangedInMs_ = daisy::System::GetNow();
    }

  private:
    uint16_t                  currentPotShownId_;
    float                     currentValueShown_;
    static constexpr uint32_t timeoutInMs_ = 2000;
    uint32_t                  lastTimePotChangedInMs_;
};

PotentiometerValuePopupPage potValuePopup;

class PotentiometerPage : public daisy::UiPage
{
  public:
    virtual ~PotentiometerPage() {}

    bool OnCancelButton(uint8_t numberOfPresses) override
    {
        Close();
        return true;
    }

    bool OnPotMoved(uint16_t potID, float newPosition) override
    {
        if(!potValuePopup.IsActive())
        {
            potValuePopup.SetCurrentValue(potID, newPosition);
            if(const auto parentUI = GetParentUI())
                parentUI->OpenPage(potValuePopup);
        }
        return true;
    }

    void Draw(const daisy::UiCanvasDescriptor& canvasDescriptor) override
    {
        if(canvasDescriptor.id_ == canvasOledDisplay)
        {
            OledDisplayType& display
                = *((OledDisplayType*)(canvasDescriptor.handle_));

            display.SetCursor(2, 2);
            display.WriteString("Rotate potentiometers", Font_11x18, true);
            display.SetCursor(2, 22);
            display.WriteString("to see the popup", Font_11x18, true);

            display.SetCursor(2, 52);
            display.WriteString("                  back", Font_7x10, true);
        }
    }
};

PotentiometerPage potentiometerPage;

class ToggleButtonPage : public daisy::UiPage
{
  public:
    virtual ~ToggleButtonPage() {}

    void Reset()
    {
        for(int i = 0; i < numToggles; i++)
            toggleStates_[i] = false;
    }

    bool OnOkayButton(uint8_t numberOfPresses) override
    {
        if(numberOfPresses < 1)
            return true;

        if(currentSelection_ < numToggles)
            toggleStates_[currentSelection_]
                = !toggleStates_[currentSelection_];

        return true;
    }

    bool OnCancelButton(uint8_t numberOfPresses) override
    {
        Close();
        return true;
    }

    bool OnArrowButton(daisy::ArrowButtonType arrowType,
                       uint8_t                numberOfPresses) override
    {
        if(numberOfPresses < 1)
            return true;

        if((arrowType == daisy::ArrowButtonType::left)
           && (currentSelection_ > 0))
            currentSelection_--;
        else if((arrowType == daisy::ArrowButtonType::right)
                && (currentSelection_ < numToggles - 1))
            currentSelection_++;
        return true;
    }

    void Draw(const daisy::UiCanvasDescriptor& canvasDescriptor) override
    {
        if(canvasDescriptor.id_ == canvasOledDisplay)
        {
            OledDisplayType& display
                = *((OledDisplayType*)(canvasDescriptor.handle_));

            display.SetCursor(2, 2);
            display.WriteString("Select Toggle < >", Font_7x10, true);

            display.SetCursor(2, 24);
            char toggleStr[] = "Option A";
            toggleStr[7] += currentSelection_;
            display.WriteString(toggleStr, Font_11x18, true);

            const auto toggleWidth  = 12;
            const auto toggleHeight = 12;
            drawToggleSwitchBox(display,
                                64 - 2 - toggleWidth,
                                27,
                                toggleWidth,
                                toggleHeight,
                                toggleStates_[currentSelection_]);

            display.SetCursor(2, 52);
            display.WriteString("<   >   ok", Font_7x10, true);
            display.WriteString("<   >  toggle back", Font_7x10, true);
        }
    }

  private:
    void drawToggleSwitchBox(OledDisplayType& display,
                             int              x,
                             int              y,
                             int              width,
                             int              height,
                             bool             shouldBeEnabled)
    {
        // draw outline
        display.DrawRect(x, y, x + width, y + height, true, false);

        // draw selection
        static constexpr int padding = 2;
        if(shouldBeEnabled)
            display.DrawRect(x + padding,
                             y + padding,
                             x + width - 2 * padding,
                             y + height - 2 * padding,
                             true,
                             true);
    }

    static constexpr uint8_t numToggles = 4;
    bool                     toggleStates_[numToggles];
    uint8_t                  currentSelection_;
};

ToggleButtonPage toggleButtonPage;

class UiBasePage : public daisy::UiPage
{
  public:
    virtual ~UiBasePage() {}

    bool OnOkayButton(uint8_t numberOfPresses) override
    {
        if(numberOfPresses < 1)
            return true;

        if(auto ui = GetParentUI())
        {
            switch(currentSelection_)
            {
                case toggleSwitchMenu: ui->OpenPage(toggleButtonPage); break;
                case potentiometerView: ui->OpenPage(potentiometerPage); break;
                default: break;
            }
        }

        return true;
    }

    bool OnArrowButton(daisy::ArrowButtonType arrowType,
                       uint8_t                numberOfPresses) override
    {
        if(numberOfPresses < 1)
            return true;

        if((arrowType == daisy::ArrowButtonType::left)
           && (currentSelection_ > 0))
            currentSelection_--;
        else if((arrowType == daisy::ArrowButtonType::right)
                && (currentSelection_ < NUM_SELECTIONS - 1))
            currentSelection_++;
        return true;
    }

    void Draw(const daisy::UiCanvasDescriptor& canvasDescriptor) override
    {
        if(canvasDescriptor.id_ == canvasOledDisplay)
        {
            OledDisplayType& display
                = *((OledDisplayType*)(canvasDescriptor.handle_));

            display.SetCursor(2, 2);
            display.WriteString("Select Option < >", Font_7x10, true);

            display.SetCursor(2, 24);
            switch(currentSelection_)
            {
                case toggleSwitchMenu:
                    display.WriteString("Toggle Switches", Font_7x10, true);
                    break;
                case potentiometerView:
                    display.WriteString("Potentiometers", Font_7x10, true);
                    break;
                default: display.WriteString("???", Font_7x10, true); break;
            }

            display.SetCursor(2, 52);
            display.WriteString("<   >   ok", Font_7x10, true);
        }
    }

  private:
    enum Selection
    {
        toggleSwitchMenu = 0,
        potentiometerView,
        NUM_SELECTIONS
    };
    uint8_t currentSelection_;
};

UiBasePage uiBasePage;