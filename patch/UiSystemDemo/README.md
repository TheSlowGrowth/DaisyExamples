# UiSystemDemo

Work in Progress ...
A full tutorial for the UI system will be written later.
I hope this outlines the core concepts.

## Author

Johannes Elliesen aka TheSlowGrowth

## Description

This example shows how to use the Ui system on a Daisy Patch.
Use the encoder to navigate through the menus.

```
┌───────────┐
│           │
│ Main Menu │──────────────────────────────────────────────────┐
│           │                                                  | 
└──┬────┬───┘                                                  | 
   │    │                                                      | 
   │    └────────────────────────────┐                         |
   │                                 │                         |
   ▼                                 ▼                         ▼
┌─────────────────┐         ┌────────────────┐         ┌────────────────┐
│                 │         │                │         │                │
│ Value Edit Menu │         │ Custom UI Page │         │ Popup Info Box │
│                 │         │                │         │                │
└─────────────────┘         └────────────────┘         └────────────────┘

```

The Main Menu and Value Edit Menu are both based on the `FullScreenItemMenu` class.
The example shows how to add various types of items to these menus.

The Custom UI Page and the Popup Info Box show how to inherit from `UiPage` to
design a completely custom UI Page.

In this example, the OLED display is used as the main canvas in the UI system. The user 
LED on the Seed is used as a secondary canvas with a higher refresh rate.