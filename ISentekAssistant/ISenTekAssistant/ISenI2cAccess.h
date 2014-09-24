#pragma once
#include "stdafx.h"
#include "./interface/aardvark/AardvarkClass.h"

#include "./interface/IOW/iowkit.h"
#include "./interface/IOW/IowClass.h"
#include "iSenInterfaceLinker.h"

class CISenI2cAccess
{
public:
    CISenI2cAccess(void);
    ~CISenI2cAccess(void);

    enum I2C_DEVICE{ I2CDEV_NONE, AARDVARK, IOW};

    static const unsigned int I2C_BUF_SIZE = 256;
    static const unsigned int nDefaultI2C = AARDVARK;   // default value

    int iSenOpenI2cAccess(unsigned int nMaster);
    int iSenCloseI2cAccess(void);

    CISenI2cAccess::I2C_DEVICE GetI2cDevSel(void);
        
    inline bool CheckI2c(void);

    bool iSenI2cRead(unsigned short SlaveAddr, unsigned char RegAddr, unsigned char *rData, unsigned short length);
    bool iSenI2cWrite(unsigned short SlaveAddr, unsigned char RegAddr, const unsigned char *wData, unsigned short length=1, unsigned int sleep_ms=10);

    bool iSenRegBitMaskWrite(unsigned short SlaveAddr, unsigned char RegAddr, unsigned int Mask, unsigned int Data, int byteSize=1);

    inline void AddBuffer(int index, BYTE data);
    //ISenTek IST8301
    bool IST8301_SoftReset(void);
    bool IST8301_CheckDataReady(void);
    int  IST8301_GetTemperature(void);
    int  IST8301_SetOffsetDrift(BYTE nRegAddr, int Offset);
    int  IST8301_GetXYZ(BYTE nRegAddr);
    bool IST8301_SetOffset( int nOffsetX, int nOffsetY, int nOffsetZ );//Temporary use
    bool IST8301_ChipsetInit(int nOffsetX, int nOffsetY, int nOffsetZ, int nOffsetDriftX, int nOffsetDriftY, int nOffsetDriftZ, int nODR, int nPresetTime, int nInterruptThresholdValue);
    bool IST8301_ClearInterruptSignal(void);

    //ISenTek IST8303
    bool IST8303_CheckDataReady(void);
    int  IST8303_GetXYZ(BYTE nRegAddr);
    bool IST8303_ChipsetInit(int nODR);

    //Kionix kxtf9
    bool KXTF9_Chipset_Init(void);
    int KXTF9_ReadChipInfo(char *buf, int bufsize);
    int KXTF9_ReadSensorData(char *buf, int bufsize);
    int KXTF9_ReadSensorData(int *x, int *y, int *z);
    size_t show_chipinfo_value(struct device *dev, struct device_attribute *attr, char *buf);
    size_t show_sensordata_value(struct device *dev, struct device_attribute *attr, char *buf);

    //Domintech DMARD08
    bool DMARD08_ReadSensorData(int *x, int *y, int *z);

    //Invensense ITG3205
    int ITG3205_ReadSensorData(int *ax, int *ay, int *az);
    bool ITG3205_Rate( int rate );
    bool ITG3205_DLPF( int DLPF_CFG);
    bool ITG3205_INTCON(void);
    int  ITG3205_INTSTATUS(void);

    CString GetLibVer(void);
    //Test
    //void SetPan(unsigned short SlaveAddr, unsigned char RegAddr, unsigned int Mask, unsigned int Data, int byteSize=1);
protected:
    
    BYTE i2cBufer[I2C_BUF_SIZE];

    I2C_DEVICE m_niSenUSBI2C;
    
    CISenI2cMaster *pI2cMaster;
};
