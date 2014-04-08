#ifndef IST_CONFIG_H_INCLUDED
#define IST_CONFIG_H_INCLUDED

//** ------------------------------------------------------------------------
//** Force to support the platform with newlib/libc configurations
//**   IST_CONFIG_FORCE_LIBC=1, native floating point
//** ------------------------------------------------------------------------
//#define IST_CONFIG_FORCE_LIBC 1

//** ------------------------------------------------------------------------
//** Floating point configurations
//**   IST_CONFIG_NFPU=1, native floating point
//**   IST_CONFIG_SFPU=1, software IEEE754 floating simulation
//**   IST_CONFIG_QFPU, support Q format and IEEE754 floating
//** If no configure option set, IST_CONFIG_NFPU is default.
//** ------------------------------------------------------------------------
#define IST_CONFIG_NFPU 1

//** ------------------------------------------------------------------------
//** Debug configurations
//**   IST_CONFIG_NDEBUG=1, no debug info.
//**   IST_CONFIG_MEMDEBUG=1, calculating memory usage
//** ------------------------------------------------------------------------
//#define IST_CONFIG_NDEBUG 1
//#define IST_CONFIG_DEBUG_MEMORY 1

//** ------------------------------------------------------------------------
//** Fast inverse sqrt method in IEEE754 simulation
//**   IST_CONFIG_FAST_INVERSE_SQRT=1, support fast inverse sqrt
//** ------------------------------------------------------------------------
//#define IST_CONFIG_FAST_INVERSE_SQRT 1

//** ------------------------------------------------------------------------
//** Calculating leading zero number by lookup table
//**   IST_CONFIG_CLZ_TABLE=1, support lookup table
//** ------------------------------------------------------------------------
#define IST_CONFIG_CLZ_TABLE 1

//** ------------------------------------------------------------------------
//** Hardiron calibration parameters
//**   IST_CONFIG_HARDIRON=1, support hardiron calibration
//**   IST_CONFIG_HARDIRON_DATA_BUFFER_SIZE=N, N is hardiron buffering data 
//** number. MUST greater than 4.
//**   IST_CONFIG_HARDIRON_OFFSET_BUFFER_SIZE=N, N is hardiron buffering 
//** offset value number
//** ----------------------------------------------------------------------
#define IST_CONFIG_HARDIRON 1
#define IST_CONFIG_HARDIRON_DATA_BUFFER_SIZE 20
#define IST_CONFIG_HARDIRON_OFFSET_BUFFER_SIZE 4

//** ------------------------------------------------------------------------
//** Softiron calibration parameters
//**   IST_CONFIG_SOFTIRON, support softiron calibration
//**   IST_CONFIG_SOFTIRON_REALTIME=1, enable realtime softiron calibration
//**   IST_CONFIG_SOFTIRON_DATA_MIN=N, N is min of num of data which is enough 
//** to active softiron calibration. Default is 24.
//**   IST_CONFIG_SOFTIRON_DATA_MAX=1, N is max of num of data buffered to do
//** softiron calibration. Default is 120.
//**   IST_CONFIG_SOFTIRON_FREQ=N, N is frequency(Hz) of magnetometer calibration
//**   IST_CONFIG_SOFTIRON_UPDATE_TIME=N, N is time period(sec) of updating calibration
//** ----------------------------------------------------------------------
#define IST_CONFIG_SOFTIRON 1
#define IST_CONFIG_SOFTIRON_REALTIME 1
#define IST_CONFIG_SOFTIRON_QUAD 3
#define IST_CONFIG_SOFTIRON_DATA_MIN 12
#define IST_CONFIG_SOFTIRON_DATA_MAX 27
#define IST_CONFIG_SOFTIRON_FREQ 50
#define IST_CONFIG_SOFTIRON_UPDATE_TIME 30

//** ------------------------------------------------------------------------
//** Magnetic sensor
//**   IST_CONFIG_MAGNET=1, to build magnet sensor fusion
//**   IST_CONFIG_MAGNET_VELOCITY=1, support velocity computation
//**   IST_CONFIG_MAGNET_FILTERED=1, support filter data
//**   IST_CONFIG_MAGNET_DATA_ALMA=1, support magnet data ALMA filter
//**   IST_CONFIG_MAGNET_BIAS_ALMA=1, support magnet bias ALMA filter
//**   IST_CONFIG_MAGNET_RAD_ALMA=1, support magnet radius ALMA filter
//**   IST_CONFIG_MAGNET_VELOCITY_ALMA=1, support magnet velocity ALMA filter
//**   IST_CONFIG_MAGNET_HZ=N, N is filtered frequency
//** ----------------------------------------------------------------------
#define IST_CONFIG_MAGNET 1
#define IST_CONFIG_MAGNET_VELOCITY 1
#define IST_CONFIG_MAGNET_FILTERED 1
//#define IST_CONFIG_MAGNET_DATA_ALMA 1
//#define IST_CONFIG_MAGNET_BIAS_ALMA 1
//#define IST_CONFIG_MAGNET_RAD_ALMA 1
#define IST_CONFIG_MAGNET_VELOCITY_ALMA 1
#define IST_CONFIG_MAGNET_HZ 50

