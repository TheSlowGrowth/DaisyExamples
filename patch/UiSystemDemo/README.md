# UiSystemDemo

## Author

Johannes Elliesen aka TheSlowGrowth

## Description

This example shows off the UI system. It displays a couple of menu pages on an OLED 
display.

## Hardware connections

| Daisy Pin | GPIO | Connect to     |                                     |
| --------- |:----:| -------------- | ----------------------------------- |
| 1 - D0    | PB12 | OK Button      |                                     |
| 2 - D1    | PC11 | Cancel Button  | Optional, unconnected if unused     |
| 3 - D2    | PC10 | Left Button    |                                     |
| 4 - D3    | PC9  | Right Button   |                                     |
| 5 - D4    | PC8  | Up Button      | Optional, unconnected if unused     |
| 6 - D5    | PD2  | Down Button    | Optional, unconnected if unused     |
| 7 - D6    | PC12 | OLED D/C       |                                     |
| 8 - D7    | PG10 | OLED CS        |                                     |
| 9 - D8    | PG11 | OLED SCK       |                                     |
| 10 - D9   | PB4  | OLED RESET     |                                     |
| 11 - D10  | PB5  | OLED MOSI      |                                     |
| 22 - A0   | PC0  | Value Slider   | Optional, connect to AGND if unused |
| 23 - A1   | PA3  | Pot 1          | Optional, connect to AGND if unused |
| 24 - A2   | PB1  | Pot 2          | Optional, connect to AGND if unused |

All Buttons connect between the corresponding pin and VCC.
You can leave out any of the optional controls. 
