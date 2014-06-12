#include "stdafx.h"
#include <math.h>
#include "iSensors.h"
#include "iSensorsDlg.h"
#include "SensorsHeader.h"
#include "GsensorCalibration.h"
#include "ist_magnet.h"
#include "ist_rotation.h"
#include "ist_debug.h"
#include "acceleration_calibration.h"

#define PI 3.14159265359

extern DWORD WINAPI SensorFusionProc(LPVOID lpParam);
extern CiSensorsDlg *g_pMain;

#define MAX_NUM_REAL_SENSORS 3
#define MAX_NUM_SENSORS 4

#define ID_BASE SENSORS_HANDLE_BASE
#define ID_A    (ID_BASE+0)
#define ID_M    (ID_BASE+1)
#define ID_G    (ID_BASE+2)
#define ID_O    (ID_BASE+3)

int bSensorInit[MAX_NUM_REAL_SENSORS] = {0};
CSensorBase *pSensors[MAX_NUM_REAL_SENSORS] = {NULL};

sensors_event_t Sensors[MAX_NUM_SENSORS];

short rawMlog[100000] = {0};
UINT MlogIndex = 0;

void FreezeGUI(BOOL bFreeze)
{
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_RADIO_ACCELEROMETER_NONE)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_RADIO_ACCELEROMETER_DMT08)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_RADIO_ACCELEROMETER_KXTF9)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_RADIO_ACCELEROMETER_4)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_STATIC_ACCELEROMETER_COORDINATE_SYSTEM)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_RADIO_ACCELEROMETER_COORD_ORG)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_RADIO_ACCELEROMETER_COORD_AVIATION)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_RADIO_ACCELEROMETER_COORD_ANDROID)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_STATIC_ACCELEROMETER_LAYOUT_PATTERN)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgAccelerometer.GetDlgItem(IDC_COMBO_ACCELEROMETER_LAYOUT)->EnableWindow(bFreeze);

    g_pMain->m_SubDlgMagnetometer.GetDlgItem(IDC_RADIO_MAGNETOMETER_NONE)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgMagnetometer.GetDlgItem(IDC_RADIO_MAGNETOMETER_AKM8975)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgMagnetometer.GetDlgItem(IDC_RADIO_MAGNETOMETER_IST8301)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgMagnetometer.GetDlgItem(IDC_RADIO_MAGNETOMETER_4)->EnableWindow(bFreeze);

    g_pMain->m_SubDlgGyroscope.GetDlgItem(IDC_RADIO_GYROSCOPE_NONE)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgGyroscope.GetDlgItem(IDC_RADIO_GYROSCOPE_L3G4200D)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgGyroscope.GetDlgItem(IDC_RADIO_GYROSCOPE_3)->EnableWindow(bFreeze);
    g_pMain->m_SubDlgGyroscope.GetDlgItem(IDC_RADIO_GYROSCOPE_4)->EnableWindow(bFreeze);
}

/********************************************************/
/* Function Name¡G                                      */
/* Output parameter¡G   negative¡G                      */
/*                      positive¡G                      */
/********************************************************/
#define DEV_ID_DOMITEK      0x0001
#define DEV_ID_ISENTEK_8301 0x0002
#define DEV_ID_ISENTEK_8303 0x0003
#define DEV_ID_LG3GYRO      0x0004
#define DEV_ID_KIONIX       0x0005

void DestroySensor(CSensorBase **ppSensor)
{
	if (ppSensor && *ppSensor) {
		delete *ppSensor;
		*ppSensor = NULL;
	}
}

