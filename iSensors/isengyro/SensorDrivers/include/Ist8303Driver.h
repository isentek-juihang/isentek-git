#pragma once
#include "SensorBase.h"

class CIst8303Driver :  public CSensorBase
{
public:
    CIst8303Driver(void);
    CIst8303Driver(unsigned char SlaveAddr, CISenI2cMaster *i2c_master=NULL, LAYOUT SensorLayout=CONFIG_LAYOUT_PAT_1, COORDINATE_SYSTEM Coordinate=CONFIG_COORDINATE_ORG);
    virtual ~CIst8303Driver(void);

    virtual RETURE_TYPES ChipsetInit(int speed=1);

    virtual RETURE_TYPES ReadSensorData(short *x, short *y, short *z);
    virtual RETURE_TYPES ReadSensorData(short *pXYZ);
    virtual void UnitTransformation(float *pXYZ, int unit=0);

    RETURE_TYPES DataReady();

    RETURE_TYPES SoftReset();
    RETURE_TYPES GetOffset(unsigned short *pBoffset);

    enum  ODR{ ODR_POINT5SPS=9, ODR_10SPS=3, ODR_20SPS=5, ODR_50SPS=7, ODR_100SPS=6, ODR_200SPS=11 };
    enum  SA{ SA_0C=0x0C, SA_0E=0x0E };

private:
    //Register Address
    static const unsigned char IST8303_STB  =   0x0C; //Self-Test response
    static const unsigned char IST8303_INFO =   0x0D; //More Info
    static const unsigned char IST8303_WIA =    0x00; //Who I am
    static const unsigned char IST8303_DATAX =  0x03; //Output Value x
    static const unsigned char IST8303_DATAY =  0x05; //Output Value y
    static const unsigned char IST8303_DATAZ =  0x07; //Output Value z
    static const unsigned char IST8303_INS =    0x16; //Interrupt source register & FIFO info
    static const unsigned char IST8303_STAT =   0x02; //Status register
    static const unsigned char IST8303_INL =    0x1A; //Interrupt latch release register
    static const unsigned char IST8303_CNTRL1 = 0x0A; //Control setting register 1
    static const unsigned char IST8303_CNTRL2 = 0x0B; //Control setting register 2
    static const unsigned char IST8303_CNTRL3 = 0x0D; //Control setting register 3

    #define IST8303_STAT_DRDY       (1<<0)

    /* conversion of magnetic data to uT units */
    #define CONVERT_M                   (0.33f)
};

