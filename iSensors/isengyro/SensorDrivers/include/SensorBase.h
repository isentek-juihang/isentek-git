#pragma once
#include "../../Interface/I2C/ISenI2cMaster.h"
#include "Sensors.h"

class CSensorBase
{
public:
	//senor layout configuration, choose one of the following configuration
	enum  LAYOUT{	CONFIG_LAYOUT_PAT_1,	// Point at left-upper
					CONFIG_LAYOUT_PAT_2,	// Point at right-upper
					CONFIG_LAYOUT_PAT_3,	// Point at right-lower
					CONFIG_LAYOUT_PAT_4,	// Point at left-lower
					CONFIG_LAYOUT_PAT_5,	// At the reverse side of the CONFIG_LAYOUT_PAT_2 (Point at left-upper)
					CONFIG_LAYOUT_PAT_6,	// At the reverse side of the CONFIG_LAYOUT_PAT_1 (Point at right-upper)
					CONFIG_LAYOUT_PAT_7,	// At the reverse side of the CONFIG_LAYOUT_PAT_4 (Point at right-lower)
					CONFIG_LAYOUT_PAT_8 };	// At the reverse side of the CONFIG_LAYOUT_PAT_3 (Point at left-lower)
	
	enum  COORDINATE_SYSTEM{	CONFIG_COORDINATE_ORG, 
								CONFIG_COORDINATE_AVIATION,
								CONFIG_COORDINATE_ANDROID	 };

	enum  ACCEL_UNITS{	CONFIG_ACCEL_UNITS_LSB, 
						CONFIG_ACCEL_UNITS_MPSS	 };		// conversion of acceleration data to SI units (m/s^2)(metre per second squared)

	enum  MAG_UNITS{	CONFIG_MAG_UNITS_LSB, 
						CONFIG_MAG_UNITS_UT,			// conversion of magnetic data to uT(micro-Tesla) units
						CONFIG_MAG_UNITS_MILLI_GASS	 };

	enum  GYRO_UNITS{	CONFIG_GYRO_UNITS_LSB, 
						CONFIG_GYRO_UNITS_RAD	 };		//rad/s(radian per second)	

	enum RETURE_TYPES{
		I2C_NULL	= -3,	
		I2C_EMPTY	= -2,	
		I2C_FAIL	= -1,		// Represents fail
		DATA_FALSE	= 0,		// Data not readdy
		DATA_READY	= 1		// Represents success
	};	

	static const unsigned int nSensorAxisNum = 3;

	CSensorBase(void);
	CSensorBase(unsigned char SlaveAddr, CISenI2cMaster *i2c_master=NULL, LAYOUT SensorLayout=CONFIG_LAYOUT_PAT_1, COORDINATE_SYSTEM Coordinate=CONFIG_COORDINATE_ORG);
	virtual ~CSensorBase(void);

	static const unsigned int I2C_BUF_SIZE = 256;

	virtual RETURE_TYPES ChipsetInit(int speed=1) = 0;

	virtual RETURE_TYPES ReadSensorData(short *pXYZ) = 0;
	virtual RETURE_TYPES ReadSensorData(short *x, short *y, short *z) = 0;

	virtual void UnitTransformation(float *pXYZ, int unit) = 0;
	
	void SetSlaveAddr(unsigned char i2c_slave);
	unsigned char GetSlaveAddr(void) { return m_i2c_slave_addr;}

	void SetLayout(LAYOUT layout);
	unsigned char GetLayout(void) { return m_SensorLayout;}

	void SetCoordinateSystem(COORDINATE_SYSTEM Coordinate);
	unsigned char GetCoordinateSystem(void) { return m_Coordinate;}

	void I2cSetting(CISenI2cMaster *i2c_master);
	RETURE_TYPES I2cCheck(void);

protected:
	CISenI2cMaster *pI2cMaster;

	BYTE i2cBufer[I2C_BUF_SIZE];

	void AddBuffer(int index, BYTE data);

	unsigned char m_i2c_slave_addr;

	LAYOUT m_SensorLayout;

	COORDINATE_SYSTEM m_Coordinate;

	raw_data m_raw;
};