CSensorBase *CreateSensor(int id, float fc, CISenI2cMaster *i2c)
{
	CSensorBase *sensor = NULL;
	int odr = 1;

	if (id == DEV_ID_DOMITEK) {
		sensor = new CDmard08Driver(CDmard08Driver::SA::SA_1C, 
			i2c, CSensorBase::LAYOUT::CONFIG_LAYOUT_PAT_3, 
			CSensorBase::COORDINATE_SYSTEM::CONFIG_COORDINATE_AVIATION);
	}

	if (id == DEV_ID_KIONIX) {
		sensor = new CKxtf9Driver(CKxtf9Driver::SA::SA_0F, 
			i2c, CSensorBase::CONFIG_LAYOUT_PAT_1, 
			CSensorBase::COORDINATE_SYSTEM::CONFIG_COORDINATE_AVIATION);
	}

	if (id == DEV_ID_ISENTEK_8301) {
		sensor = new CIst8301Driver(CIst8301Driver::SA::SA_0E, 
			i2c, CSensorBase::LAYOUT::CONFIG_LAYOUT_PAT_4, 
			CSensorBase::COORDINATE_SYSTEM::CONFIG_COORDINATE_AVIATION);

		if (fc <= 5.0f)
			odr = CIst8301Driver::ODR::ODR_POINT5SPS;
		else if (fc > 5.0f && fc <= 10.0f)
			odr = CIst8301Driver::ODR::ODR_10SPS;
		else if (fc > 10.0f && fc <= 20.0f)
			odr = CIst8301Driver::ODR::ODR_20SPS;
		else if (fc > 20.0f && fc <= 100.0f)
			odr = CIst8301Driver::ODR::ODR_100SPS;
		else
			odr = CIst8301Driver::ODR::ODR_100SPS;
	}

	if (id == DEV_ID_ISENTEK_8303) {
		sensor = new CIst8303Driver(CIst8303Driver::SA::SA_0C, 
			i2c, CSensorBase::LAYOUT::CONFIG_LAYOUT_PAT_2, 
			CSensorBase::COORDINATE_SYSTEM::CONFIG_COORDINATE_AVIATION);

		if (fc <= 5.0f)
			odr = CIst8303Driver::ODR::ODR_POINT5SPS;
		else if (fc > 5.0f && fc <= 10.0f)
			odr = CIst8303Driver::ODR::ODR_10SPS;
		else if (fc > 10.0f && fc <= 20.0f)
			odr = CIst8303Driver::ODR::ODR_20SPS;
		else if (fc > 20.0f && fc <= 50.0f)
			odr = CIst8303Driver::ODR::ODR_50SPS;
		else if (fc > 50.0f && fc <= 100.0f)
			odr = CIst8303Driver::ODR::ODR_100SPS;
		else
			odr = CIst8303Driver::ODR::ODR_200SPS;
	}

	if (id == DEV_ID_LG3GYRO) {
		sensor = new CL3g4200dDriver(CL3g4200dDriver::SA_69, 
			i2c, CSensorBase::LAYOUT::CONFIG_LAYOUT_PAT_2, 
			CSensorBase::COORDINATE_SYSTEM::CONFIG_COORDINATE_AVIATION);
	}

	if (CSensorBase::RETURE_TYPES::DATA_READY != sensor->ChipsetInit(odr))
		goto EXIT;

	return sensor;

EXIT:
	DestroySensor(&sensor);
	return NULL;
}

BOOL ReadSensorData(CSensorBase *s, float *x, float *y, float *z, int unit)
{
	short xyz[3];
	float fxyz[3];

	if (!s)
		goto EXIT;

	if ((int)s->ReadSensorData(xyz) < 0)
		goto EXIT;

	fxyz[0] = (float)xyz[0];
	fxyz[1] = (float)xyz[1];
	fxyz[2] = (float)xyz[2];

	//printf("raw: %14.10f,%14.10f,%14.10f\n", fxyz[0], fxyz[1], fxyz[2]);
	s->UnitTransformation(fxyz, unit);
	//printf("uT: %14.10f,%14.10f,%14.10f\n", fxyz[0], fxyz[1], fxyz[2]);
	if (x)
		*x = fxyz[0];
	if (y)
		*y = fxyz[1];
	if (z)
		*z = fxyz[2];

	return TRUE;

EXIT:
	return FALSE;
}

float GetCurrentSeconds()
{
	return ((float)GetTickCount())*0.001f;
}

float EllapsedSeconds(float *start)
{
	float n = GetCurrentSeconds(), s = start ? *start : 0.0f;
	start ? *start = n : (void)0;
	return n - s;
}

class FloatType 
{
	typedef union {
		float f;
		ISTFLOAT s;
	} convert;

	convert mVal;

public:
	explicit FloatType(const float value = 0.0f) {
		mVal.f = value;
	}
	operator ISTFLOAT () { return mVal.s; }
};

class NativeType
{
	typedef union {
		float f;
		ISTFLOAT s;
	} convert;

	convert mVal;

public:
	explicit NativeType(const ISTFLOAT value = 0) {
		mVal.s = value;
	}
	operator float () { return mVal.f; }
};

