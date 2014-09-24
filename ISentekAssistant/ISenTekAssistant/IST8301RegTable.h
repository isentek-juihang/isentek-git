
#include "IST8301Def.h"

typedef struct _tagREGISTERDATA
{
	unsigned long uRegister;
	unsigned int bytes;
	unsigned char *uValue;
}	REGISTERDATA;

REGISTERDATA IST8301_RegisterTable[]=
{
	{IST8301_STB,					1, NULL},
	{IST8301_INFO,					2, NULL},
	{IST8301_WIA,					1, NULL},
	{IST8301_DATAX,					2, NULL},		//Output Value x
	{IST8301_DATAY,					2, NULL},		//Output Value y
	{IST8301_DATAZ,					2, NULL},		//Output Value z
	{IST8301_INS,					2, NULL},		//Interrupt source register & FIFO info
	{IST8301_STAT,					1, NULL},		//Status register
	{IST8301_INL,					1, NULL},		//Interrupt latch release register
	{IST8301_CNTRL1,				1, NULL},		//Control setting register 1
	{IST8301_CNTRL2,				1, NULL},		//Control setting register 2
	{IST8301_CNTRL3,				1, NULL},		//Control setting register 3
	{IST8301_INC,					2, NULL},		//Interrupt & FIFO control
	{IST8301_OFFX,					2, NULL},		//Offset drift x value
	{IST8301_OFFY,					2, NULL},		//Offset drift y value
	{IST8301_OFFZ,					2, NULL},		//Offset drift z value
	{IST8301_ITHR1,					2, NULL},		//Interrupt threshold
	{IST8301_DBX,					2, NULL},		//Debug entry x
	{IST8301_DBY,					2, NULL},		//Debug entry y
	{IST8301_DBZ,					2, NULL},		//Debug entry z
	{IST8301_PRET,					1, NULL},		//Preset time
	{IST8301_TEMP,					1, NULL},		//Temperature value
	{IST8301_DIFF_FLAG,				1, NULL},		//DIFF_flag
	{IST8301_ANALOG_TEST,			2, NULL},		//ABCD input registers for calibration circuit and analog test and resistor tuning
	{IST8301_UC_TMP5,				2, NULL},		//uC temporary storage register 5
	{IST8301_UC_TMP6,				2, NULL},		//uC temporary storage register 6
	{IST8301_SENSOR_EN,				1, NULL},		//Sensor enable register
	{IST8301_SENSOR_CTRL,			1, NULL},		//Sensor control register
	{IST8301_ADC_CTRL,				1, NULL},		//ADC control register
	{IST8301_MEA_COUNT,				2, NULL},		//Measurement count
	{IST8301_CHIP_CTRL1,			2, NULL},		//Chip control register 1
	{IST8301_S2P_RDY,				1, NULL},		//Ready register of serial to parallel conversion circuit for ADC output
	{IST8301_ADC_OUT,				2, NULL},		//ADC output
	{IST8301_UC_REG,				1, NULL},		//uC proprietary register
	{IST8301_ANALOG_TESTMODE,		2, NULL}, 		//Analog test mode selection register
	{IST8301_UC_TMP0,				2, NULL},		//uC temporary storage register 0
	{IST8301_UC_TMP1,				2, NULL},		//uC temporary storage register 1
	{IST8301_UC_TMP2,				2, NULL},		//uC temporary storage register 2
	{IST8301_UC_TMP3,				2, NULL},		//uC temporary storage register 3
	{IST8301_UC_TMP4,				2, NULL},		//uC temporary storage register 4
	{IST8301_UC_PC_IN,				1, NULL},		//uC program counter input
	{IST8301_USER_OD_INTEVAL,		1, NULL},		//User defined output data interval
	{IST8301_CH_NEG,				2, NULL},		
	{IST8301_CH_OUT_X,				2, NULL},		//X sensor channel output
	{IST8301_CHX_GAIN,				1, NULL},		//[5:0] x_gain
	{IST8301_CHX_OFFSET,			2, NULL},		//[8:0] x_offset
	{IST8301_CH_OUT_Y,				2, NULL},		//Y sensor channel output
	{IST8301_CHY_GAIN,				1, NULL},		//[5:0] y_gain
	{IST8301_CHY_OFFSET,			2, NULL},		//[8:0] y_offset
	{IST8301_CH_OUT_Z,				2, NULL},		//Z sensor channel output
	{IST8301_CHZ_GAIN,				1, NULL},		//[5:0] z_gain
	{IST8301_CHZ_OFFSET,			2, NULL},		//[8:0] z_offset
	{IST8301_40MHZ_CLK,				1, NULL},		//Normal 40Mhz clock tuning value
	{IST8301_32KHZ_CLK,				1, NULL},		//Suspend 32KHz clock tuning value
	{IST8301_UC_FIFO_RD_VAL,		2, NULL}	
};