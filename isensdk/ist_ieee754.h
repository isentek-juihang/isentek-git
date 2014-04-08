#ifndef IST_IEEE754_H_INCLUDED
#define IST_IEEE754_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

//** ------------------------------------------------------------------------
//** Single-precision conversion
//** ------------------------------------------------------------------------
ISTFLOAT int_to_float(ISTINT);
ISTINT float_to_int(ISTFLOAT);

//** ------------------------------------------------------------------------
//** Single-precision operations
//** ------------------------------------------------------------------------
ISTFLOAT float_add(ISTFLOAT, ISTFLOAT);
ISTFLOAT float_sub(ISTFLOAT, ISTFLOAT);
ISTFLOAT float_mul(ISTFLOAT, ISTFLOAT);
ISTFLOAT float_div(ISTFLOAT, ISTFLOAT);
#define float_negate(x) ((ISTFLOAT)((x) ^ 0x80000000))
ISTFLOAT float_sqrt(ISTFLOAT);
ISTBOOL float_eq(ISTFLOAT, ISTFLOAT);
ISTBOOL float_le(ISTFLOAT, ISTFLOAT);
ISTBOOL float_lt(ISTFLOAT, ISTFLOAT);
ISTFLOAT float_abs(ISTFLOAT);
#define float_eps() ((ISTFLOAT)(0x34000000))
ISTFLOAT float_copysign(ISTFLOAT, ISTFLOAT);
ISTFLOAT float_atan(ISTFLOAT);
ISTFLOAT float_atan2(ISTFLOAT, ISTFLOAT);
#ifdef IST_FLOOR
  ISTFLOAT float_floor(ISTFLOAT);
#endif // IST_FLOOR
#ifdef IST_EXP
  ISTFLOAT float_exp(ISTFLOAT);
#endif // IST_EXP
#ifdef IST_TAN
  ISTFLOAT float_tan(ISTFLOAT);
#endif // IST_TAN
#ifdef IST_SIN
  ISTFLOAT float_sin(ISTFLOAT);
#endif // SIT_SIN
#ifdef IST_COS
  ISTFLOAT float_cos(ISTFLOAT);
#endif // IST_COS
#ifdef IST_ASIN
  ISTFLOAT float_asin(ISTFLOAT);
#endif // IST_ASIN
#ifdef IST_ACOS
  ISTFLOAT float_acos(ISTFLOAT);
#endif // IST_ACOS
#define float_pi() ((ISTFLOAT)0x40490FDB)
#define float_pi_2() ((ISTFLOAT)0x3FC90FDB)
#define float_pi_4() ((ISTFLOAT)0x3F490FDB)
#define float_pi_3_4() ((ISTFLOAT)0x4016CBE4)
#define float_one() ((ISTFLOAT)0x3F800000)
#define float_max() ((ISTFLOAT)0x7F7FFFFF)

IST_EXTERN_C_END

#endif // IST_IEEE754_H_INCLUDED
