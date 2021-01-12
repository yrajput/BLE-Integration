/* i2c driver header file
 *
 */

//need to define anything here??

//initializing and configuring i2c driver
void i2c_init();

//perform data transaction between MCU and i2c peripheral
uint8_t i2c_transaction(uint8_t address, uint8_t dir, uint8_t* data, uint8_t len);


