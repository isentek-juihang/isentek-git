#pragma once
#include "SensorBase.h"

class CKxtf9Driver :	public CSensorBase
{
public:
	CKxtf9Driver(void);
	CKxtf9Driver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master=NULL, LAYOUT SensorLayout=CONFIG_LAYOUT_PAT_1, COORDINATE_SYSTEM Coordinate=CONFIG_COORDINATE_ORG);
	virtual ~CKxtf9Driver(void);

	//Kionix kxtf9
	virtual RETURE_TYPES ChipsetInit(int speed=1);
	virtual RETURE_TYPES ReadSensorData(short *x, short *y, short *z);
	virtual RETURE_TYPES ReadSensorData(short *pXYZ);
	int ReadSensorData(char *buf, int bufsize);
	virtual void UnitTransformation(float *pXYZ, int unit=0);

	int ReadChipInfo(char *buf, int bufsize);		

	size_t show_chipinfo_value(struct device *dev, struct device_attribute *attr, char *buf);
	size_t show_sensordata_value(struct device *dev, struct device_attribute *attr, char *buf);

	enum  SA{ SA_0E=0x0E, SA_0F=0x0F };

private:
	/* KXTF9 Internal Command  (Please refer to KXTF9 Specifications) */
	static const unsigned char KXTF9_REG_XOUT_L	= 0x06;	
	static const unsigned char KXTF9_REG_CTRL_REG1	= 0x1B;
	static const unsigned char KXTF9_REG_CTRL_REG2 = 0x1C;	
	static const unsigned char KXTF9_REG_CTRL_REG3 = 0x1D;
	static const unsigned char KXTF9_REG_DATA_CTRL = 0x21;	

	static const unsigned char KXTF9_OPERATION = 0x80;	
	static const unsigned char KXTF9_12BIT = 0x40;	
	static const unsigned char KXTF9_DATA_ODR_400HZ = 0x05;	

	/* conversion of acceleration data to SI units (m/s^2) */
	static const double CONVERT_A;
	//#define CONVERT_A;		(GRAVITY_EARTH / 1024.0f)  //1g = 1024codes
};
