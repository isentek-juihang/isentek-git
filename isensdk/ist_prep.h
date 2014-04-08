#ifndef IST_PREP_H_INCLUDED
#define IST_PREP_H_INCLUDED

#include "ist_config.h"

//** ------------------------------------------------------------------------
//** CPU predefines
//** ------------------------------------------------------------------------
#if defined(ARM) || defined(__arm__) || defined(_ARM) || defined(_M_ARM)
#  define IST_CPU_ARM 1
#  define IST_CPU_STRING "ARM"
#endif

#if defined(__X86__) || defined(__i386__) || defined(i386) || defined(_M_IX86) \
    || defined(__386__) || defined(__x86_64__) || defined(_M_X64)
#  define IST_CPU_X86 1
#  if defined(__x86_64__) || defined(_M_X64)
#     define IST_CPU_X86_64 1
#  endif
#  ifdef IST_CPU_X86_64
#     define IST_CPU_STRING "AMD x86-64"
#  else
#     define IST_CPU_STRING "Intel 386+"
#  endif
#endif

#ifndef IST_CPU_STRING
#  define IST_CPU_STRING "Unknown CPU"
#  define IST_CPU_NONE 1
#endif

//** ------------------------------------------------------------------------
//** Compiler predefines
//** ------------------------------------------------------------------------
#ifdef __GNUC__
#  define IST_COMPILER_STRING "Gnu GCC"
#  define IST_COMPILER_GCC 1
#endif

#ifdef _MSC_VER
#  define IST_COMPILER_STRING "Microsoft Visual C++"
#  define IST_COMPILER_MSVC 1
#endif

#ifndef IST_COMPILER_STRING
#  define IST_COMPILER_STRING "Unknown compiler"
#  define IST_COMPILER_NONE 1
#endif

//** ------------------------------------------------------------------------
//** OS predefines
//** ------------------------------------------------------------------------
#if defined(IST_CONFIG_FORCE_LIBC)
#  define IST_OS_LIBC 1	// With newlib/libc environment 
#  define IST_OS_STRING "Libc OS"
#else
#  if defined(_WIN32) || defined(WIN32) || defined(__NT__) || defined(__WIN32__)
#    define IST_OS_WIN32 1
#    ifdef _WIN64
#      define IST_OS_WIN64 1
#    endif
#    ifdef IST_OS_WIN64
#      define IST_OS_STRING "Win64"
#    else
#      define IST_OS_STRING "Win32"
#    endif
#  endif
#  if defined(ANDROID) || defined(__ANDROID__)
#    define IST_OS_STRING "Android"
#    define IST_OS_ANDROID 1
#  endif
#  if defined(linux) || defined(__linux__)
#    define IST_OS_LINUX 1
#    define IST_OS_STRING "Linux"
#  endif
#endif

#ifndef IST_OS_STRING
#  define IST_OS_STRING "Unknown OS"
#  define IST_OS_NONE 1
#endif

//** ------------------------------------------------------------------------
//** Build options
//** ------------------------------------------------------------------------
//** ------------------------------------------------------------------------
//** Floating point type
//** ------------------------------------------------------------------------
#if defined(IST_CONFIG_NFPU) && defined(IST_CONFIG_SFPU)
#  error "Floating types conflict."
#elif !defined(IST_CONFIG_NFPU) && !defined(IST_CONFIG_SFPU)
#  ifdef IST_CONFIG_QFPU
#    define IST_CONFIG_SFPU 1
#  else
#    define IST_CONFIG_NFPU 1
#  endif
#endif

#ifdef IST_CONFIG_NFPU
#  define IST_FLOAT_NATIVE 1
#endif

#ifdef IST_CONFIG_SFPU
#  define IST_FLOAT_IEEE754 1
#endif

#ifdef IST_CONFIG_QFPU
#  define IST_FLOAT_QFT 1
#  define IST_QFT_N 8
#  define IST_QFT_M (32-IST_QFT_N)
#  define IST_QFT_ODD ((IST_QFT_N & 1) == 1)
#  define IST_QFT_MAX ((ISTFLOAT)(((ISTUBIT32)-1)>>1))
#  define IST_QFT_MIN ((ISTFLOAT)(-1<<31))
#  define IST_QFT_INF IST_QFT_MAX
#endif

//** ------------------------------------------------------------------------
//** Debug
//** ------------------------------------------------------------------------
#ifndef IST_CONFIG_NDEBUG
#  define IST_DEBUG 1
#  ifdef IST_OS_WIN32
#    define IST_DBG_BUF_SIZE 1024
#    define IST_DBG_CONSOLE 1
#  endif
#  ifdef IST_CONFIG_DEBUG_MEMORY
#    define IST_DEBUG_MEM 1
#  endif
#endif

