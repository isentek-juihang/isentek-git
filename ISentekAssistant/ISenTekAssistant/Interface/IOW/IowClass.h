#pragma once

#include "iowkit.h"

class CIowClass
{
public:
	CIowClass(void);
	virtual ~CIowClass(void);

	// STANDARD=93.75kHz; FAST=375kHz; SLOW=46.8kHz
	enum I2C_SPEED{ I2CSPD_STANDARD, I2CSPD_FAST, I2CSPD_SLOW };
	
	virtual bool InitializeIow(void);
	virtual bool CloseIow(void);

	virtual bool I2cRead(unsigned short SlaveAddr, unsigned char RegAddr, unsigned char *rData, unsigned short length);
	virtual bool I2cWrite(unsigned short SlaveAddr, unsigned char RegAddr, const unsigned char *wData, unsigned short length);

	virtual CString getI2cDongleInfo(void);

	int SetI2cBitRate(I2C_SPEED bitrate_khz);

private:

	IOWKIT_HANDLE iowHandle;
	ULONG Pid;
	ULONG Rev;
	ULONG NumDevs;
	WCHAR sn[16];
	PCSTR Ver;
	IOWKIT56_SPECIAL_REPORT report;

	CString info;
	bool SendReport(IOWKIT56_SPECIAL_REPORT *report);
	bool ReceiveReport(IOWKIT56_SPECIAL_REPORT *report);


protected:
};