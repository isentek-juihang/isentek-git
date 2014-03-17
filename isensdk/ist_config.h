#ifndef IST_CONFIG_H_INCLUDED
#define IST_CONFIG_H_INCLUDED

//** ------------------------------------------------------------------------
//** Force to support the platform with newlib/libc configurations
//** Define IST_CONFIG_FORCE_LIBC=1, native floating point
//** ------------------------------------------------------------------------
//#define IST_CONFIG_FORCE_LIBC 1

//** ------------------------------------------------------------------------
//** Floating point configurations
//** Define IST_CONFIG_NFPU=1, native floating point
//** Define IST_CONFIG_SFPU=1, software IEEE754 floating simulation
//** Define IST_CONFIG_QFPU, support Q format and IEEE754 floating
//** If no configure option set, IST_CONFIG_NFPU is default.
//** ------------------------------------------------------------------------
#define IST_CONFIG_QFPU 1

//** ------------------------------------------------------------------------
//** Debug configurations
//** Define IST_CONFIG_NDEBUG=1, no debug info.
//** Define IST_CONFIG_MEMDEBUG=1, calculating memory usage
//** ------------------------------------------------------------------------
//#define IST_CONFIG_NDEBUG 1
#define IST_CONFIG_DEBUG_MEMORY 1

//** ------------------------------------------------------------------------
//** Fast inverse sqrt method in IEEE754 simulation
//** Define IST_CONFIG_FAST_INVERSE_SQRT=1, support fast inverse sqrt
//** ------------------------------------------------------------------------
//#define IST_CONFIG_FAST_INVERSE_SQRT 1

//** ------------------------------------------------------------------------
//** Calculating leading zero number by lookup table
//** Define IST_CONFIG_CLZ_TABLE=1, support lookup table
//** ------------------------------------------------------------------------
#define IST_CONFIG_CLZ_TABLE 1

//** ------------------------------------------------------------------------
//** Hardiron calibration parameters
//** Define IST_CONFIG_HARDIRON_DATA_BUFFER_SIZE=N, N is hardiron buffering data 
//** number. MUST greater than 4.
//** Define IST_CONFIG_HARDIRON_OFFSET_BUFFER_SIZE=N, N is hardiron buffering 
//** offset value number
//** ----------------------------------------------------------------------
#define IST_CONFIG_HARDIRON_DATA_BUFFER_SIZE 20
#define IST_CONFIG_HARDIRON_OFFSET_BUFFER_SIZE 4

//** ------------------------------------------------------------------------
//** Softiron calibration parameters
//** Define IST_CONFIG_SOFTIRON_REALTIME=1, enable realtime softiron calibration
//** Define IST_CONFIG_SOFTIRON_DATA_MIN=N, N is min of num of data which is enough 
//** to active softiron calibration. Default is 24.
//** Define IST_CONFIG_SOFTIRON_DATA_MAX=1, N is max of num of data buffered to do
//** softiron calibration. Default is 120.
//** Define IST_CONFIG_SOFTIRON_FREQ=N, N is frequency(Hz) of magnetometer calibration
//** Define IST_CONFIG_SOFTIRON_UPDATE_TIME=N, N is time period(sec) of updating calibration
//** ----------------------------------------------------------------------
#define IST_CONFIG_SOFTIRON_REALTIME 1
#define IST_CONFIG_SOFTIRON_QUAD 3
#define IST_CONFIG_SOFTIRON_DATA_MIN 12
#define IST_CONFIG_SOFTIRON_DATA_MAX 27
#define IST_CONFIG_SOFTIRON_FREQ 50
#define IST_CONFIG_SOFTIRON_UPDATE_TIME 30

#endif // IST_CONFIG_H_INCLUDED
