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
//** Hardiron calibration parameters
//** ----------------------------------------------------------------------
#define IST_HIRON_RAD_MIN (IST_MAG_MIN - 15)	// uT
#define IST_HIRON_RAD_MAX (IST_MAG_MAX + 15)	// uT
#define IST_HIRON_DBUF_MAX IST_CONFIG_HARDIRON_DATA_BUFFER_SIZE
#define IST_HIRON_ODBUF_MAX IST_CONFIG_HARDIRON_OFFSET_BUFFER_SIZE

//** ------------------------------------------------------------------------
//** Softiron calibration parameters
//** ----------------------------------------------------------------------
#ifdef IST_CONFIG_SOFTIRON_REALTIME
#  define IST_SIRON_REALTIME 1
#  define IST_ELLIPSOID 1
#endif
#define IST_SIRON_QUAD (IST_CONFIG_SOFTIRON_QUAD < 2 ? 2 : IST_CONFIG_SOFTIRON_QUAD)
#define IST_SIRON_DBUF_MAX (IST_SIRON_QUAD*IST_SIRON_QUAD*IST_SIRON_QUAD)
#define IST_SIRON_DATA_MIN (IST_CONFIG_SOFTIRON_DATA_MIN > IST_SIRON_DBUF_MAX ? IST_SIRON_DBUF_MAX : IST_CONFIG_SOFTIRON_DATA_MIN)
#define IST_SIRON_DATA_MAX (IST_CONFIG_SOFTIRON_DATA_MAX > IST_SIRON_DBUF_MAX ? IST_SIRON_DBUF_MAX : IST_CONFIG_SOFTIRON_DATA_MAX)
#define IST_SIRON_HZ IST_CONFIG_SOFTIRON_FREQ
#define IST_SIRON_UPDATE_HZ (IST_CONFIG_SOFTIRON_UPDATE_TIME*IST_SIRON_HZ)
#define IST_SIRON_RAD_MIN (IST_MAG_MIN - 15)	// uT
#define IST_SIRON_RAD_MAX (IST_MAG_MAX + 15)	// uT

#endif // IST_PREP_H_INCLUDED