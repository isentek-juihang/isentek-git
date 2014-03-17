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
#define float_one()	1.0f
#define float_max() FLT_MAX

#if defined(IST_OS_WIN32) || defined(IST_OS_LINUX)
#  include <math.h>
#  include <float.h>
#  define float_sqrt sqrtf
#  define float_abs fabsf
#  define float_eps() FLT_EPSILON
#  define float_copysign(_x_,_y_) ((ISTFLOAT)_copysign((_x_),(_y_)))
#  define float_atan atanf
#  define float_atan2 atan2f
#  define float_pi() M_PI
#  define float_pi_2() M_PI_2
#  define float_pi_4() M_PI_4
#  define float_pi_3_4() (M_PI_2 + M_PI_4)
#elif defined(IST_OS_ANDROID)
#  error "[ANDROID]Do not define sqrt function."
#else
#  error "Do not define sqrt function."
#endif

IST_EXTERN_C_END

#endif // IST_FLOAT_H_INCLUDED
