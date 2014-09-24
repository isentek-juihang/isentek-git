#pragma once
#include "SensorBase.h"

class CL3g4200dDriver :	public CSensorBase
{
public:
	CL3g4200dDriver(void);
	CL3g4200dDriver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master=NULL, LAYOUT SensorLayout=CONFIG_LAYOUT_PAT_1, COORDINATE_SYSTEM Coordinate=CONFIG_COORDINATE_ORG);
	virtual ~CL3g4200dDriver(void);

	virtual RETURE_TYPES ChipsetInit(int speed=1);	
	
	virtual RETURE_TYPES ReadSensorData(short *x, short *y, short *z);
	virtual RETURE_TYPES ReadSensorData(short *pXYZ);
	virtual void UnitTransformation(float *pXYZ, int unit=0);
	RETURE_TYPES DataReady(void);
	enum  SA{ SA_68=0x68, SA_69=0x69 };

private:
	//Register Address
	#define WHO_AM_I 0x0F
	#define CTRL_REG1 0x20    /* CTRL REG1 */
	#define CTRL_REG2 0x21    /* CTRL REG2 */
	#define CTRL_REG3 0x22    /* CTRL_REG3 */
	#define CTRL_REG4 0x23    /* CTRL_REG4 */
	#define CTRL_REG5 0x24    /* CTRL_REG5 */
	#define REFERENCE 0x25
	#define OUT_TEMP 0x26
	#define STATUS_REG 0x27
	#define OUT_X_L 0x28
	#define OUT_X_H 0x29
	#define OUT_Y_L 0x2A
	#define OUT_Y_H 0x2B
	#define OUT_Z_L 0x2C
	#define OUT_Z_H 0x2D
	#define FIFO_CTRL_REG 0x2E
	#define FIFO_SRC_REG 0x2F
	#define INT1_CFG 0x30
	#define INT1_SRC 0x31
	#define INT1_TSH_XH 0x32
	#define INT1_TSH_XL 0x33
	#define INT1_TSH_YH 0x34
	#define INT1_TSH_YL 0x35
	#define INT1_TSH_ZH 0x36
	#define INT1_TSH_ZL 0x37
	#define INT1_DURATION 0x38
	

	/* CTRL_REG1 */
	#define ALL_ZEROES	0x00
	#define PM_OFF		0x00
	#define PM_NORMAL	0x08
	#define ENABLE_ALL_AXES	0x07
	#define BW00		0x00
	#define BW01		0x10
	#define BW10		0x20
	#define BW11		0x30
	#define ODR100		0x00  /* ODR = 100Hz */
	#define ODR200		0x40  /* ODR = 200Hz */
	#define ODR400		0x80  /* ODR = 400Hz */
	#define ODR800		0xC0  /* ODR = 800Hz */

	/* CTRL_REG4 bits */
	#define	FS_MASK				0x30
	#define	BDU_ENABLE			0x80

	#define	SELFTEST_MASK			0x06
	#define L3G4200D_SELFTEST_DIS		0x00
	#define L3G4200D_SELFTEST_EN_POS	0x02
	#define L3G4200D_SELFTEST_EN_NEG	0x04

	#define AXISDATA_REG			0x28

	#define FUZZ			0
	#define FLAT			0
	#define AUTO_INCREMENT		0x80

	/* RESUME STATE INDICES */
	#define	RES_CTRL_REG1		0
	#define	RES_CTRL_REG2		1
	#define	RES_CTRL_REG3		2
	#define	RES_CTRL_REG4		3
	#define	RES_CTRL_REG5		4
	#define	RESUME_ENTRIES		5

	#define L3G4200D_FS_250_LSB			131	// LSB/(o/s)
	#define L3G4200D_FS_500_LSB			66 
	#define L3G4200D_FS_2000_LSB		16  
	#define L3G4200D_OUT_MAGNIFY		131

	/* conversion of magnetic data to udegree units */
	#define CONVERT_G           0.0012217294

	s16 cali_sw[3+1];
};

