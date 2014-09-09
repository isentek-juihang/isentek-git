#include "stdafx.h"
#include "ISenI2cAccess.h"

#include "MyUtility.h"

#include "IST8301Def.h"
#include "IST8303Def.h"
#include "DMARD08.h"
#include "KXTF9.h"

////bruce
#define ITG3205_I2CSLAVEADD		0x68
#define ITG3205_T_REG_START_ADDRESS 0x1B
#define ITG3205_X_REG_START_ADDRESS 0x1D
#define ITG3205_Y_REG_START_ADDRESS 0x1F
#define ITG3205_Z_REG_START_ADDRESS 0x21
#define ITG3205_RATE_REG_START_ADDRESS 0x15
#define ITG3205_DLPF_START_ADDRESS 0x16
#define ITG3205_INT_CON 0x17
#define ITG3205_INT_STS 0x1A

extern int IST8301_I2CSLAVEADD;
extern int IST8303_I2CSLAVEADD;
extern int DMARD08_I2CSLAVEADD;

CISenI2cAccess::CISenI2cAccess(void) : m_niSenUSBI2C(I2CDEV_NONE)
	, pI2cMaster(NULL)
{
	for(int i=0; i<I2C_BUF_SIZE; i++)
		i2cBufer[i] = 0;

	pI2cMaster = CreateIicObj();
}


CISenI2cAccess::~CISenI2cAccess(void)
{
	ReleaseIicObj(pI2cMaster);
}


int CISenI2cAccess::iSenOpenI2cAccess(unsigned int nMaster)
{
	return pI2cMaster->iSenOpenI2cMaster(nMaster);
}

int CISenI2cAccess::iSenCloseI2cAccess(void)
{
	return pI2cMaster->iSenCloseI2cMaster();
}

bool CISenI2cAccess::iSenI2cRead(unsigned short SlaveAddr, unsigned char RegAddr, unsigned char *rData, unsigned short length)
{
	return pI2cMaster->iSenI2cRead(SlaveAddr, RegAddr, rData, length);
}

bool CISenI2cAccess::iSenI2cWrite(unsigned short SlaveAddr, unsigned char RegAddr, const unsigned char *wData, unsigned short length, unsigned int sleep_ms)
{
	return pI2cMaster->iSenI2cWrite(SlaveAddr, RegAddr, wData, length);
}

bool CISenI2cAccess::iSenRegBitMaskWrite(unsigned short SlaveAddr, unsigned char RegAddr, unsigned int Mask, unsigned int Data, int byteSize)
{
	return pI2cMaster->iSenRegBitMaskWrite(SlaveAddr, RegAddr, Mask, Data, byteSize);
}

bool CISenI2cAccess::CheckI2c(void)
{
	return pI2cMaster->CheckI2c();
}

CISenI2cAccess::I2C_DEVICE CISenI2cAccess::GetI2cDevSel(void)
{
	return (CISenI2cAccess::I2C_DEVICE)pI2cMaster->GetI2cDevSel();
}

inline void CISenI2cAccess::AddBuffer(int index, BYTE data)
{
	i2cBufer[index] = data;
}

bool CISenI2cAccess::IST8301_SoftReset(void)
{
	if(!CheckI2c()) {
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return false;
	}	
	
	if(!iSenRegBitMaskWrite(IST8301_I2CSLAVEADD, IST8301_CNTRL3, 0x80, 0x80))
		return false;
	
	Sleep(300); 

	return true;
}

bool CISenI2cAccess::IST8301_CheckDataReady(void)
{
	bool bResult = false;

	if(!iSenI2cRead(IST8301_I2CSLAVEADD, IST8301_STAT, i2cBufer, 1))
		return false;

	bResult = (bool)((i2cBufer[0] & IST8301_STAT_DRDY)>>6);

	return bResult;
}

int CISenI2cAccess::IST8301_GetTemperature(void)
{
	int Temperature = 0;

	AddBuffer(0, 1);
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_SENSOR_EN, i2cBufer, 1))
		return Temperature;
			
	AddBuffer(0, 1);
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_ADC_CTRL, i2cBufer, 1))
		return Temperature;
			
	if(!iSenI2cRead(IST8301_I2CSLAVEADD, IST8301_ADC_OUT, i2cBufer, 2))
		return Temperature;
	
	Temperature = ((i2cBufer[1]<<8) & 0x1f00) | i2cBufer[0];
	
	return Temperature;
}

