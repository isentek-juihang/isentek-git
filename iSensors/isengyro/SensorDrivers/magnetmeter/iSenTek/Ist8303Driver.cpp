#include "../../../StdAfx.h"
#include "../../include/Ist8303Driver.h"

static const short LayoutAVIATION[8][3][3] = {
        { {-1, 0, 0}, { 0, 1, 0}, { 0, 0,-1}},  //CONFIG_LAYOUT_PAT_1
        { { 0,-1, 0}, {-1, 0, 0}, { 0, 0,-1}},  //CONFIG_LAYOUT_PAT_2
        { { 1, 0, 0}, { 0,-1, 0}, { 0, 0,-1}},  //CONFIG_LAYOUT_PAT_3
        { { 0, 1, 0}, { 1, 0, 0}, { 0, 0,-1}},  //CONFIG_LAYOUT_PAT_4
        { { 0,-1, 0}, { 1, 0, 0}, { 0, 0, 1}},  //CONFIG_LAYOUT_PAT_5
        { {-1, 0, 0}, { 0,-1, 0}, { 0, 0, 1}},  //CONFIG_LAYOUT_PAT_6
        { { 0, 1, 0}, {-1, 0, 0}, { 0, 0, 1}},  //CONFIG_LAYOUT_PAT_7
        { { 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}}   //CONFIG_LAYOUT_PAT_8
};

CIst8303Driver::CIst8303Driver(void)
{
    CSensorBase::SetSlaveAddr(SA_0E);
    CSensorBase::SetLayout(CSensorBase::CONFIG_LAYOUT_PAT_1);
    CSensorBase::SetCoordinateSystem(CSensorBase::CONFIG_COORDINATE_ORG);
    CSensorBase::I2cSetting(NULL);
}

CIst8303Driver::CIst8303Driver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master, LAYOUT SensorLayout, COORDINATE_SYSTEM Coordinate)
{
    CSensorBase::SetSlaveAddr(SlaveAddr);
    CSensorBase::SetLayout(SensorLayout);
    CSensorBase::SetCoordinateSystem(Coordinate);
    CSensorBase::I2cSetting(i2c_master);
}

CIst8303Driver::~CIst8303Driver(void)
{
}

CSensorBase::RETURE_TYPES CIst8303Driver::ChipsetInit(int nODR)
{
    UINT tmpInt;
    RETURE_TYPES rtl;

    rtl = I2cCheck();
    if(rtl <= I2C_EMPTY)
        return rtl;

    tmpInt = nODR;
    if(!pI2cMaster->iSenRegBitMaskWrite(m_i2c_slave_addr, IST8303_CNTRL1, 0x0F, tmpInt))
        return I2C_FAIL;

    return DATA_READY;
}

CSensorBase::RETURE_TYPES CIst8303Driver::ReadSensorData(short *ax, short *ay, short *az)
{
    RETURE_TYPES rtl;
    short tmpXYZ[3]={0};
    s16 xyzTmp[3];
    short temp = 0;
    *ax = 0;
    *ay = 0;
    *az = 0;

    rtl = I2cCheck();
    if(rtl <= I2C_EMPTY)
        return rtl;

    rtl = DataReady();
    if(rtl == I2C_FAIL || rtl == DATA_FALSE)
        return rtl;

    if(!pI2cMaster->iSenI2cRead(m_i2c_slave_addr, IST8303_DATAX, i2cBufer, 6))
        return CSensorBase::I2C_FAIL;

    for(int i=0; i<3; i++) {
        *(tmpXYZ+i) = ((i2cBufer[(i<<1)+1]<<8) & 0x1f00) | i2cBufer[i<<1];
    }

    for(int i=0; i<SENSOR_DATA_SIZE; i++) {
        xyzTmp[i] = *(tmpXYZ+i);
	}
    temp = xyzTmp[0];
    xyzTmp[0] = xyzTmp[1];
    xyzTmp[1] = temp;

    //
    for(int i = 0; i < SENSOR_DATA_SIZE; ++i)
    {
        tmpXYZ[i] = 0;
        if(m_Coordinate == CONFIG_COORDINATE_AVIATION) {
            for(int j = 0; j < 3; j++)
                tmpXYZ[i] += (short)LayoutAVIATION[m_SensorLayout][i][j] * (short)xyzTmp[j];
        } else if(m_Coordinate == CONFIG_COORDINATE_ANDROID) {

        } else {
            tmpXYZ[i] = xyzTmp[i];
        }
        m_raw.v[i] = tmpXYZ[i];
    }

    *ax = m_raw.v[0];
    *ay = m_raw.v[1];
    *az = m_raw.v[2];
    return CSensorBase::DATA_READY;
}

CSensorBase::RETURE_TYPES CIst8303Driver::ReadSensorData(short *pXYZ)
{
    int rtl = ReadSensorData(&pXYZ[0], &pXYZ[1], &pXYZ[2]);
    if(rtl != CSensorBase::DATA_READY) {
        pXYZ[0] = 0;
        pXYZ[1] = 0;
        pXYZ[2] = 0;
    }
    return (CSensorBase::RETURE_TYPES)rtl;
}


void CIst8303Driver::UnitTransformation(float *pXYZ, int unit)
{
    switch(unit)
    {
    case CONFIG_MAG_UNITS_LSB:
        for(int i=0; i<3; i++)
            pXYZ[i] = (float)m_raw.v[i];
        break;

    case CONFIG_MAG_UNITS_UT:
        for(int i=0; i<3; i++)
            pXYZ[i] = (float)m_raw.v[i]*(CONVERT_M);
        break;

    case CONFIG_MAG_UNITS_MILLI_GASS:
        for(int i=0; i<3; i++)
            pXYZ[i] = (float)m_raw.v[i]*(CONVERT_M)*10000;
        break;
    }
}

CSensorBase::RETURE_TYPES CIst8303Driver::SoftReset()
{
    RETURE_TYPES rtl;

    rtl = I2cCheck();
    if(rtl <= I2C_EMPTY)
        return rtl;

    if(!pI2cMaster->iSenRegBitMaskWrite(m_i2c_slave_addr, IST8303_CNTRL2, 0x01, 0x01))
        return I2C_FAIL;

    Sleep(300);

    return DATA_READY;
}

CSensorBase::RETURE_TYPES CIst8303Driver::DataReady()
{
    if(!pI2cMaster->iSenI2cRead(m_i2c_slave_addr, IST8303_STAT, i2cBufer, 1))
        return I2C_FAIL;

    return(RETURE_TYPES)((i2cBufer[0]&IST8303_STAT_DRDY)==IST8303_STAT_DRDY);
}

CSensorBase::RETURE_TYPES CIst8303Driver::GetOffset(unsigned short *pBoffset)
{
    for(int i=0; i<3; i++)
        *(pBoffset+i)=0;

    if(!pI2cMaster->iSenI2cRead(m_i2c_slave_addr, 0xDC, i2cBufer, 6))
        return I2C_FAIL;
    *(pBoffset)= ((i2cBufer[1]<<8) & 0xff00) | i2cBufer[0];
    *(pBoffset+1)= ((i2cBufer[3]<<8) & 0xff00) | i2cBufer[2];
    *(pBoffset+2)= ((i2cBufer[5]<<8) & 0xff00) | i2cBufer[4];

    return DATA_READY;
}
