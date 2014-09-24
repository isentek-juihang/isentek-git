#include "../../../StdAfx.h"
#include "../../include/Kxtf9Driver.h"

#include "../../../MyUtility.h"

static const short LayoutAVIATION[8][3][3] = {
		{ { 0,-1, 0}, {-1, 0, 0}, { 0, 0, 1}},	//CONFIG_LAYOUT_PAT_1
		{ { 1, 0, 0}, { 0,-1, 0}, { 0, 0, 1}},	//CONFIG_LAYOUT_PAT_2
		{ { 0, 1, 0}, { 1, 0, 0}, { 0, 0, 1}},	//CONFIG_LAYOUT_PAT_3
		{ {-1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}},	//CONFIG_LAYOUT_PAT_4
		{ { 1, 0, 0}, { 0, 1, 0}, { 0, 0,-1}},	//CONFIG_LAYOUT_PAT_5
		{ { 0,-1, 0}, { 1, 0, 0}, { 0, 0,-1}},	//CONFIG_LAYOUT_PAT_6
		{ {-1, 0, 0}, { 0,-1, 0}, { 0, 0,-1}},	//CONFIG_LAYOUT_PAT_7
		{ { 0, 1, 0}, {-1, 0, 0}, { 0, 0,-1}}	//CONFIG_LAYOUT_PAT_8
};

CKxtf9Driver::CKxtf9Driver(void)
{
	CSensorBase::SetSlaveAddr(SA_0F);
	CSensorBase::SetLayout(CSensorBase::CONFIG_LAYOUT_PAT_1);
	CSensorBase::SetCoordinateSystem(CSensorBase::CONFIG_COORDINATE_ORG);
	CSensorBase::I2cSetting(NULL);
}

CKxtf9Driver::CKxtf9Driver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master, LAYOUT SensorLayout, COORDINATE_SYSTEM Coordinate) 
{
	CSensorBase::SetSlaveAddr(SlaveAddr);
	CSensorBase::SetLayout(SensorLayout);
	CSensorBase::SetCoordinateSystem(Coordinate);
	CSensorBase::I2cSetting(i2c_master);
}

CKxtf9Driver::~CKxtf9Driver(void)
{
}

CSensorBase::RETURE_TYPES CKxtf9Driver::ChipsetInit(int speed)
{
	RETURE_TYPES rtl;

	rtl = I2cCheck();
	if(rtl <= I2C_EMPTY) 
		return rtl;

	AddBuffer(0, KXTF9_OPERATION | KXTF9_12BIT);
	if(!pI2cMaster->iSenI2cWrite(m_i2c_slave_addr, KXTF9_REG_CTRL_REG1, i2cBufer, 1))
        return I2C_FAIL;
    
	AddBuffer(0, KXTF9_DATA_ODR_400HZ);
	if(!pI2cMaster->iSenI2cWrite(m_i2c_slave_addr, KXTF9_REG_DATA_CTRL, i2cBufer, 1))
        return I2C_FAIL;

	return DATA_READY;
}

int CKxtf9Driver::ReadChipInfo(char *buf, int bufsize)
{
    BYTE databuf[10]; 
    
    memset(databuf, 0, sizeof(BYTE )*10);
        
    if ((!buf)||(bufsize<=30))
        return -1;

	if(!pI2cMaster->CheckI2c()) {
		*buf = 0;
		//AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return -2;
	}
    
    sprintf(buf, "KXTF9 Chip");
    return 0;
}



CSensorBase::RETURE_TYPES CKxtf9Driver::ReadSensorData(short *pXYZ)
{
	int rtl = ReadSensorData(&pXYZ[0], &pXYZ[1], &pXYZ[2]);
	if(rtl != CSensorBase::DATA_READY) {
		pXYZ[0] = 0;
		pXYZ[1] = 0;
		pXYZ[2] = 0;
	} 
	return (CSensorBase::RETURE_TYPES)rtl;
}

CSensorBase::RETURE_TYPES CKxtf9Driver::ReadSensorData(short *ax, short *ay, short *az)
{
	BYTE databuf[20];
    UINT x, y, z;
	int xyzData = 0;
	s16 xyzTmp[3];
	s16 tmpXYZ[3]={0};
	int i, j;

    int res = 0;
	
	if(!pI2cMaster->CheckI2c()) {
		//AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return CSensorBase::I2C_EMPTY;
	}

	if(!pI2cMaster->iSenI2cRead(m_i2c_slave_addr, KXTF9_REG_XOUT_L, databuf, 6))
		return CSensorBase::I2C_FAIL;
	
	x = ((int) databuf[1]) << 4 | ((int) databuf[0]) >> 4;
	y = ((int) databuf[3]) << 4 | ((int) databuf[2]) >> 4;
	z = ((int) databuf[5]) << 4 | ((int) databuf[4]) >> 4;
	
	Reg2NumericInt(x, &xyzData, true, 11);
	xyzTmp[0] = xyzData;	
	
	Reg2NumericInt(y, &xyzData, true, 11);
	xyzTmp[1] = xyzData;    
	
	Reg2NumericInt(z, &xyzData, true, 11);
	xyzTmp[2] = xyzData;

	/*remap coordinate*/
	for(i = 0; i < SENSOR_DATA_SIZE; ++i)
	{
		if(m_Coordinate == CONFIG_COORDINATE_AVIATION) {			
			for(j = 0; j < SENSOR_DATA_SIZE; j++)
				tmpXYZ[i] += LayoutAVIATION[m_SensorLayout][i][j] * xyzTmp[j];				
		} else if(m_Coordinate == CONFIG_COORDINATE_ANDROID) {
			
		} else {
			tmpXYZ[i] = xyzTmp[i];
		}
		m_raw.v[i] = tmpXYZ[i];
	}

	*ax =  m_raw.v[0];;
	*ay =  m_raw.v[1];;
 	*az =  m_raw.v[2];;

    return CSensorBase::DATA_READY;
}

void CKxtf9Driver::UnitTransformation(float *pXYZ, int unit)
{
	switch(unit)
	{
	case CONFIG_ACCEL_UNITS_LSB:
		for(int i=0; i<3; i++)
			pXYZ[i] = (float)m_raw.v[i];
		break;

	case CONFIG_ACCEL_UNITS_MPSS:	//metre per second squared
		for(int i=0; i<3; i++)
			pXYZ[i] = (float)m_raw.v[i]*((float)CKxtf9Driver::CONVERT_A);
		break;
	}	
}

const double CKxtf9Driver::CONVERT_A = (GRAVITY_EARTH / 1024.0f);