//** ------------------------------------------------------------------------
//** Using fast inverse sqrt method in IEEE754 FPU simulation
//** ------------------------------------------------------------------------
#ifdef IST_CONFIG_SFPU
#  ifdef IST_CONFIG_FAST_INVERSE_SQRT
#    define IST_ENABLE_FAST_INVERSE_SQRT 1
#  endif
#endif

//** ------------------------------------------------------------------------
//** Using lookup table clz in Q format simulation
//** ------------------------------------------------------------------------
#ifdef IST_CONFIG_CLZ_TABLE
#  define IST_CLZ_TABLE 1
#endif

//** ------------------------------------------------------------------------
//** Magnetic field parameters (Unit: uT, 1 T = 10000 Gauss)
//** ------------------------------------------------------------------------
#define IST_MAG_MIN	20
#define IST_MAG_MAX	100

//** ------------------------------------------------------------------------
//** Rotation sensor parameters
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_ROTATION
#  ifndef IST_CONFIG_SIN
#    define IST_CONFIG_SIN 1
#  endif
#  ifndef IST_CONFIG_COS
#    define IST_CONFIG_COS 1
#  endif
#endif

#ifdef IST_CONFIG_ROTATION
#  define IST_ROTATION 1
#  ifdef IST_CONFIG_ROTATION_FILTERED
#    define IST_ROTATION_FILTERED 1
#  endif
#  ifdef IST_CONFIG_ROTATION_VELOCITY
#    define IST_ROTATION_VELOCITY 1
#  endif
#  ifndef IST_CONFIG_MAGNET
#    define IST_CONFIG_MAGNET 1
#  endif
#  ifndef IST_CONFIG_ACCEL
#    define IST_CONFIG_ACCEL 1
#  endif
#endif

#ifdef IST_CONFIG_ROTATION_FILTERED
#  ifdef IST_CONFIG_ROTATION_HZ
#    define IST_ROTATION_HZ IST_CONFIG_ROTATION_HZ
#  else
#    define IST_ROTATION_HZ 50
#  endif
#  ifdef IST_CONFIG_MAGNET_HZ
#    undef IST_CONFIG_MAGNET_HZ
#    define IST_CONFIG_MAGNET_HZ IST_ROTATION_HZ
#  endif
#  ifdef IST_CONFIG_MAGNET_HZ
#    undef IST_CONFIG_MAGNET_HZ
#    define IST_CONFIG_MAGNET_HZ IST_ROTATION_HZ
#  endif
#endif

#ifdef IST_CONFIG_ROTATION_FILTERED
#  ifndef IST_CONFIG_MAGNET_FILTERED
#    define IST_CONFIG_MAGNET_FILTERED 1
#  endif
#  ifdef IST_CONFIG_ROTATION_MAGNET_ALMA
#    ifndef IST_CONFIG_MAGNET_DATA_ALMA
#      define IST_CONFIG_MAGNET_DATA_ALMA 1
#    endif
#    ifndef IST_CONFIG_MAGNET_BIAS_ALMA
#      define IST_CONFIG_MAGNET_BIAS_ALMA 1
#    endif
#    ifndef IST_CONFIG_MAGNET_RADIUS_ALMA
#      define IST_CONFIG_MAGNET_RADIUS_ALMA 1
#    endif
#  endif
#endif

#ifdef IST_CONFIG_ROTATION_FILTERED
#  ifndef IST_CONFIG_ACCEL_FILTERED
#    define IST_CONFIG_ACCEL_FILTERED 1
#  endif
#  ifdef IST_CONFIG_ROTATION_ACCEL_ALMA
#    ifndef IST_CONFIG_ACCEL_DATA_ALMA
#      define IST_CONFIG_ACCEL_DATA_ALMA 1
#    endif
#  endif
#endif

#ifdef IST_CONFIG_ROTATION_FILTERED
#  ifdef IST_CONFIG_ROTATION_ANGULAR_ALMA
#    define IST_ROTATION_ANGULAR_FILTERED 1
#    define IST_ROTATION_ANGULAR_ALMA 1
#    define IST_ROTATION_ANGULAR_ALMA_PERIOD 11
#    ifndef IST_CONFIG_ALMA
#      define IST_CONFIG_ALMA 1
#    endif
#  endif
#endif

#ifdef IST_CONFIG_ROTATION_FILTERED
#  ifdef IST_CONFIG_ROTATION_VELOCITY
#    ifdef IST_CONFIG_ROTATION_VELOCITY_ALMA
#      define IST_ROTATION_VELOCITY_FILTERED 1
#      define IST_ROTATION_VELOCITY_ALMA 1
#      define IST_ROTATION_VELOCITY_ALMA_PERIOD 11
#      ifndef IST_CONFIG_ALMA
#        define IST_CONFIG_ALMA 1
#      endif
#    endif
#  endif
#endif

