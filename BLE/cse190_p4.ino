#include <sam.h>
#include "timer.h"
#include "i2c.h"
#include "bma250.h"


#include <SPI.h>
#include "STBLE.h"
#include "ble.h"

//Debug output adds extra flash and memory requirements!
#ifndef BLE_DEBUG
#define BLE_DEBUG true
#endif

#define PIPE_UART_OVER_BTLE_UART_TX_TX 0

extern "C" int _write(int fd, const void *buf, size_t count) {
  // STDOUT
  //if (fd == 1)
    //SerialUSB.write((char*)buf, count);
  return 0;
}

/* === DO NOT REMOVE: Initialize C library === */
extern "C" void __libc_init_array(void);

volatile bool drop = false;
volatile bool displayThings = false; 
volatile int timeSinceDrop = 0; // counts number of minutes since last drop 
volatile int count = 0;     // counts to tell when a new minute starts


void TC3_Handler() {

   uint8_t getDrop[1] = {0};

   if(count % 5 == 0) {
    i2c_transaction(0x9, 1, getDrop, 8);
   }

   if(getDrop[0] == 1) {
        drop = true;
        timeSinceDrop = 0;
        displayThings = true;
    }
    count = (count + 1) % 10; 
    if(count == 0) {
      timeSinceDrop++;
      displayThings = true;
    }

    timer3_reset();
}

int main(void) {
  /* ==== DO NOT REMOVE: USB configuration ==== */
  init();
  __libc_init_array();
  USBDevice.init();
  USBDevice.attach();
  /* =========================================== */

  /* === Init Drivers === */
    BLEsetup();

  i2c_init();
  timer3_init();
  bma250_init();
  

  while (1) {

    if(drop) {
      aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
      if(get_connected() != 1) {
        continue; 
      }

      char input[21];
      int len = get_input(input);
      if(len > 0) {
        if(strcmp(input, "found") == 0) {
          drop = false;
        }
      }

      if(timeSinceDrop % 10 == 0 && displayThings) {
        char sendBuffer[21];
        uint8_t sendLength = 0;
        char test1[] = "Privtag BGCSW has";
        test1[20] = '\0'; 
        int i;
        
        for( i = 0; i < strlen(test1); i++ ){
          sendBuffer[i] = test1[i];
        }
        for( i; i < strlen(sendBuffer); i++ ) {
          sendBuffer[i] = '\0';
        }
        sendLength = strlen(sendBuffer);
        
        lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)sendBuffer, sendLength);


        char test2[] = " been missing for ";
        test2[20] = '\0'; 
        
        for( i = 0; i < strlen(test2); i++ ){
          sendBuffer[i] = test2[i];
        }
        for( i; i < strlen(sendBuffer); i++ ) {
          sendBuffer[i] = '\0';
        }
        sendLength = strlen(sendBuffer);
        lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)sendBuffer, sendLength);

        char test3[14]; 
        sprintf(test3, "%d seconds", timeSinceDrop);
        test3[13] = '\0'; 

        
        for(i = 0; i < strlen(test3); i++ ){
          sendBuffer[i] = test3[i];
        }

        for( i; i < strlen(sendBuffer); i++ ) {
          sendBuffer[i] = '\0';
        }
        sendLength = strlen(sendBuffer);
        lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)sendBuffer, sendLength);

        displayThings = false;
       
      }
    } else {
      aci_hal_device_standby();
      PM->SLEEP.reg = 0x1;
      __asm volatile ("wfi");
    }
  }
  return 0;
}