//** ------------------------------------------------------------------------
//** Acceleration sensor
//**   IST_CONFIG_ACCEL=1, to build acceleration sensor
//**   IST_CONFIG_ACCEL_FILTERED=1, support filter data
//**   IST_CONFIG_ACCEL_DATA_ALMA=1, support acceleration data ALMA filter
//**   IST_CONFIG_ACCEL_HZ=N, N is filtered frequency
//** ----------------------------------------------------------------------
#define IST_CONFIG_ACCEL 1
//#define IST_CONFIG_ACCEL_FILTERED 1
//#define IST_CONFIG_ACCEL_DATA_ALMA 1
//#define IST_CONFIG_ACCEL_HZ 50

//** ------------------------------------------------------------------------
//** Rotation sensor
//**   IST_CONFIG_ROTATION=1, to build acceleration sensor
//**   IST_CONFIG_ROTATION_VELOCITY=1, support velocity computation
//**   IST_CONFIG_ROTATION_FILTERED=1, support filter data
//**   IST_CONFIG_ROTATION_MAGNET_ALMA=1, support magnet data ALMA filter
//**   IST_CONFIG_ROTATION_ACCEL_ALMA=1, support acceleration data ALMA filter
//**   IST_CONFIG_ROTATION_ANGULAR_ALMA=1, support rotation angular ALMA filter
//**   IST_CONFIG_ROTATION_VELOCITY_ALMA=1, support rotation angular ALMA filter
//**   IST_CONFIG_ROTATION_HZ=N, N is filtered frequency
//**   IST_CONFIG_ROTATION_MATRIX=1, support angular calculation by rotation 
//** matrix method. If config is not set, it is by quaternion method.
//** ----------------------------------------------------------------------
//#define IST_CONFIG_ROTATION 1
//#define IST_CONFIG_ROTATION_VELOCITY 1
//#define IST_CONFIG_ROTATION_FILTERED 1
//#define IST_CONFIG_ROTATION_MAGNET_ALMA 1
//#define IST_CONFIG_ROTATION_ACCEL_ALMA 1
//#define IST_CONFIG_ROTATION_ANGULAR_ALMA 1
//#define IST_CONFIG_ROTATION_VELOCITY_ALMA 1
//#define IST_CONFIG_ROTATION_HZ 50
//#define IST_CONFIG_ROTATION_MATRIX 1

//** ------------------------------------------------------------------------
//** Filter support 
//**   IST_CONFIG_ALMA=1, support arnaud legoux moving average filter
//**   IST_CONFIG_SMA=1, support simple moving average filter
//**   IST_CONFIG_RMA=1, support rolling moving average filter
//**   IST_CONFIG_WMA=1, support weighted moving average filter
//**   IST_CONFIG_EMA=1, support exponential weighted moving average filter
//**   IST_CONFIG_MMA=1, support modified moving average filter
//**   IST_CONFIG_HMA=1, support hull moving average filter
//**   IST_CONFIG_BIQUAD_LPF=1, support Biquad low pass IIR filter
//** ----------------------------------------------------------------------
#define IST_CONFIG_ALMA 1
//#define IST_CONFIG_SMA 1
//#define IST_CONFIG_RMA 1
//#define IST_CONFIG_WMA 1
//#define IST_CONFIG_EMA 1
//#define IST_CONFIG_MMA 1
//#define IST_CONFIG_HMA 1
//#define IST_CONFIG_BIQUAD_LPF 1

//** ------------------------------------------------------------------------
//** Advanced mathematic support 
//**   IST_CONFIG_FLOOR=1, support floor math
//**   IST_CONFIG_EXP=1, support exp math
//**   IST_CONFIG_TAN=1, support tan math
//**   IST_CONFIG_SIN=1, support sin math
//**   IST_CONFIG_COS=1, support cos math
//**   IST_CONFIG_ASIN=1, support asin math
//**   IST_CONFIG_ACOS=1, support acos math
//** ----------------------------------------------------------------------
#define IST_CONFIG_FLOOR 1
#define IST_CONFIG_EXP 1
#define IST_CONFIG_TAN 1
#define IST_CONFIG_SIN 1
#define IST_CONFIG_COS 1
#define IST_CONFIG_ASIN 1
//#define IST_CONFIG_ACOS 1

//** ------------------------------------------------------------------------
//** FIFO buffer
//**   IST_CONFIG_FIFO=1, support FIFO buffer
//**   IST_CONFIG_FIFO_DESTROY_RESOURCE=1, FIFO will destroy all resource
//** of items while deleting the FIFO buffer
//** ----------------------------------------------------------------------
//#define IST_CONFIG_FIFO 1
//#define IST_CONFIG_FIFO_DESTROY_RESOURCE 1

#endif // IST_CONFIG_H_INCLUDED
