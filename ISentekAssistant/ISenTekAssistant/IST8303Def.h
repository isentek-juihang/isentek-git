#ifndef IST8303DEF_H
#define IST8303DEF_H

#define I2C_BUFSIZE 512

#define IST8303_REGLENG _T("1")

//Register Address
#define IST8303_STB					0x0C	//Self-Test response
#define IST8303_INFO				0x0D	//More Info
#define IST8303_WIA					0x00	//Who I am
#define IST8303_DATAX				0x03	//Output Value x
#define IST8303_DATAY				0x05	//Output Value y
#define IST8303_DATAZ				0x07	//Output Value z
#define IST8303_INS					0x16	//Interrupt source register & FIFO info
#define IST8303_STAT				0x02	//Status register
#define IST8303_INL					0x1A	//Interrupt latch release register
#define IST8303_CNTRL1				0x0A	//Control setting register 1
#define IST8303_CNTRL2				0x0B	//Control setting register 2
#define IST8303_CNTRL3				0x0D	//Control setting register 3

#define IST8303_STAT_DRDY		    (1<<0)

#endif // IST8303DEF_H
