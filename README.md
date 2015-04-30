labUtilization

Contains code needed to use the 4D Systems touchscreen interface with Galileo to create a device for capturing lab utilization data.

Requires ViSi Genie Arduino Library located at https://github.com/4dsystems/ViSi-Genie-Arduino-Library

To run:
(Flash program to touch panel)
Load 4D systems IDE and open project (labUsageTracker.4DGenie)
Insert SD card from touch panel into PC/adapter
Press Build Copy/Load button
Select drive letter for SD card to copy files, press okay
Unmount sd card, insert into touch panel. Display should show WWID entry screen at this point

(Flash program to arduino)
Install 4D systems arduino shield on Galileo, connect cable from shield to screen
Power on galileo, connect to PC
Load usageTracker.ino and upload to galileo

After a second or two the screen should redraw, at this point it is ready to go