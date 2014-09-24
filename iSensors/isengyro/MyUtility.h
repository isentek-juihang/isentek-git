#ifndef MYUTILITY_H
#define MYUTILITY_H

//#include <Mmsystem.h>

unsigned long HexadecimalToDecimal(const TCHAR *Hexadecimal);

const int SIGNEDBIT_NUM =1;
const float NUMBER_BASE =2.0F;

// Register To NumericValue
void Reg2Numeric(int nReg, double *NumericValue, unsigned char SignBit, unsigned char IntegerLen, unsigned char FloatPointLen);

// NumericValue To Register
int Numeric2Reg(double NumericValue, unsigned char SignBit, unsigned char IntegerLen, unsigned char FloatPointLen);

void Reg2NumericInt(unsigned int nReg, int *NumericValue, bool SignBit, unsigned char IntegerLen);
unsigned int  NumericInt2Reg(int NumericValue, bool SignBit, unsigned char IntegerLen);

//CString Get_FileVersion();
#endif