int CISenI2cAccess::IST8301_SetOffsetDrift(BYTE nRegAddr, int Offset)
{
	UINT regData = 0;
	UINT nMSB = 0;

	if(nRegAddr==IST8301_OFFX || nRegAddr==IST8301_OFFY || nRegAddr==IST8301_OFFZ) {

		regData = NumericInt2Reg(Offset, true, 12);
		
		nMSB = (0x1000&regData)>>12;
		if(nMSB){
			for(int i=0; i<3; i++)
				nMSB = (nMSB<<1)|0x01;
			nMSB = nMSB<<12;
			regData = ( regData & (~0xf000)) | (nMSB) ;
		}

		AddBuffer(0, regData & 0xff);
		AddBuffer(1, (regData &0xff00)>>8);
		if(!iSenI2cWrite(IST8301_I2CSLAVEADD, nRegAddr, i2cBufer, 2))
			return -1;

		return true;
	}
	return false;
}
#if 0
int CISenI2cAccess::IST8301_GetXYZ(BYTE nRegAddr)
{
	int xyzData = 0;
	UINT regData = 0;

	if(nRegAddr==IST8301_DATAX || nRegAddr==IST8301_DATAY || nRegAddr==IST8301_DATAZ) {

		if(!iSenI2cRead(IST8301_I2CSLAVEADD, nRegAddr, i2cBufer, 2))
			return xyzData;
		regData = ((i2cBufer[1]<<8) & 0x1f00) | i2cBufer[0];	
	
		Reg2NumericInt(regData, &xyzData, true, 12);
	}
	return xyzData;
}
#else
//For Only set
int CISenI2cAccess::IST8301_GetXYZ(BYTE nRegAddr)
{
	int xyzData = 0;
	UINT regData = 0;

	if(nRegAddr==IST8301_DATAX || nRegAddr==IST8301_DATAY || nRegAddr==IST8301_DATAZ) {

		if(!iSenI2cRead(IST8301_I2CSLAVEADD, nRegAddr, i2cBufer, 2))
			return xyzData;
		xyzData = ((i2cBufer[1]<<8) & 0x1f00) | i2cBufer[0];	

		if(nRegAddr==IST8301_DATAX)
			xyzData -= (4096+439);
		else if(nRegAddr==IST8301_DATAY)
			xyzData -= (4096+445);
		else
			xyzData -= (4096+470);
	}
	return xyzData;
}
#endif
//Temporary use
bool CISenI2cAccess::IST8301_SetOffset( int nOffsetX, int nOffsetY, int nOffsetZ )
{
	AddBuffer(0, nOffsetX & 0xff);
	AddBuffer(1, 0);
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_CHX_OFFSET, i2cBufer, 2))
		goto _ERROR;

	AddBuffer(0, nOffsetY & 0xff);
	AddBuffer(1, 0);
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_CHY_OFFSET, i2cBufer, 2))
		goto _ERROR;

	AddBuffer(0, nOffsetZ & 0xff);
	AddBuffer(1, 0);
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_CHZ_OFFSET, i2cBufer, 2))
		goto _ERROR;	

	return true;

_ERROR:
	AfxMessageBox(_T("Fail to init iSenTek chipset(I2C error)"));
	return false;
}

