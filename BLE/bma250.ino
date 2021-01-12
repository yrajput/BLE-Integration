/**
 * This file contains methods required to use the BMA250 accelerometer
 * peripheral. 
 * Written by Priyal Suneja and Yousuf Rajput
 */

#include <sam.h>
#include "bma250.h"
#include "i2c.h"

/*
 * Function Name: bma250_init
 * 
 * Purpose: Configure and enable the acceleromter peripheral (BMA250)
 *
 * Parameters: None
 *
 * Return: void
 */
void bma250_init(){

  uint8_t x[1] = {1};
  
  i2c_transaction(0x11, 0, x, 8);   // writing to lowpower_en bit
  i2c_transaction(0x12, 0, x, 8);   // writing to lowpower_mode bit

  uint8_t enableInterrupt[1] = {0x8};
  i2c_transaction(0x17, 0, enableInterrupt, 8);   // writing to lowpower_mode bit

  uint8_t threshold[1] = {0x48};
  i2c_transaction(0x23, 0, threshold, 8);   // editing threshold
  
}



/*
 * Function Name: bma250_read_xyz
 * 
 * Purpose: Read the lsb and msb of acceleration in the three different axis
 * from the bma250 peripheral return the combined value that represents the
 * acceleration of the board in each axis. 
 *
 * Parameters: 
 *	x - acceleration along x axis in two's complement representation
 * 	y - acceleration along y axis in two's complement representation
 * 	z - acceleration along z axis in two's complement representation
 *
 * Return: void
 */
void bma250_read_xyz(int16_t* x, int16_t* y, int16_t* z){

  uint8_t xa[2];
  uint8_t ya[2];
  uint8_t za[2];

  i2c_transaction(0x02, 1, &xa[0], 8); // reading lsb of x axis  
  i2c_transaction(0x03, 1, &xa[1], 8); // reading msb of x axis
  i2c_transaction(0x04, 1, &ya[0], 8); // reading lsb of y axis
  i2c_transaction(0x05, 1, &ya[1], 8); // reading msb of y axis
  i2c_transaction(0x06, 1, &za[0], 8); // reading lsb of z axis
  i2c_transaction(0x07, 1, &za[1], 8); // reading msb of z axis

  // converting from uint8_t to int16_t so that it doesn't overflow while bit
  // shifting
  int16_t xlsb = xa[0];
  int16_t xmsb = xa[1];
  int16_t ylsb = ya[0];
  int16_t ymsb = ya[1];
  int16_t zlsb = za[0];
  int16_t zmsb = za[1];
    
  if( (xmsb >> 7) == 1 ) {   // negative acc to two's complement
    x[0] = ((xmsb << 2) | (xlsb >> 6)) | 0xfc00;  
  } else {
    x[0] = ((xmsb << 2) | (xlsb >> 6)) | 0x0000;
  }

  if( (ymsb >> 7) == 1 ) {   // negative acc to two's complement
    y[0] = ((ymsb << 2) | (ylsb >> 6)) | 0xfc00;  
  } else {
    y[0] = ((ymsb << 2) | (ylsb >> 6)) | 0x0000;
  }

  if( (zmsb >> 7) == 1 ) {   // negative acc to two's complement
    z[0] = ((zmsb << 2) | (zlsb >> 6)) | 0xfc00;  
  } else {
    z[0] = ((zmsb << 2) | (zlsb >> 6)) | 0x0000;
  }
  
}
