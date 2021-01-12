# BLE-Integration
Project aims to add low-energy wireless proximity detection onto a MCU. By dropping the device, the board 
advertises a bluetooth signal, which can be picked up by a cellphone. The fall detection algorithm allows 
the board to detect the fall. This is done through two steps: 

1. Adding Bluetooth proximity detection - 

First, in the ble_setup() function, I aim to integrate the bluetooth functionality as specified in the datasheet
for the MCU. Secondly, I then implement the ble_loop() function. What this aims to do is continue to send out a 
beacon or signal until the device has been found or detected with a smartphone. The bonding information stored on
the device allows it to send out the beacon. 

In order to test the broadcasting, I downloaded an application on my phone that can detect surrounding bluetooth 
signals. 

2. Integrate power management 

I previously worked on a project that involved reducing power consumption through reducing clock speed and placing
the board to sleep or low power mode when not in use. 

