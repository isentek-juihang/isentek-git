#pragma once
#include "SensorBase.h"

class CDmard08Driver :  public CSensorBase
{
public:
    CDmard08Driver(void);
    CDmard08Driver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master=NULL, LAYOUT SensorLayout=CONFIG_LAYOUT_PAT_1, COORDINATE_SYSTEM Coordinate=CONFIG_COORDINATE_ORG);
    virtual ~CDmard08Driver(void);

    virtual RETURE_TYPES ChipsetInit(int speed=1);

    virtual RETURE_TYPES ReadSensorData(short *x, short *y, short *z);
    virtual RETURE_TYPES ReadSensorData(s16 *pXYZ);

    virtual void UnitTransformation(float *pXYZ, int unit=0);

    RETURE_TYPES gsensor_reset(void);

    enum  SA{ SA_1C=0x1C, SA_1D=0x1D };

private:
    inline void device_i2c_correct_accel_sign(s16 *val);
    void device_i2c_merge_register_values(s16 *val, unsigned char msb, unsigned char lsb);

    static const unsigned char DMARD01_T_REG_START_ADDRESS = 0;
    static const unsigned char DMARD01_X_REG_START_ADDRESS = 0x02;
    static const unsigned char DMARD01_Y_REG_START_ADDRESS = 0x04;
    static const unsigned char DMARD01_Z_REG_START_ADDRESS = 0x06;

    static const int DEFAULT_SENSITIVITY = 256;
    static const unsigned char CHIP_ID_REG = 0x0a;
    static const unsigned char CHIP_ID_VALUE = 0x88;
    static const unsigned char BANDWIDTH_REG = 0x08;
    static const unsigned char BANDWIDTH_REG_VALUE = 0x03;
    #define ACC_REG                     0x02
    #define AUTO_CALIBRATION_MAX        20
    #define AUTO_CALIBRATION_MIN        -20

    /* conversion of acceleration data to SI units (m/s^2) */
    /* 720 LSB = 1G */
	static const double CONVERT_A;
    //#define CONVERT_A       (GRAVITY_EARTH / 256.0f)  //1g = 256codes
};

