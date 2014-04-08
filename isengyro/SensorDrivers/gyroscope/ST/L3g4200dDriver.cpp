#include "../../../StdAfx.h"
#include "../../include/L3g4200dDriver.h"

#include "../../../MyUtility.h"

static const short LayoutAVIATION[8][3][3] = {
		{ { 0, 1, 0}, { 1, 0, 0}, { 0, 0, -1}},	//CONFIG_LAYOUT_PAT_1
		{ {-1, 0, 0}, { 0, 1, 0}, { 0, 0, -1}},	//CONFIG_LAYOUT_PAT_2
		{ { 0,-1, 0}, {-1, 0, 0}, { 0, 0, -1}},	//CONFIG_LAYOUT_PAT_3
		{ { 1, 0, 0}, { 0,-1, 0}, { 0, 0, -1}},	//CONFIG_LAYOUT_PAT_4
		{ {-1, 0, 0}, { 0,-1, 0}, { 0, 0, 1}},	//CONFIG_LAYOUT_PAT_5
		{ { 0, 1, 0}, {-1, 0, 0}, { 0, 0, 1}},	//CONFIG_LAYOUT_PAT_6
		{ { 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}},	//CONFIG_LAYOUT_PAT_7
		{ { 0,-1, 0}, { 1, 0, 0}, { 0, 0, 1}}	//CONFIG_LAYOUT_PAT_8
};

CL3g4200dDriver::CL3g4200dDriver(void)
{
	CSensorBase::SetSlaveAddr(SA_69);
	CSensorBase::SetLayout(CSensorBase::CONFIG_LAYOUT_PAT_1);
	CSensorBase::SetCoordinateSystem(CSensorBase::CONFIG_COORDINATE_ORG);
	CSensorBase::I2cSetting(NULL);
}

CL3g4200dDriver::CL3g4200dDriver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master, LAYOUT SensorLayout, COORDINATE_SYSTEM Coordinate) 
{
	CSensorBase::SetSlaveAddr(SlaveAddr);
	CSensorBase::SetLayout(SensorLayout);
	CSensorBase::SetCoordinateSystem(Coordinate);
	CSensorBase::I2cSetting(i2c_master);
}

CL3g4200dDriver::~CL3g4200dDriver(void)
{
}


CSensorBase::RETURE_TYPES CL3g4200dDriver::ChipsetInit(int speed)
{	
	RETURE_TYPES rtl;

	rtl = I2cCheck();
	if(rtl <= I2C_EMPTY) 
		return rtl;

	AddBuffer(0, 0x0f);	//ODR=100Hz BW=12.5
	AddBuffer(1, 0x00);
	AddBuffer(2, 0x08);	//Enable DRDY
	AddBuffer(3, 0x30);	//+-2000 dps
	AddBuffer(4, 0x00);	//FIFO disable
	
	if(!pI2cMaster->iSenI2cWrite(m_i2c_slave_addr, (AUTO_INCREMENT | CTRL_REG1), i2cBufer, 5))
		return I2C_FAIL;

	return DATA_READY;
}


CSensorBase::RETURE_TYPES CL3g4200dDriver::ReadSensorData(short *pXYZ)
{
	RETURE_TYPES rtl;
	s16 xyzTmp[3];
	int i, j;
	
	rtl = I2cCheck();
	if(rtl <= I2C_EMPTY) 
		return rtl;
	
	rtl = DataReady();
	if(rtl == I2C_FAIL || rtl == DATA_FALSE)
		return rtl;

	if(!pI2cMaster->iSenI2cRead(m_i2c_slave_addr, (AUTO_INCREMENT | AXISDATA_REG), i2cBufer, 6))
		return I2C_FAIL;

	xyzTmp[0] = (short int)((i2cBufer[1]<<8) | i2cBufer[0]); 	
	xyzTmp[1] = (short int)((i2cBufer[3]<<8) | i2cBufer[2]);
	xyzTmp[2] = (short int)((i2cBufer[5]<<8) | i2cBufer[4]); 

	for(i = 0; i < SENSOR_DATA_SIZE; i++)
		pXYZ[i] = 0;
		
	/*remap coordinate*/
	for(i = 0; i < SENSOR_DATA_SIZE; ++i)
	{
		if(m_Coordinate == CONFIG_COORDINATE_AVIATION) {			
			for(j = 0; j < SENSOR_DATA_SIZE; j++)
				pXYZ[i] += LayoutAVIATION[m_SensorLayout][i][j] * xyzTmp[j];				
		} else if(m_Coordinate == CONFIG_COORDINATE_ANDROID) {
			
		} else {
			pXYZ[i] = xyzTmp[i];
		}
		m_raw.v[i] = pXYZ[i];
	}

	return DATA_READY;
}


CSensorBase::RETURE_TYPES CL3g4200dDriver::ReadSensorData(short *ax, short *ay, short *az)
{
	s16 tmpXYZ[3] = {0};
	RETURE_TYPES rtl;

	rtl = ReadSensorData(tmpXYZ);
	if(rtl == CSensorBase::DATA_READY) {
		*ax = tmpXYZ[0];
		*ay = tmpXYZ[1];
		*az = tmpXYZ[2];
	} else {
		*ax = 0;
		*ay = 0;
		*az = 0;
	} 
	
    return rtl;
}

CSensorBase::RETURE_TYPES CL3g4200dDriver::DataReady()
{
	if(!pI2cMaster->iSenI2cRead(m_i2c_slave_addr, STATUS_REG, i2cBufer, 1))
		return I2C_FAIL;

	return(RETURE_TYPES)((i2cBufer[0]&(1<<3))==(1<<3));
}

void CL3g4200dDriver::UnitTransformation(float *pXYZ, int unit)
{
	switch(unit)
	{
	case CONFIG_GYRO_UNITS_LSB:
		for(int i=0; i<3; i++)
			pXYZ[i] = (float)m_raw.v[i];
		break;

	case CONFIG_GYRO_UNITS_RAD:
		for(int i=0; i<3; i++)
			pXYZ[i] = m_raw.v[i]*(float)(CONVERT_G);
		
		break;
	}	
	
}


