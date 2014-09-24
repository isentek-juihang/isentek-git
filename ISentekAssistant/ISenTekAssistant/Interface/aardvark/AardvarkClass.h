#pragma once

#include "aardvark.h"

class CAardvarkClass
{
public:

	enum {	TARGET_POWER_NONE  = 0x00, 
			TARGET_POWER_BOTH  = 0x03, 
			TARGET_POWER_QUERY = 0x80};

	enum {	I2C_PULLUP_NONE  = 0x00,
			I2C_PULLUP_BOTH  = 0x03,
			I2C_PULLUP_QUERY = 0x80};

	enum I2cStatus {
			I2C_STATUS_OK            = 0,
			I2C_STATUS_BUS_ERROR     = 1,
			I2C_STATUS_SLA_ACK       = 2,
			I2C_STATUS_SLA_NACK      = 3,
			I2C_STATUS_DATA_NACK     = 4,
			I2C_STATUS_ARB_LOST      = 5,
			I2C_STATUS_BUS_LOCKED    = 6,
			I2C_STATUS_LAST_DATA_ACK = 7};

	static const int DefultI2cBitRate =  400;

	CAardvarkClass(void);
	~CAardvarkClass(void);
	
	bool InitializeAardvark(void);
	bool CloseAardvark(void);

	bool I2cRead(unsigned short SlaveAddr, unsigned char RegAddr, unsigned char *rData, unsigned short length);
	bool I2cWrite(unsigned short SlaveAddr, unsigned char RegAddr, const unsigned char *wData, unsigned short length);

	int TargetPower(u08 power_mask);
	int PullUp(u08 pullup_mask);

	int SetI2cBitrate(int bitrate_khz);
	int GetI2cBitrate(void);
	

private:
	Aardvark aa_handle;

protected:
};
