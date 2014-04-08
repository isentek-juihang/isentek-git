#include "../../../StdAfx.h"
#include "../../include/Dmard08Driver.h"

#define BITS_PER_BYTE           8

static const short LayoutAVIATION[8][3][3] = {
        { { 0, 1, 0}, {-1, 0, 0}, { 0, 0,-1}},  //CONFIG_LAYOUT_PAT_1
        { { 1, 0, 0}, { 0, 1, 0}, { 0, 0,-1}},  //CONFIG_LAYOUT_PAT_2
        { { 0,-1, 0}, { 1, 0, 0}, { 0, 0,-1}},  //CONFIG_LAYOUT_PAT_3
        { {-1, 0, 0}, { 0,-1, 0}, { 0, 0,-1}},  //CONFIG_LAYOUT_PAT_4
        { { 1, 0, 0}, { 0,-1, 0}, { 0, 0, 1}},  //CONFIG_LAYOUT_PAT_5
        { { 0, 1, 0}, { 1, 0, 0}, { 0, 0, 1}},  //CONFIG_LAYOUT_PAT_6
        { {-1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}},  //CONFIG_LAYOUT_PAT_7
        { { 0,-1, 0}, {-1, 0, 0}, { 0, 0, 1}}   //CONFIG_LAYOUT_PAT_8
};

CDmard08Driver::CDmard08Driver(void)
{
    CSensorBase::SetSlaveAddr(SA_1C);
    CSensorBase::SetLayout(CSensorBase::CONFIG_LAYOUT_PAT_1);
    CSensorBase::SetCoordinateSystem(CSensorBase::CONFIG_COORDINATE_ORG);
    CSensorBase::I2cSetting(NULL);
}

CDmard08Driver::CDmard08Driver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master, LAYOUT SensorLayout, COORDINATE_SYSTEM Coordinate)
{
    CSensorBase::SetSlaveAddr(SlaveAddr);
    CSensorBase::SetLayout(SensorLayout);
    CSensorBase::SetCoordinateSystem(Coordinate);
    CSensorBase::I2cSetting(i2c_master);
}

CDmard08Driver::~CDmard08Driver(void)
{
}

CSensorBase::RETURE_TYPES CDmard08Driver::ChipsetInit(int speed)
{
    RETURE_TYPES rtl;

    rtl = I2cCheck();
    if(rtl <= I2C_EMPTY)
        return rtl;

    return gsensor_reset();
}


void CDmard08Driver::device_i2c_correct_accel_sign(s16 *val)
{
    *val<<= (sizeof(s16) * BITS_PER_BYTE - 11);
    *val>>= (sizeof(s16) * BITS_PER_BYTE - 11);
}


void CDmard08Driver::device_i2c_merge_register_values(s16 *val, unsigned char msb, unsigned char lsb)
{
    *val = (((u16)msb) << 3) | (u16)lsb;
    device_i2c_correct_accel_sign(val);
}

CSensorBase::RETURE_TYPES CDmard08Driver::ReadSensorData(s16 *pXYZ)
{
    unsigned char buffer[6];
    s16 xyzTmp[3];
    int i, j;

    if(!pI2cMaster->CheckI2c()) {
        AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
        return CSensorBase::I2C_EMPTY;
    }

    if(!pI2cMaster->iSenI2cRead(m_i2c_slave_addr, ACC_REG, buffer, 6))
        return CSensorBase::I2C_FAIL;

    //merge to 11-bits value
    for(i = 0; i < 3; ++i){
        pXYZ[i] = 0;
        device_i2c_merge_register_values((xyzTmp + i), buffer[2*i], buffer[2*i + 1]);
    }

    //Change Coordinate System
    for(i = 0; i < SENSOR_DATA_SIZE; ++i)
    {
        if(m_Coordinate == CONFIG_COORDINATE_AVIATION) {
            for(j = 0; j < 3; j++)
                pXYZ[i] += LayoutAVIATION[m_SensorLayout][i][j] * xyzTmp[j];
        } else if(m_Coordinate == CONFIG_COORDINATE_ANDROID) {

        } else {
            pXYZ[i] = xyzTmp[i];
        }
        m_raw.v[i] = pXYZ[i];
    }
	return DATA_READY;
}

CSensorBase::RETURE_TYPES CDmard08Driver::ReadSensorData(short *ax, short *ay, short *az)
{
    s16 xyz[3];

    RETURE_TYPES rtl = ReadSensorData((s16 *)&xyz);
    if(rtl == CSensorBase::DATA_READY) {
        *ax = xyz[0];
        *ay = xyz[1];
        *az = xyz[2];
    } else {
        *ax = 0;
        *ay = 0;
        *az = 0;
    }
    return rtl;
}

void CDmard08Driver::UnitTransformation(float *pXYZ, int unit)
{
    switch(unit)
    {
    case CONFIG_ACCEL_UNITS_LSB:
        for(int i=0; i<3; i++)
            pXYZ[i] = (float)m_raw.v[i];
        break;

    case CONFIG_ACCEL_UNITS_MPSS:
        for(int i=0; i<3; i++)
            pXYZ[i] = (float)m_raw.v[i]*((float)CDmard08Driver::CONVERT_A);
        break;
    }

}

CSensorBase::RETURE_TYPES CDmard08Driver::gsensor_reset()
{
    unsigned char buffer[2];

    AddBuffer(0, BANDWIDTH_REG_VALUE);
    if (!pI2cMaster->iSenI2cWrite(m_i2c_slave_addr, BANDWIDTH_REG, i2cBufer, 1))
        return I2C_FAIL;

    if(!pI2cMaster->iSenI2cRead(m_i2c_slave_addr, BANDWIDTH_REG, buffer, 1))
        return I2C_FAIL;

    return DATA_READY;
}

const double CDmard08Driver::CONVERT_A = (GRAVITY_EARTH / 256.0f);
