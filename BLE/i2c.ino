/**
 * This file contains methods required to use the i2c peripheral to communicate
 * with other peripherals.
 * Written by Priyal Suneja and Yousuf Rajput
 */

#include <sam.h>
#include "i2c.h"

/*
 * Function Name: i2c_init
 *
 * Purpose: Configure and enable the SERCOM i2c peripheral to 
 *          to communicate in master mode. 
 * 
 * Parameters: none
 *          
 * Return: void
 */

void i2c_init(){

    // setting the GPIO pins to be used for I2C
    PORT->Group[0].PMUX[11].reg = PORT_PMUX_PMUXO_C | PORT_PMUX_PMUXE_C;
    PORT->Group[0].PINCFG[22].reg = PORT_PINCFG_PMUXEN;
    PORT->Group[0].PINCFG[23].reg = PORT_PINCFG_PMUXEN;
    PM->APBCMASK.bit.SERCOM3_ = 1;

    //disable enable bit
    SERCOM3->I2CM.CTRLA.bit.ENABLE = 0;
    while(SERCOM3->I2CM.SYNCBUSY.bit.ENABLE);

    // enable master mode
    SERCOM3->I2CM.CTRLA.bit.MODE = 0x5;

    // enabling interrupts
    SERCOM3->I2CM.INTENSET.bit.MB = 1;
    SERCOM3->I2CM.INTENSET.bit.SB = 1;

    // enabling smart mode
    SERCOM3->I2CM.CTRLB.bit.SMEN = 1;       
    while(SERCOM3->I2CM.SYNCBUSY.bit.SYSOP); 

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_SERCOM3_CORE | GCLK_CLKCTRL_CLKEN; 
    //set BAUD bit rate to 200Khz 
    SERCOM3->I2CM.BAUD.bit.BAUD = 125;   
    SERCOM3->I2CM.BAUD.bit.BAUDLOW = 0;   
    while(SERCOM3->I2CM.SYNCBUSY.bit.SYSOP);


    SERCOM3->I2CM.CTRLA.bit.ENABLE = 1;
    while(SERCOM3->I2CM.SYNCBUSY.bit.ENABLE);

    SERCOM3->I2CM.STATUS.bit.BUSSTATE = 0x1;  // setting bus state to idle
    while(SERCOM3->I2CM.SYNCBUSY.bit.SYSOP);
  
}



/*
 * Function Name: i2c_transaction
 *
 * Purpose: Perform a read/write transaction across the i2c bus
 *	    between master and peripheral(accelerometer)
 * 
 * Parameters: 
 *	address - the address of the data register
 *	dir - 0: writing to slave 1: reading from slave
 *	data - the data we want to read/write from data reg
 *	len - total length (bits) of data we want to read
 *          
 * Return: void
 */
uint8_t i2c_transaction(uint8_t address, uint8_t dir, 
                        uint8_t* data, uint8_t len){

    int dataTransacted = 0;

    // setting the device address to talk to with write mode
     SERCOM3->I2CM.ADDR.bit.ADDR = (0x19 << 1) | 0;
     while(!SERCOM3->I2CM.INTFLAG.bit.MB);   

    // writing the register we want to read/write
     SERCOM3->I2CM.DATA.bit.DATA = address;
     while(!SERCOM3->I2CM.INTFLAG.bit.MB);   

     if(dir == 0) {
         // writing len amount of data
         while(dataTransacted < len) {
             SERCOM3->I2CM.DATA.bit.DATA = data[dataTransacted/8];
             while(!SERCOM3->I2CM.INTFLAG.bit.MB);   
             dataTransacted += 8;
         }
     } else {
        // setting the device address to talk to in read mode
         SERCOM3->I2CM.ADDR.bit.ADDR = (0x19 << 1) | 1;
         
         while(dataTransacted < len) {
            while(!SERCOM3->I2CM.INTFLAG.bit.SB); 
            if( len - dataTransacted == 8) {
                // waiting for device to send data
                 SERCOM3->I2CM.CTRLB.bit.ACKACT = 1;
                 while(SERCOM3->I2CM.SYNCBUSY.bit.SYSOP);   
            }  

            // reading data
             data[0] = SERCOM3->I2CM.DATA.bit.DATA;
             dataTransacted += 8;
         }
     }

    // ending the transaction
     SERCOM3->I2CM.CTRLB.bit.CMD = 0x3;
     while(SERCOM3->I2CM.SYNCBUSY.bit.SYSOP);
  
     return 0;
  
}
