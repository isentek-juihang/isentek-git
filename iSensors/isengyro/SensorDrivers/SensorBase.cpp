#include "../StdAfx.h"
#include "./include/SensorBase.h"


CSensorBase::CSensorBase(void) : m_i2c_slave_addr(0)
    , m_SensorLayout(CONFIG_LAYOUT_PAT_1)
    , m_Coordinate(CONFIG_COORDINATE_ORG)
    , pI2cMaster(NULL)
{
    for(int i=0; i<I2C_BUF_SIZE; i++)
        i2cBufer[i] = 0;

    for(int i=0; i<nSensorAxisNum; i++)
        m_raw.v[i] = 0;
}

CSensorBase::CSensorBase(unsigned char SlaveAddr, CISenI2cMaster *i2c_master, LAYOUT SensorLayout, COORDINATE_SYSTEM Coordinate) : m_i2c_slave_addr(SlaveAddr)
    , m_SensorLayout(SensorLayout)
    , m_Coordinate(Coordinate)
{
    for(int i=0; i<I2C_BUF_SIZE; i++)
        i2cBufer[i] = 0;

    for(int i=0; i<nSensorAxisNum; i++)
        m_raw.v[i] = 0;
}

CSensorBase::~CSensorBase(void)
{
}

void CSensorBase::AddBuffer(int index, BYTE data)
{
    i2cBufer[index] = data;
}

void CSensorBase::SetSlaveAddr(unsigned char i2c_slave)
{
    m_i2c_slave_addr = i2c_slave;
}

void CSensorBase::SetLayout(LAYOUT layout)
{
    m_SensorLayout = layout;
}

void CSensorBase::SetCoordinateSystem(COORDINATE_SYSTEM Coordinate)
{
    m_Coordinate = Coordinate;
}

void CSensorBase::I2cSetting(CISenI2cMaster *i2c_master)
{
    pI2cMaster = i2c_master;
}

CSensorBase::RETURE_TYPES CSensorBase::I2cCheck(void)
{
    if(!pI2cMaster )
        return I2C_NULL;

    if(!pI2cMaster->CheckI2c())
        return I2C_EMPTY;

	return DATA_READY;
}