DWORD WINAPI SensorFusionProc(LPVOID lpParam)
{
#define MAGNET_VELOCITY 1
	float dT = 0.05f, fs = 1.0f/dT, mfs = fs;
	float ax, ay, az;
	float mx, my, mz;
	float gx, gy, gz;

#ifndef MAGNET_VELOCITY
	Rotation *rotation = NULL;
	ISTBOOL rotation_done = ISTFALSE;
#endif
	Magnet *magnet = NULL;
	ISTBOOL magnet_done = ISTFALSE;
	ISTFLOAT ws[3] = {0};
	ISTFLOAT gs[3] = {0};
	ISTFLOAT gdata[3];
	ISTFLOAT mdata[3];
	CSensorBase *accel_sensor = NULL;
	CSensorBase *magnet_sensor = NULL;
	CSensorBase *gyro_sensor = NULL;
	AccelerationCalibration *accel_cal = NULL;
	AcceleratorMeasurement cx, cy, cz;
	float g;

	float tmA, tmM;
	DWORD dwRet, dwCnt, dwStart, dwEnd, dwTimeout;
	DWORD samples = (DWORD)(fs/mfs);
	DWORD period = (DWORD)(1000.0f/fs);
	HANDLE event = NULL;

	IST_DBG("+++ Enter SensorFusionProc() +++\n");

	FreezeGUI(false);

	accel_sensor = CreateSensor(DEV_ID_DOMITEK, fs, g_pMain->m_iSenI2c);
	if (!accel_sensor) {
		IST_DBG("!accel_sensor\n");
		goto EXIT;
	}

	magnet_sensor = CreateSensor(DEV_ID_ISENTEK_8301, mfs, g_pMain->m_iSenI2c);
	if (!magnet_sensor) {
		IST_DBG("!magnet_sensor\n");
		goto EXIT;
	}

	gyro_sensor = CreateSensor(DEV_ID_LG3GYRO, fs, g_pMain->m_iSenI2c);
	if (!gyro_sensor) {
		IST_DBG("!gyro_sensor\n");
		goto EXIT;
	}

	magnet = IST_Magnet()->New();
	if (!magnet) {
		IST_DBG("!magnet\n");
		goto EXIT;
	}
#ifndef MAGNET_VELOCITY
	rotation = IST_Rotation()->New();
	if (!rotation) {
		IST_DBG("!rotation\n");
		goto EXIT;
	}
#endif

#define GRAVITY_SENSOR_POSITIVE_X_OFFSET		12.600f
#define GRAVITY_SENSOR_NEGATIVE_X_OFFSET		-8.450f
#define GRAVITY_SENSOR_ZERO_X_OFFSET			1.838f
#define GRAVITY_SENSOR_POSITIVE_Y_OFFSET		11.350f
#define GRAVITY_SENSOR_NEGATIVE_Y_OFFSET		-9.365f
#define GRAVITY_SENSOR_ZERO_Y_OFFSET			0.981f
#define GRAVITY_SENSOR_POSITIVE_Z_OFFSET		14.920f
#define GRAVITY_SENSOR_NEGATIVE_Z_OFFSET		-7.000f
#define GRAVITY_SENSOR_ZERO_Z_OFFSET			3.154f
#define GRAVITY_SENSOR_G						9.81f

	g = GRAVITY_SENSOR_G;
	cx.at_positive_gravity = GRAVITY_SENSOR_POSITIVE_X_OFFSET;
	cx.at_negative_gravity = GRAVITY_SENSOR_NEGATIVE_X_OFFSET;
	cx.at_zero_gravity = GRAVITY_SENSOR_ZERO_X_OFFSET;

	cy.at_positive_gravity = GRAVITY_SENSOR_POSITIVE_Y_OFFSET;
	cy.at_negative_gravity = GRAVITY_SENSOR_NEGATIVE_Y_OFFSET;
	cy.at_zero_gravity = GRAVITY_SENSOR_ZERO_Y_OFFSET;

	cz.at_positive_gravity = GRAVITY_SENSOR_POSITIVE_Z_OFFSET;
	cz.at_negative_gravity = GRAVITY_SENSOR_NEGATIVE_Z_OFFSET;
	cz.at_zero_gravity = GRAVITY_SENSOR_ZERO_Z_OFFSET;
	accel_cal = CAccelerationCalibration()->New(g, cx, cy, cz);
	if (!accel_cal) {
		IST_DBG("!accel_cal\n");
		goto EXIT;
	}

	event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!event) {
		goto EXIT;
	}

	dwCnt = 0;
	tmM = tmA = GetCurrentSeconds();
	dwTimeout = period;
    while (!g_pMain->bStopSignal) {
		xIST_DBG("dwTimeout = %u ms\n", dwTimeout);
		dwRet = WaitForSingleObject(event, dwTimeout);
		if (WAIT_TIMEOUT != dwRet) {
			xIST_DBG("dwRet = %u\n", dwRet);
			g_pMain->bStopSignal = true;
			continue;
		}

		dwStart = GetTickCount();

		/* get real gyroscope data */
		if (FALSE == ReadSensorData(gyro_sensor, &gx, &gy, &gz, CSensorBase::GYRO_UNITS::CONFIG_GYRO_UNITS_RAD)){
			g_pMain->bStopSignal = true;
			continue;
		}

		gs[0] = FloatType(gx);
		gs[1] = FloatType(gy);
		gs[2] = FloatType(gz);

		/* Get accelerator data */
		if (FALSE == ReadSensorData(accel_sensor, &ax, &ay, &az, CSensorBase::ACCEL_UNITS::CONFIG_ACCEL_UNITS_MPSS)){
			g_pMain->bStopSignal = true;
			continue;
		}
		if (accel_cal->Process(accel_cal, ax, ay, az, dT)) {
			accel_cal->GetData(accel_cal, &ax, &ay, &az);
		}
		gdata[0] = FloatType(ax);
		gdata[1] = FloatType(ay);
		gdata[2] = FloatType(az);

		xIST_DBG("a[%14.10f,%14.10f,%14.10f]\n", ax, ay, az);

		/* Get magnet field data */
		if (FALSE == ReadSensorData(magnet_sensor, &mx, &my, &mz, CSensorBase::MAG_UNITS::CONFIG_MAG_UNITS_UT)){
			g_pMain->bStopSignal = true;
			continue;
		}

		dT = EllapsedSeconds(&tmM);
		mdata[0] = FloatType(mx);
		mdata[1] = FloatType(my);
		mdata[2] = FloatType(mz);

		xIST_DBG("m[%14.10f,%14.10f,%14.10f]\n", mx, my, mz);
#ifndef MAGNET_VELOCITY
		rotation_done = rotation->Process(rotation, mdata, gdata, FloatType(dT));
#endif
		magnet_done = magnet->Process(magnet, mdata, FloatType(dT));

#ifndef MAGNET_VELOCITY
		/* check valid */
		xIST_DBG("rotation_done:%s\n", rotation_done ? "YES" : "NO");
#endif
		xIST_DBG("magnet_done:%s\n", magnet_done ? "YES" : "NO");
#ifndef MAGNET_VELOCITY
		if (rotation_done && magnet_done) {
#else
		if (magnet_done) {
#endif
			CString tmpStr;
			if (g_pMain->GetCurrentPage() == CiSensorsDlg::GYROSCOPE_DATA) {
				xIST_DBG("update gyro\n");
#ifdef MAGNET_VELOCITY
				ws[0] = magnet->Velocity[0];
				ws[1] = magnet->Velocity[1];
				ws[2] = magnet->Velocity[2];
#else
				ws[0] = rotation->Velocity[0];
				ws[1] = rotation->Velocity[1];
				ws[2] = rotation->Velocity[2];
#endif
				xIST_DBG("%14.10f,%14.10f,%14.10f,%14.10f,%14.10f,%14.10f\n", gx, gy, gz, 
					(float)NativeType(ws[0]), (float)NativeType(ws[1]), (float)NativeType(ws[2]));
				for (int i = 0; i < 3; ++i) {
					tmpStr.Format(_T("%0.2f"), (float)NativeType(ws[i]));
					g_pMain->m_SubDlgGyroscopeData.GetDlgItem(IDC_STATIC_EGYROSCOPE_X + i)->SetWindowTextW(tmpStr);
					tmpStr.Format(_T("%0.2f"), (float)NativeType(gs[i]));
					g_pMain->m_SubDlgGyroscopeData.GetDlgItem(IDC_STATIC_GYROSCOPE_X + i)->SetWindowTextW(tmpStr);
					g_pMain->m_SubDlgGyroscopeData.m_GyroWaveform[i].SetData(
						(float)NativeType(gs[0]), (float)NativeType(gs[1]), (float)NativeType(gs[2]));
					g_pMain->m_SubDlgGyroscopeData.m_SoftGyroWaveform[i].SetData(
						(float)NativeType(ws[0]), (float)NativeType(ws[1]), (float)NativeType(ws[2]));
				}
			}
		}

        dwEnd = GetTickCount();
		if (period > (dwEnd - dwStart))
			dwTimeout = period - (dwEnd - dwStart);
		else
			dwTimeout = 0;
    }

EXIT:
	if (event) {
		CloseHandle(event);
		event = NULL;
	}

#ifndef MAGNET_VELOCITY
	if (rotation) {
		IST_Rotation()->Delete(rotation);
		rotation = NULL;
	}
#endif

	if (magnet) {
		IST_Magnet()->Delete(magnet);
		magnet = NULL;
	}

	DestroySensor(&gyro_sensor);
	DestroySensor(&magnet_sensor);
	DestroySensor(&accel_sensor);

	FreezeGUI(true);

	IST_DBG("--- Leave SensorFusionProc() ---\n");

	ExitThread(0);
}
