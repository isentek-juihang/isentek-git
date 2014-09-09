#ifndef IST_TYPES_H_INCLUDED
#define IST_TYPES_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"

IST_EXTERN_C_BEGIN

//** ------------------------------------------------------------------------
//** Type definitions
//** ------------------------------------------------------------------------
#ifdef IST_OS_WIN32
  typedef signed char _bool_t;
  typedef char _char_t;
  typedef unsigned char _uchar_t;
  typedef signed char _schar_t;
  typedef short _short_t;
  typedef unsigned short _ushort_t;
  typedef signed short _sshort_t;
  typedef int _int_t;
  typedef unsigned int _uint_t;
  typedef signed int _sint_t;
#  if defined(IST_FLOAT_NATIVE)
     typedef float _float_t;
#  elif defined(IST_FLOAT_IEEE754)
     typedef unsigned int _float_t;
#  endif
#  ifdef IST_FLOAT_QFT
	 typedef signed int _qft_t;
#  else
#    if defined(IST_FLOAT_NATIVE)
	   typedef float _qft_t;
#    elif defined(IST_FLOAT_IEEE754)
	   typedef unsigned int _qft_t;
#    endif
#  endif
#elif defined(IST_OS_ANDROID)
  typedef signed char _bool_t;
  typedef char _char_t;
  typedef unsigned char _uchar_t;
  typedef signed char _schar_t;
  typedef short _short_t;
  typedef unsigned short _ushort_t;
  typedef signed short _sshort_t;
  typedef int _int_t;
  typedef unsigned int _uint_t;
  typedef signed int _sint_t;
#  if defined(IST_FLOAT_NATIVE)
     typedef float _float_t;
#  elif defined(IST_FLOAT_IEEE754)
     typedef unsigned int _float_t;
#  endif
#  ifdef IST_FLOAT_QFT
	 typedef signed int _qft_t;
#  else
#    if defined(IST_FLOAT_NATIVE)
	   typedef float _qft_t;
#    elif defined(IST_FLOAT_IEEE754)
	   typedef unsigned int _qft_t;
#    endif
#  endif
#else
  typedef signed char _bool_t;
  typedef char _char_t;
  typedef unsigned char _uchar_t;
  typedef signed char _schar_t;
  typedef short _short_t;
  typedef unsigned short _ushort_t;
  typedef signed short _sshort_t;
  typedef int _int_t;
  typedef unsigned int _uint_t;
  typedef signed int _sint_t;
#  if defined(IST_FLOAT_NATIVE)
     typedef float _float_t;
#  elif defined(IST_FLOAT_IEEE754)
     typedef unsigned int _float_t;
#  endif
#  ifdef IST_FLOAT_QFT
	 typedef signed int _qft_t;
#  else
#    if defined(IST_FLOAT_NATIVE)
	   typedef float _qft_t;
#    elif defined(IST_FLOAT_IEEE754)
	   typedef unsigned int _qft_t;
#    endif
#  endif
#endif

#define ISTBOOL _bool_t
#define ISTFLOAT _float_t
#define ISTCHAR _char_t
#define ISTSHORT _short_t
#define ISTINT _int_t
#define ISTUCHAR _uchar_t
#define ISTUSHORT _ushort_t
#define ISTUINT _uint_t
#define ISTQFT _qft_t

IST_COMPILE_ASSERT(check_ISTCHAR, sizeof(ISTCHAR)>=1);
IST_COMPILE_ASSERT(check_ISTSHORT, sizeof(ISTSHORT)>=2);
IST_COMPILE_ASSERT(check_ISTINT, sizeof(ISTINT)>=4);
IST_COMPILE_ASSERT(check_ISTUCHAR, sizeof(ISTUCHAR)>=1);
IST_COMPILE_ASSERT(check_ISTUSHORT, sizeof(ISTUSHORT)>=2);
IST_COMPILE_ASSERT(check_ISTUINT, sizeof(ISTUINT)>=4);
IST_COMPILE_ASSERT(check_ISTQFT, sizeof(ISTQFT)>=4);

//** ------------------------------------------------------------------------
//** Bits type definitions
//** ------------------------------------------------------------------------
#if defined(IST_OS_WIN32)
  typedef unsigned char _ubs8_t;
  typedef signed char _sbs8_t;
  typedef unsigned short _ubs16_t;
  typedef signed short _sbs16_t;
  typedef unsigned int _ubs32_t;
  typedef signed int _sbs32_t;
#elif defined(IST_OS_ANDROID)
  typedef unsigned char _ubs8_t;
  typedef signed char _sbs8_t;
  typedef unsigned short _ubs16_t;
  typedef signed short _sbs16_t;
  typedef unsigned int _ubs32_t;
  typedef signed int _sbs32_t;
#else
  typedef unsigned char _ubs8_t;
  typedef signed char _sbs8_t;
  typedef unsigned short _ubs16_t;
  typedef signed short _sbs16_t;
  typedef unsigned int _ubs32_t;
  typedef signed int _sbs32_t;
#endif

#define ISTSBIT8 _sbs8_t
#define ISTUBIT8 _ubs8_t
#define ISTSBIT16 _sbs16_t
#define ISTUBIT16 _ubs16_t
#define ISTSBIT32 _sbs32_t
#define ISTUBIT32 _ubs32_t

IST_COMPILE_ASSERT(check_ISTSBIT8, sizeof(ISTSBIT8)==1);
IST_COMPILE_ASSERT(check_ISTSBIT16, sizeof(ISTSBIT16)==2);
IST_COMPILE_ASSERT(check_ISTSBIT32, sizeof(ISTSBIT32)==4);
IST_COMPILE_ASSERT(check_ISTUBIT8, sizeof(ISTUBIT8)==1);
IST_COMPILE_ASSERT(check_ISTUBIT16, sizeof(ISTUBIT16)==2);
IST_COMPILE_ASSERT(check_ISTUBIT32, sizeof(ISTUBIT32)==4);

//** ------------------------------------------------------------------------
//** TRUE/FALSE/VOID/NULL definitions
//** ------------------------------------------------------------------------
#define ISTTRUE 1
#define ISTFALSE 0
#define ISTVOID void

#ifdef NULL
#  undef NULL
#  ifdef __cplusplus
#    define NULL 0
#  else
#    define NULL ((void *)0)
#  endif
#endif

//** ------------------------------------------------------------------------
//** Readable symbol definitions
//** ------------------------------------------------------------------------
#define IST_WRITE
#define IST_READ
#define IST_READWRITE

IST_EXTERN_C_END

#endif // IST_TYPES_H_INCLUDED