bool CISenI2cAccess::IST8301_ChipsetInit(int nOffsetX, int nOffsetY, int nOffsetZ, int nOffsetDriftX, int nOffsetDriftY, int nOffsetDriftZ, int nODR, int nPresetTime, int nInterruptThresholdValue)
{
	UINT tmpInt;

	if(!IST8301_SetOffset( nOffsetX, nOffsetY, nOffsetZ ))
		goto _ERROR;

	//For only set
	AddBuffer(0, 0x1C); 
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_CHX_GAIN, i2cBufer))
		goto _ERROR;

	AddBuffer(0, 0x1C); 
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_CHY_GAIN, i2cBufer))
		goto _ERROR;

	AddBuffer(0, 0x13); 
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_CHZ_GAIN, i2cBufer))
		goto _ERROR;
	//End of for only set
	tmpInt = ( 0x80 & (~(0x3<<3))) | (nODR<<3) ;
	if(!iSenRegBitMaskWrite(IST8301_I2CSLAVEADD, IST8301_CNTRL1, 0x9A, tmpInt))
		goto _ERROR;

	tmpInt = ( 0x1C & (~(0x3))) | (0x00) ;	
	if(!iSenRegBitMaskWrite(IST8301_I2CSLAVEADD, IST8301_CNTRL2, 0xFF, tmpInt))
		goto _ERROR;

	IST8301_SetOffsetDrift(IST8301_OFFX, nOffsetDriftX);
	IST8301_SetOffsetDrift(IST8301_OFFY, nOffsetDriftY);
	IST8301_SetOffsetDrift(IST8301_OFFZ, nOffsetDriftZ);

	if(nInterruptThresholdValue>4095) nInterruptThresholdValue=4095;
	AddBuffer(0, nInterruptThresholdValue & 0xff);
	AddBuffer(1, (nInterruptThresholdValue &0x0f00)>>8);
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_ITHR1, i2cBufer, 2))
		goto _ERROR;

	AddBuffer(0, nPresetTime);
	if(!iSenI2cWrite(IST8301_I2CSLAVEADD, IST8301_PRET, i2cBufer))
		goto _ERROR;

	return true;

_ERROR:
	AfxMessageBox(_T("Fail to init iSenTek chipset(I2C error)"));
	return false;
}

bool CISenI2cAccess::IST8301_ClearInterruptSignal(void)
{
	if(!iSenI2cRead(IST8301_I2CSLAVEADD, IST8301_INL, i2cBufer, 1))
		return false;
	else
		return true;
}


/////////////////////////////////////////////////////////////////////////////////
bool CISenI2cAccess::IST8303_CheckDataReady(void)
{
	bool bResult = false;

	if(!iSenI2cRead(IST8303_I2CSLAVEADD, IST8303_STAT, i2cBufer, 1))
		return false;

	bResult = (bool)((i2cBufer[0] & IST8303_STAT_DRDY)==IST8303_STAT_DRDY);

	return bResult;
}

int CISenI2cAccess::IST8303_GetXYZ(BYTE nRegAddr)
{
	short xyzData = 0;
	UINT regData = 0;

	if (nRegAddr==IST8303_DATAX || nRegAddr==IST8303_DATAY || nRegAddr==IST8303_DATAZ) {

		if (!iSenI2cRead(IST8303_I2CSLAVEADD, nRegAddr, i2cBufer, 2))
			return xyzData;
		xyzData = (short)(i2cBufer[1]<<8) | i2cBufer[0];	
	}
	return xyzData;
}

bool CISenI2cAccess::IST8303_ChipsetInit(int nODR)
{
	UINT tmpInt;

	tmpInt = nODR;
	if (!iSenRegBitMaskWrite(IST8303_I2CSLAVEADD, IST8303_CNTRL1, 0x0F, tmpInt))
		goto _ERROR;

	return true;

_ERROR:
	AfxMessageBox(_T("Fail to init iSenTek chipset(I2C error)"));
	return false;
}
/////////////////////////////////////////////////////////////////////////////////
bool CISenI2cAccess::KXTF9_Chipset_Init(void)
{
	//BYTE databuf[10]={0};
    int res = 0;
	CString tmpStr;

	//databuf[0] = KXTF9_REG_CTRL_REG1;//0x1B
	//databuf[0] = KXTF9_OPERATION | KXTF9_12BIT;//0xC0
	AddBuffer(0, KXTF9_OPERATION | KXTF9_12BIT);
	res = iSenI2cWrite(KXTF9_I2CSLAVEADD, KXTF9_REG_CTRL_REG1, i2cBufer, 1);
	if (res <= 0) 
        goto exit_KXTF9_Chipset_Init;
    
	//databuf[0] = KXTF9_REG_DATA_CTRL;//0x21
	//databuf[0] = KXTF9_DATA_ODR_400HZ;//0x05 output data rate 400HZ
	AddBuffer(0, KXTF9_DATA_ODR_400HZ);
	res = iSenI2cWrite(KXTF9_I2CSLAVEADD, KXTF9_REG_DATA_CTRL, i2cBufer, 1);
	if (res <= 0) 
        goto exit_KXTF9_Chipset_Init;

exit_KXTF9_Chipset_Init:
    if (res <= 0) {
		tmpStr.Format(_T("Fail to init kionix chipset(I2C error): ret value=%d"), res);
		AfxMessageBox(tmpStr);
    } 
    return res;		
}

