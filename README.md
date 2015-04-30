#labUtilization

## Description: 
Contains code needed to use the 4D Systems touchscreen interface with Galileo to create a device for capturing lab utilization data.

## Requirements: 
Requires ViSi Genie Arduino Library located [here](https://github.com/4dsystems/ViSi-Genie-Arduino-Library)




##To run:

1. Flash program to touch panel
  1. Load 4D systems IDE and open project (labUsageTracker.4DGenie)
  2. Insert SD card from touch panel into PC/adapter
  3. Press Build Copy/Load button
  4. Select drive letter for SD card to copy files, press okay
  5. Unmount sd card, insert into touch panel. Display should show WWID entry screen at this point
2. Flash program to arduino
  1. Install 4D systems arduino shield on Galileo, connect cable from shield to screen
  2. Power on galileo, connect to PC
  3. Load usageTracker.ino and upload to galileo
  4. After a second or two the screen should redraw, at this point it is ready to go