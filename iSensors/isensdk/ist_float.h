#ifndef IST_FLOAT_H_INCLUDED
#define IST_FLOAT_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

//** ------------------------------------------------------------------------
//** Single-precision conversion
//** ------------------------------------------------------------------------
#define int_to_float(_x_) ((ISTFLOAT)(_x_))
#define float_to_int(_x_) ((ISTINT)(_x_))

//** ------------------------------------------------------------------------
//** Single-precision operations
//** ------------------------------------------------------------------------
#define float_add(_x_,_y_) ((ISTFLOAT)((_x_)+(_y_)))
#define float_sub(_x_,_y_) ((ISTFLOAT)((_x_)-(_y_)))
#define float_mul(_x_,_y_) ((ISTFLOAT)((_x_)*(_y_)))
#define float_div(_y_,_x_) ((ISTFLOAT)((_y_)/(_x_)))
#define float_negate(_x_) ((ISTFLOAT)(-(_x_)))
#define float_eq(_x_,_y_) ((ISTBOOL)((_x_)==(_y_)))
#define float_le(_x_,_y_) ((ISTBOOL)((_x_)<=(_y_)))
#define float_lt(_x_,_y_) ((ISTBOOL)((_x_)<(_y_)))
#define float_one()	(1.0f)
#define float_max() (FLT_MAX)

#if defined(IST_OS_WIN32) || \
	defined(IST_OS_LINUX) || \
	defined(IST_OS_ANDROID)
#ifdef IST_OS_WIN32
#  define _USE_MATH_DEFINES
#endif // IST_OW_WIN32
#  include <math.h>
#  include <float.h>
#  define float_sqrt sqrtf
#  define float_abs fabsf
#  define float_eps() (FLT_EPSILON)
#  define float_copysign(_x_,_y_) ((ISTFLOAT)_copysign((_x_),(_y_)))
#  define float_atan atanf
#  define float_atan2 atan2f
#  ifdef IST_FLOOR
#    define float_floor floorf
#  endif // IST_FLOOR
#  ifdef IST_EXP
#    define float_exp expf
#  endif // IST_EXP
#  ifdef IST_TAN
#    define float_tan tanf
#  endif // IST_TAN
#  ifdef IST_SIN
#    define float_sin sinf
#  endif // IST_SIN
#  ifdef IST_COS
#    define float_cos cosf
#  endif // IST_COS
#  ifdef IST_ASIN
#    define float_asin asinf
#  endif // IST_ASIN
#  ifdef IST_ACOS
#    define float_acos acosf
#  endif // IST_ACOS
#  define float_pi() ((ISTFLOAT)M_PI)
#  define float_pi_2() ((ISTFLOAT)M_PI_2)
#  define float_pi_4() ((ISTFLOAT)M_PI_4)
#  define float_pi_3_4() ((ISTFLOAT)(M_PI_2 + M_PI_4))
#else
#  error "Do not define sqrt function."
#endif

IST_EXTERN_C_END

#endif // IST_FLOAT_H_INCLUDED