int CISenI2cAccess::KXTF9_ReadChipInfo(char *buf, int bufsize)
{
    BYTE databuf[10]; 
    
    memset(databuf, 0, sizeof(BYTE )*10);
        
    if ((!buf)||(bufsize<=30))
        return -1;

	if(!CheckI2c()) {
		*buf = 0;
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return -2;
	}
    
    sprintf(buf, "KXTF9 Chip");
    return 0;
}

int CISenI2cAccess::KXTF9_ReadSensorData(int *ax, int *ay, int *az)
{
    BYTE databuf[20];
    UINT x, y, z;
	int xyzData = 0;

    int res = 0;
	
	if(!CheckI2c()) {
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return -2;
	}

	if(!iSenI2cRead(KXTF9_I2CSLAVEADD, KXTF9_REG_XOUT_L, databuf, 6))
		return -2;
	x = ((int) databuf[1]) << 4 | ((int) databuf[0]) >> 4;
	y = ((int) databuf[3]) << 4 | ((int) databuf[2]) >> 4;
	z = ((int) databuf[5]) << 4 | ((int) databuf[4]) >> 4;
	
	Reg2NumericInt(x, &xyzData, true, 11);
	*ax = xyzData;
	
	
	Reg2NumericInt(y, &xyzData, true, 11);
	*ay = xyzData;
    
	
	Reg2NumericInt(z, &xyzData, true, 11);
	*az = xyzData;
	
    return 0;
}

int CISenI2cAccess::KXTF9_ReadSensorData(char *buf, int bufsize)
{
    BYTE data_out[20];
    int ax, ay, az;
        
    if ((!buf)||(bufsize<=80))
        return -1;

	if(!CheckI2c()) {
		*buf = 0;
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return -2;
	}

	if(!iSenI2cRead(KXTF9_I2CSLAVEADD, KXTF9_REG_XOUT_L, data_out, 6))
		return -2;

	ax = ((int) data_out[1]) << 4 | ((int) data_out[0]) >> 4;
    ay = ((int) data_out[3]) << 4 | ((int) data_out[2]) >> 4;
    az = ((int) data_out[5]) << 4 | ((int) data_out[4]) >> 4;
	
exit_KXTF9_ReadSensorData:  
        sprintf(buf, "%04x %04x %04x", ax,ay,az);
    return 0;
}

size_t CISenI2cAccess::show_chipinfo_value(struct device *dev, struct device_attribute *attr, char *buf)
{
    char strbuf[KXTF9_BUFSIZE];
    KXTF9_ReadChipInfo(strbuf, KXTF9_BUFSIZE);
    return sprintf(buf, "%s\n", strbuf);        
}

size_t CISenI2cAccess::show_sensordata_value(struct device *dev, struct device_attribute *attr, char *buf)
{
    char strbuf[KXTF9_BUFSIZE];
    KXTF9_ReadSensorData(strbuf, KXTF9_BUFSIZE);
    return sprintf(buf, "%s\n", strbuf);            
}

///////////////////////////////////////////////////////////////////////////////////

bool CISenI2cAccess::DMARD08_ReadSensorData(int *ax, int *ay, int *az)
{
	unsigned char data[8], data_len = 8;
	unsigned char adc_bit = 11;

	if(!CheckI2c()) {
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return false;
	}

	if(!iSenI2cRead(DMARD08_I2CSLAVEADD, DMARD01_T_REG_START_ADDRESS, data, data_len))
		return false;

				
	*ax = (data[2]<<3)|(data[3]&7);
	if (*ax & (1<<(adc_bit-1))) *ax -= (1<<adc_bit); 
		
	*ay = (data[4]<<3)|(data[5]&7);
	if (*ay & (1<<(adc_bit-1))) *ay -= (1<<adc_bit); 
				
	*az = (data[6]<<3)|(data[7]&7);
	if (*az & (1<<(adc_bit-1))) *az -= (1<<adc_bit); 
	
    return true;
}


