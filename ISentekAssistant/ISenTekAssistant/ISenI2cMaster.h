#pragma once

#include "./interface/aardvark/AardvarkClass.h"

#include "./interface/IOW/iowkit.h"
#include "./interface/IOW/IowClass.h"


class CISenI2cMaster
{
public:
	CISenI2cMaster(void);
	virtual ~CISenI2cMaster(void);

	enum I2C_DEVICE{ I2CDEV_NONE, I2CDEV_AARDVARK, I2CDEV_IOW };

	// STANDARD=100KHz; FAST=400KHz; SLOW=50KHz
	enum I2C_SPEED{ I2CSPD_STANDARD, I2CSPD_FAST, I2CSPD_SLOW };

	virtual int iSenOpenI2cMaster(unsigned int nMaster);
	virtual int iSenCloseI2cMaster(void);

	inline I2C_DEVICE GetI2cDevSel(void);
		
	inline bool CheckI2c(void);

	// Return Value
	// TRUE if the read/write command was processed; otherwise FALSE.
	virtual bool iSenI2cRead(BYTE SlaveAddr, BYTE RegAddr, BYTE *rData, unsigned short length);
	virtual bool iSenI2cWrite(BYTE SlaveAddr, BYTE RegAddr, const BYTE *wData, unsigned short length=1);
	virtual bool iSenRegBitMaskWrite(BYTE SlaveAddr, BYTE RegAddr, unsigned int Mask, unsigned int Data, int byteSize=1);
	
	// Return Value
	// TRUE if the BitRate changing was processed; -1 if the device isn't support; otherwise FALSE.
	int SetBitRate(I2C_SPEED speed);
	int GetBitRate(void);

	virtual CString LibVer(void);

protected:	
	I2C_DEVICE m_niSenUSBI2C;
	
	CAardvarkClass *pAardvark;
	CIowClass *pIow;
};

inline bool CISenI2cMaster::CheckI2c(void)
{
	return m_niSenUSBI2C>0?true:false;
}

inline CISenI2cMaster::I2C_DEVICE CISenI2cMaster::GetI2cDevSel(void)
{
	return m_niSenUSBI2C;
}