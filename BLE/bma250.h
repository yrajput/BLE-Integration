/*
 * Header file for BMA250 peripheral
 */

//configuring and enabling bma250 peripheral
void bma250_init();

//reading acceleration from bma250
void bma250_read_xyz(int16_t* x, int16_t* y, int16_t* z); 