/**********************************************ITG3205 by bruce********************************************************/


bool CISenI2cAccess::ITG3205_Rate( int ITG3205Rate )//reg21 sampling rate
{
	if(ITG3205Rate>255||ITG3205Rate<0)
		goto _ERROR;

	AddBuffer(0, ITG3205Rate & 0xff);
	if(!iSenI2cWrite(ITG3205_I2CSLAVEADD, ITG3205_RATE_REG_START_ADDRESS, i2cBufer, 1))
		goto _ERROR;
	return true;

_ERROR:
	AfxMessageBox(_T("Fail to write ITG3205 register21(sampling ate)"));
	return false;
}

bool CISenI2cAccess::ITG3205_DLPF( int DLPF_CFG )//reg22 low pass filter
{
	if(DLPF_CFG>7||DLPF_CFG<0)
		goto _ERROR;
	int datta=(int)3<<3|DLPF_CFG;

	AddBuffer(0, datta & 0xff);
	
	if(!iSenI2cWrite(ITG3205_I2CSLAVEADD, ITG3205_DLPF_START_ADDRESS, i2cBufer, 1))
		goto _ERROR;
	return true;

_ERROR:
	AfxMessageBox(_T("Fail to write ITG3205 register22(DLPF, Full scale)"));
	return false;
}

bool CISenI2cAccess::ITG3205_INTCON(void)
{
	
	int datta=1;

	AddBuffer(0, datta & 0xff);
	
	if(!iSenI2cWrite(ITG3205_I2CSLAVEADD, ITG3205_INT_CON, i2cBufer, 1))
		goto _ERROR;
	return true;

_ERROR:
	AfxMessageBox(_T("Fail to write ITG3205 register22(DLPF, Full scale)"));
	return false;
}
	

int  CISenI2cAccess::ITG3205_INTSTATUS(void)
{
BYTE databuf[1];
   
	int datta ;
	
	
	if(!CheckI2c()) {
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return -2;
	}

	if(!iSenI2cRead(ITG3205_I2CSLAVEADD, ITG3205_INT_STS, databuf, 1))
		return -2;
	datta = ((int) databuf[0]);
	
	
    return datta;
}


int CISenI2cAccess::ITG3205_ReadSensorData(int *ax, int *ay, int *az)//reg27-34 sensor outputs
{
	BYTE databuf[20];
    UINT x, y, z;
	int xyzData = 0;
	unsigned char adc_bit = 16;
    int res = 0;
	
	if(!CheckI2c()) {
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return -2;
	}

	if(!iSenI2cRead(ITG3205_I2CSLAVEADD, ITG3205_X_REG_START_ADDRESS, databuf, 6))
		return -2;
	x = ((int) databuf[0]) << 8 | ((int) databuf[1]);
	y = ((int) databuf[2]) << 8 | ((int) databuf[3]);
	z = ((int) databuf[4]) << 8 | ((int) databuf[5]);
	 
	
	Reg2NumericInt(x, &xyzData, true, 16);
	*ax = xyzData;
	if (*ax & (1<<(adc_bit-1))) *ax -= (1<<adc_bit); 
	
	Reg2NumericInt(y, &xyzData, true, 16);
	*ay = xyzData;
    if (*ay & (1<<(adc_bit-1))) *ay -= (1<<adc_bit); 
	
	Reg2NumericInt(z, &xyzData, true, 16);
	*az = xyzData;
	if (*az & (1<<(adc_bit-1))) *az -= (1<<adc_bit); 
    return 0;
}


CString CISenI2cAccess::GetLibVer(void)
{
	CString ver;
	
	if(pI2cMaster)
		ver = pI2cMaster->LibVer();
	return ver;
}/**/
