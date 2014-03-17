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
INLINE ISTFLOAT float_negate(ISTFLOAT x) { return ((ISTFLOAT)(x ^ 0x80000000)); }
ISTFLOAT float_sqrt(ISTFLOAT);
ISTBOOL float_eq(ISTFLOAT, ISTFLOAT);
ISTBOOL float_le(ISTFLOAT, ISTFLOAT);
ISTBOOL float_lt(ISTFLOAT, ISTFLOAT);
ISTFLOAT float_abs(ISTFLOAT);
INLINE ISTFLOAT float_eps() { return ((ISTFLOAT)(0x34000000)); }
ISTFLOAT float_copysign(ISTFLOAT, ISTFLOAT);
ISTFLOAT float_atan(ISTFLOAT);
ISTFLOAT float_atan2(ISTFLOAT, ISTFLOAT);
INLINE ISTFLOAT float_pi() { return (ISTFLOAT)0x40490FDB; }
INLINE ISTFLOAT float_pi_2() { return (ISTFLOAT)0x3FC90FDB; }
INLINE ISTFLOAT float_pi_4() { return (ISTFLOAT)0x3F490FDB; }
INLINE ISTFLOAT float_pi_3_4() { return (ISTFLOAT)0x4016CBE4; }
INLINE ISTFLOAT float_one() { return (ISTFLOAT)0x3F800000; }
INLINE ISTFLOAT float_max() { return (ISTFLOAT)0x7F7FFFFF; }
INLINE ISTFLOAT float_sqrt2() { return (ISTFLOAT)0x3FB504F3; }

IST_EXTERN_C_END

#endif // IST_IEEE754_H_INCLUDED
