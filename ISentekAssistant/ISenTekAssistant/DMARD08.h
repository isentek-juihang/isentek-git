/*
* Example I2C header code for DMARD01 g-sensor I2C coding
* Date: 2009/09/14
* Version: V1.0
* Company: DMT
*/

#ifndef __DMARD08_H
#define __DMARD08_H

/*
//Need to define the following Macros
#define SET_I2C_CLK_OUTPUT       //set SCL GPIO pin as output
#define SET_I2C_DATA_OUTPUT      //set SDA GPIO pin as output
#define SET_I2C_DATA_INPUT       //set SDA GPIO pin as input
#define SET_I2C_CLK_HIGH         //ouput SCL high
#define SET_I2C_CLK_LOW          //output SCL low
#define SET_I2C_DATA_HIGH        //output SDA high
#define SET_I2C_DATA_LOW         //output SDA low
#define GET_I2C_DATA_BIT         //input SDA
*/

//Slave address for SDO(pin17) connected to GND
#define DMARD01_I2C_SLAVE_WRITE_ADDR 0x38
#define DMARD01_I2C_SLAVE_READ_ADDR  0x39
//Slave address for SDO(pin17) connected to VCC
//#define DMARD01_I2C_SLAVE_WRITE_ADDR 0x3a
//#define DMARD01_I2C_SLAVE_READ_ADDR  0x3b

//Start register address for T, X, Y, Z
#define DMARD01_T_REG_START_ADDRESS 0x00
#define DMARD01_X_REG_START_ADDRESS 0x02
#define DMARD01_Y_REG_START_ADDRESS 0x04
#define DMARD01_Z_REG_START_ADDRESS 0x06

//I2C function
//extern void acc_sensor_init(void);
//extern char acc_sensor_write_bytes(unsigned char reg_addr, unsigned char* reg_data, unsigned char wr_len);
//extern char acc_sensor_read_bytes(unsigned char reg_addr, unsigned char* reg_data, unsigned char r_len);

#endif