#ifdef IST_CONFIG_ROTATION_MATRIX
#  define IST_ROTATION_MATRIX 1
#else
#  define IST_ROTATION_QUATERNION 1
#  ifndef IST_CONFIG_ASIN
#    define IST_CONFIG_ASIN 1
#  endif
#endif

//** ------------------------------------------------------------------------
//** Magnet sensor parameters
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_MAGNET
#  define IST_MAGNET 1
#  ifndef IST_CONFIG_HARDIRON
#    define IST_CONFIG_HARDIRON 1
#  endif
#  ifdef IST_CONFIG_MAGNET_FILTERED
#    define IST_MAGNET_FILTERED 1
#  endif
#  ifdef IST_CONFIG_MAGNET_VELOCITY
#    define IST_MAGNET_VELOCITY 1
#  endif
#endif

#ifdef IST_CONFIG_MAGNET_FILTERED
#  ifdef IST_CONFIG_MAGNET_HZ
#    define IST_MAGNET_HZ IST_CONFIG_MAGNET_HZ
#  else
#    ifdef IST_CONFIG_SOFTIRON
#      define IST_MAGNET_HZ IST_CONFIG_SOFTIRON_FREQ
#    else
#      define IST_MAGNET_HZ 50
#    endif
#  endif
#endif

#ifdef IST_CONFIG_MAGNET_FILTERED
#  ifdef IST_CONFIG_MAGNET_DATA_ALMA
#    define IST_MAGNET_DATA_FILTERED 1
#    define IST_MAGNET_DATA_ALMA 1
#    define IST_MAGNET_DATA_ALMA_PERIOD 11
#    ifndef IST_CONFIG_ALMA
#      define IST_CONFIG_ALMA 1
#    endif
#  endif
#endif

#ifdef IST_CONFIG_MAGNET_FILTERED
#  ifdef IST_CONFIG_MAGNET_BIAS_ALMA
#    define IST_MAGNET_BIAS_FILTERED 1
#    define IST_MAGNET_BIAS_ALMA 1
#    define IST_MAGNET_BIAS_ALMA_PERIOD 11
#    ifndef IST_CONFIG_ALMA
#      define IST_CONFIG_ALMA 1
#    endif
#  endif
#endif

#ifdef IST_CONFIG_MAGNET_FILTERED
#  ifdef IST_CONFIG_MAGNET_RAD_ALMA
#    define IST_MAGNET_RAD_FILTERED 1
#    define IST_MAGNET_RAD_ALMA 1
#    define IST_MAGNET_RAD_ALMA_PERIOD 11
#    ifndef IST_CONFIG_ALMA
#      define IST_CONFIG_ALMA 1
#    endif
#  endif
#endif

#ifdef IST_CONFIG_MAGNET_FILTERED
#  ifdef IST_CONFIG_MAGNET_VELOCITY
#    ifdef IST_CONFIG_MAGNET_VELOCITY_ALMA
#      define IST_MAGNET_VELOCITY_FILTERED 1
#      define IST_MAGNET_VELOCITY_ALMA 1
#      define IST_MAGNET_VELOCITY_ALMA_PERIOD 11
#      ifndef IST_CONFIG_ALMA
#        define IST_CONFIG_ALMA 1
#      endif
#    endif
#  endif
#endif

//** ------------------------------------------------------------------------
//** Acceleration sensor parameters
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_ACCEL
#  define IST_ACCEL 1
#  ifdef IST_CONFIG_ACCEL_FILTERED
#    define IST_ACCEL_FILTERED 1
#  endif
#endif

#ifdef IST_CONFIG_ACCEL_FILTERED
#  ifdef IST_CONFIG_ACCEL_HZ
#    define IST_ACCEL_HZ IST_CONFIG_ACCEL_HZ
#  else
#    define IST_ACCEL_HZ 50
#  endif
#endif

#ifdef IST_CONFIG_ACCEL_FILTERED
#  ifdef IST_CONFIG_ACCEL_DATA_ALMA
#    define IST_ACCEL_DATA_FILTERED 1
#    define IST_ACCEL_DATA_ALMA 1
#    ifndef IST_CONFIG_ALMA
#      define IST_CONFIG_ALMA 1
#    endif
#  endif
#endif

//** ------------------------------------------------------------------------
//** Filters
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_ALMA
#  define IST_ALMA 1
#  define IST_ALMA_PERIOD 21
#  define IST_ALMA_OFFSET (_frac(1,10))
#  define IST_ALMA_SIGMA 6
#  ifndef IST_CONFIG_FLOOR
#    define IST_CONFIG_FLOOR 1
#  endif
#  ifndef IST_CONFIG_EXP
#    define IST_CONFIG_EXP 1
#  endif
#endif

