#pragma once
#include "SensorBase.h"

class CIst8301Driver :  public CSensorBase
{
public:
    CIst8301Driver(void);
    CIst8301Driver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master=NULL, LAYOUT SensorLayout=CONFIG_LAYOUT_PAT_1, COORDINATE_SYSTEM Coordinate=CONFIG_COORDINATE_ORG);
    virtual ~CIst8301Driver(void);

    virtual RETURE_TYPES ChipsetInit(int speed=1);

    virtual RETURE_TYPES ReadSensorData(short *x, short *y, short *z);
    virtual RETURE_TYPES ReadSensorData(short *pXYZ);
    virtual void UnitTransformation(float *pXYZ, int unit=0);

    RETURE_TYPES DataReady();

    RETURE_TYPES SoftReset();
    RETURE_TYPES GetOffset(unsigned short *pBoffset);

    enum  ODR{ ODR_POINT5SPS, ODR_10SPS, ODR_20SPS, ODR_100SPS };
    enum  SA{ SA_0E=0x0E, SA_0F=0x0F };

private:
    //Register Address
    static const unsigned char IST8301_STB  = 0x0C; //Self-Test response
    static const unsigned char IST8301_INFO = 0x0D; //More Info
    static const unsigned char IST8301_WIA = 0x0F;  //Who I am
    static const unsigned char IST8301_DATAX = 0x10;    //Output Value x
    static const unsigned char IST8301_DATAY = 0x12;    //Output Value y
    static const unsigned char IST8301_DATAZ = 0x14;    //Output Value z
    static const unsigned char IST8301_INS = 0x16;  //Interrupt source register & FIFO info
    static const unsigned char IST8301_STAT = 0x18; //Status register
    static const unsigned char IST8301_INL = 0x1A;  //Interrupt latch release register
    static const unsigned char IST8301_CNTRL1 = 0x1B;   //Control setting register 1
    static const unsigned char IST8301_CNTRL2 = 0x1C;   //Control setting register 2
    static const unsigned char IST8301_CNTRL3 = 0x1D;   //Control setting register 3

    #define IST8301_STAT_DRDY       (1<<6)

    /* conversion of magnetic data to uT units */
    #define CONVERT_M                   (0.33f)
};

