#ifndef BLE_H
#define BLE_H

// Initalize the BLE driver
int BLEsetup();

// Run this in the main loop to handle any BLE events
void aci_loop();

uint8_t lib_aci_send_data(uint8_t ignore, uint8_t* sendBuffer, uint8_t sendLength);

int get_connected();

int get_input(char *input);
#endif