#ifdef IST_CONFIG_SMA 
#  define IST_SMA 1
#  define IST_SMA_PERIOD 21
#endif

#ifdef IST_CONFIG_MMA
#  define IST_MMA 1
#  define IST_MMA_WEIGHT _one
#  define IST_MMA_PERIOD 21
#endif

#ifdef IST_CONFIG_RMA 
#  define IST_RMA 1
#  define IST_RMA_WEIGHT _one
#  define IST_RMA_PERIOD (-1)
#endif

#ifdef IST_CONFIG_WMA 
#  define IST_WMA 1
#  define IST_WMA_PERIOD 1
#endif

#ifdef IST_CONFIG_EMA
#  define IST_EMA 1
#endif

#ifdef IST_CONFIG_HMA 
#  define IST_HMA 1
#endif

#ifdef IST_CONFIG_BUTTERWORTH_LPF 
#  define IST_BTW_LPF 1
#endif

#ifdef IST_CONFIG_BIQUAD_LPF 
#  define IST_BIQUAD_LPF 1
#  define IST_BIQUAD_LPF_Q _div(_one,_sqrt(_float(2)))
#  define IST_BIQUAD_LPF_FC _frac(3,2)
#  define IST_BIQUAD_LPF_DT _frac(1,50)
#  ifndef IST_CONFIG_TAN
#    define IST_CONFIG_TAN 1
#  endif
#endif

//** ------------------------------------------------------------------------
//** Math functions
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_FLOOR
#  define IST_FLOOR 1
#endif

#ifdef IST_CONFIG_EXP
#  define IST_EXP 1
#endif

#ifdef IST_CONFIG_TAN
#  define IST_TAN 1
#endif

#ifdef IST_CONFIG_SIN
#  define IST_SIN 1
#endif

#ifdef IST_CONFIG_COS
#  define IST_COS 1
#endif

#ifdef IST_CONFIG_ASIN
#  define IST_ASIN 1
#endif

#ifdef IST_CONFIG_ACOS
#  define IST_ACOS 1
#endif

//** ------------------------------------------------------------------------
//** FIFO buffer
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_FIFO
#  define IST_FIFO 1
#  define IST_FIFO_TYPE E_FIFO_FLOAT	// float
#  ifdef IST_CONFIG_FIFO_DESTROY_RESOURCE
#    define IST_FIFO_DESTROY 1
#  endif
#endif // IST_CONFIG_FIFO

//** ------------------------------------------------------------------------
//** Softiron calibration parameters
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_SOFTIRON
#  define IST_SIRON 1
#  ifndef IST_CONFIG_HARDIRON
#    define IST_CONFIG_HARDIRON 1
#  endif
#  ifdef IST_CONFIG_SOFTIRON_REALTIME
#    define IST_SIRON_REALTIME 1
#    define IST_ELLIPSOID 1
#  endif
#  define IST_SIRON_QUAD (IST_CONFIG_SOFTIRON_QUAD < 2 ? 2 : IST_CONFIG_SOFTIRON_QUAD)
#  define IST_SIRON_DBUF_MAX (IST_SIRON_QUAD*IST_SIRON_QUAD*IST_SIRON_QUAD)
#  define IST_SIRON_DATA_MIN (IST_CONFIG_SOFTIRON_DATA_MIN > IST_SIRON_DBUF_MAX ? IST_SIRON_DBUF_MAX : IST_CONFIG_SOFTIRON_DATA_MIN)
#  define IST_SIRON_DATA_MAX (IST_CONFIG_SOFTIRON_DATA_MAX > IST_SIRON_DBUF_MAX ? IST_SIRON_DBUF_MAX : IST_CONFIG_SOFTIRON_DATA_MAX)
#  define IST_SIRON_HZ IST_CONFIG_SOFTIRON_FREQ
#  define IST_SIRON_UPDATE_HZ (IST_CONFIG_SOFTIRON_UPDATE_TIME*IST_SIRON_HZ)
#  define IST_SIRON_RAD_MIN (IST_MAG_MIN - 15)	// uT
#  define IST_SIRON_RAD_MAX (IST_MAG_MAX + 15)	// uT
#endif

//** ------------------------------------------------------------------------
//** Hardiron calibration parameters
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_HARDIRON
#  define IST_HIRON 1
#  define IST_SPHERE 1
#  define IST_HIRON_RAD_MIN (IST_MAG_MIN - 15)	// uT
#  define IST_HIRON_RAD_MAX (IST_MAG_MAX + 15)	// uT
#  define IST_HIRON_DBUF_MAX IST_CONFIG_HARDIRON_DATA_BUFFER_SIZE
#  define IST_HIRON_ODBUF_MAX IST_CONFIG_HARDIRON_OFFSET_BUFFER_SIZE
#endif

#endif // IST_PREP_H_INCLUDED