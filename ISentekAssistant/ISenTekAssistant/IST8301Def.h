#ifndef IST8301DEF_H
#define IST8301DEF_H

#define I2C_BUFSIZE 512

#define IST8301_REGLENG _T("1")

//Register Address
#define IST8301_STB					0x0C	//Self-Test response
#define IST8301_INFO				0x0D	//More Info
#define IST8301_WIA					0x0F	//Who I am
#define IST8301_DATAX				0x10	//Output Value x
#define IST8301_DATAY				0x12	//Output Value y
#define IST8301_DATAZ				0x14	//Output Value z
#define IST8301_INS					0x16	//Interrupt source register & FIFO info
#define IST8301_STAT				0x18	//Status register
#define IST8301_INL					0x1A	//Interrupt latch release register
#define IST8301_CNTRL1				0x1B	//Control setting register 1
#define IST8301_CNTRL2				0x1C	//Control setting register 2
#define IST8301_CNTRL3				0x1D	//Control setting register 3
#define IST8301_INC					0x1E	//Interrupt & FIFO control
#define IST8301_OFFX				0x20	//Offset drift x value
#define IST8301_OFFY				0x22	//Offset drift y value
#define IST8301_OFFZ				0x24	//Offset drift z value
#define IST8301_ITHR1				0x26	//Interrupt threshold
#define IST8301_DBX					0x2A	//Debug entry x
#define IST8301_DBY					0x2C	//Debug entry y
#define IST8301_DBZ					0x2E	//Debug entry z
#define IST8301_PRET				0x30	//Preset time
#define IST8301_TEMP				0x31	//Temperature value
#define IST8301_DIFF_FLAG			0x32	//DIFF_flag
#define IST8301_ANALOG_TEST			0x40	//ABCD input registers for calibration circuit and analog test and resistor tuning
#define IST8301_UC_TMP5				0x42	//uC temporary storage register 5
#define IST8301_UC_TMP6				0x44	//uC temporary storage register 6
#define IST8301_SENSOR_EN			0x46	//Sensor enable register
#define IST8301_SENSOR_CTRL			0x48	//Sensor control register
#define IST8301_ADC_CTRL			0x4A	//ADC control register
#define IST8301_MEA_COUNT			0x4C	//Measurement count
#define IST8301_CHIP_CTRL1			0x4E	//Chip control register 1
#define IST8301_S2P_RDY				0x50	//Ready register of serial to parallel conversion circuit for ADC output
#define IST8301_ADC_OUT				0x52	//ADC output
#define IST8301_UC_REG				0x54	//uC proprietary register
#define IST8301_ANALOG_TESTMODE		0x56	//Analog test mode selection register
#define IST8301_UC_TMP0				0x58	//uC temporary storage register 0
#define IST8301_UC_TMP1				0x5A	//uC temporary storage register 1
#define IST8301_UC_TMP2				0x5C	//uC temporary storage register 2
#define IST8301_UC_TMP3				0x5E	//uC temporary storage register 3
#define IST8301_UC_TMP4				0x60	//uC temporary storage register 4
#define IST8301_UC_PC_IN			0x62	//uC program counter input
#define IST8301_USER_OD_INTEVAL		0x64	//User defined output data interval
#define IST8301_CH_NEG				0x66	
#define IST8301_CH_OUT_X			0x68	//X sensor channel output
#define IST8301_CHX_GAIN			0x6A	//[5:0] x_gain
#define IST8301_CHX_OFFSET			0x6C	//[8:0] x_offset
#define IST8301_CH_OUT_Y			0x6E	//Y sensor channel output
#define IST8301_CHY_GAIN			0x70	//[5:0] y_gain
#define IST8301_CHY_OFFSET			0x72	//[8:0] y_offset
#define IST8301_CH_OUT_Z			0x74	//Z sensor channel output
#define IST8301_CHZ_GAIN			0x76	//[5:0] z_gain
#define IST8301_CHZ_OFFSET			0x78	//[8:0] z_offset
#define IST8301_40MHZ_CLK			0x7A	//Normal 40Mhz clock tuning value
#define IST8301_32KHZ_CLK			0x7C	//Suspend 32KHz clock tuning value
#define IST8301_UC_FIFO_RD_VAL		0x7E	//FIFO read out value when UC requests, hardware will automatically select one of the 3-axis result according to the sensor enable setting. I2C can not read this register.


#define IST8301_STR_PRET				_T("30")
//#define IST8301_STR_TEMP				_T("31")
//#define IST8301_STR_DIFF_FLAG			_T("32")
#define IST8301_STR_SENSOR_EN			_T("46")	//Sensor enable register
#define IST8301_STR_ADC_CTRL			_T("4A")	//ADC control register
#define IST8301_STR_ADC_OUT				_T("52")	//ADC output

//0x16
#define IST8301_INS_FFU			1<<15
#define IST8301_INS_WML			1<<14
#define IST8301_INS_FEM			1<<13
//#define FP			

//0x18
#define IST8301_STAT_DRDY		1<<6

//0x1B
#define IST8301_CNTRL1_PC1		1<<7
#define IST8301_CNTRL1_ODR		3<<3
#define IST8301_CNTRL1_FS1		1<<1
//0x1C
#define IST8301_CNTRL2_FF2		1<<0
#define IST8301_CNTRL2_FF1		1<<1
#define IST8301_CNTRL2_DREN		1<<3
#define IST8301_CNTRL2_DFEN 	1<<7


#endif // IST8301DEF_H