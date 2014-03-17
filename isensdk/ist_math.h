#ifndef IST_MATH_H_INCLUDED
#define IST_MATH_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"
#if defined(IST_FLOAT_NATIVE)
#  include "ist_float.h"
#elif defined(IST_FLOAT_IEEE754)
#  include "ist_ieee754.h"
#endif

IST_EXTERN_C_BEGIN

#define _add(_x_,_y_) float_add(_x_,_y_)
#define _sub(_x_,_y_) float_sub(_x_,_y_)
#define _mul(_x_,_y_) float_mul(_x_,_y_)
#define _div(_y_,_x_) float_div(_y_,_x_)
#define _neg(_x_) float_negate(_x_)
#define _sqrt(_x_) float_sqrt(_x_)
#define _eq(_x_,_y_) float_eq(_x_,_y_)
#define _neq(_x_,_y_) (!float_eq(_x_,_y_))
#define _le(_x_,_y_) float_le(_x_,_y_)
#define _lt(_x_,_y_) float_lt(_x_,_y_)
#define _ge(_x_,_y_) (!float_lt(_x_,_y_))
#define _gt(_x_,_y_) (!float_le(_x_,_y_))
#define _abs(_x_) float_abs(_x_)
#define _eps float_eps()
#define _pi float_pi()
#define _pi_2 float_pi_2()
#define _pi_4 float_pi_4()
#define _pi_3_4 float_pi_3_4()
#define _one float_one()
#define _max float_max()
#define _cpysgn(_x_,_y_) float_copysign(_x_,_y_)
#define _atan(_x_) float_atan(_x_)
#define _atan2(_y_,_x_) float_atan2(_y_,_x_)
#define _float(_x_) int_to_float(_x_)
#define _int(_x_) float_to_int(_x_)
#define _frac(_y_,_x_) float_div(int_to_float(_y_),int_to_float(_x_))
#define _add3(_x_0,_x_1,_x_2) _add(_add(_x_0,_x_1),_x_2)
#define _add4(_x_0,_x_1,_x_2,_x_3) _add(_add3(_x_0,_x_1,_x_2),_x_3)
#define _add5(_x_0,_x_1,_x_2,_x_3,_x_4) _add(_add4(_x_0,_x_1,_x_2,_x_3),_x_4)
#define _add6(_x_0,_x_1,_x_2,_x_3,_x_4,_x_5) _add(_add5(_x_0,_x_1,_x_2,_x_3,_x_4),_x_5)
#define _sub3(_x_0,_x_1,_x_2) _sub(_sub(_x_0,_x_1),_x_2)
#define _sub4(_x_0,_x_1,_x_2,_x_3) _sub(_sub3(_x_0,_x_1,_x_2),_x_3)
#define _sub5(_x_0,_x_1,_x_2,_x_3,_x_4) _sub(_sub4(_x_0,_x_1,_x_2,_x_3),_x_4)
#define _sub6(_x_0,_x_1,_x_2,_x_3,_x_4,_x_5) _sub(_sub5(_x_0,_x_1,_x_2,_x_3,_x_4),_x_5)
#define _mul3(_x_0,_x_1,_x_2)	_mul(_mul(_x_0,_x_1),_x_2)
#define _mul4(_x_0,_x_1,_x_2,_x_3) _mul(_mul3(_x_0,_x_1,_x_2),_x_3)
#define _mul5(_x_0,_x_1,_x_2,_x_3,_x_4) _mul(_mul4(_x_0,_x_1,_x_2,_x_3),_x_4)
#define _mul6(_x_0,_x_1,_x_2,_x_3,_x_4,_x_5) _mul(_mul5(_x_0,_x_1,_x_2,_x_3,_x_4),_x_5)
#define _div3(_x_0,_x_1,_x_2) _div(_div(_x_0,_x_1),_x_2)
#define _div4(_x_0,_x_1,_x_2,_x_3) _div(_div3(_x_0,_x_1,_x_2),_x_3)
#define _div5(_x_0,_x_1,_x_2,_x_3,_x_4) _div(_div4(_x_0,_x_1,_x_2,_x_3),_x_4)
#define _div6(_x_0,_x_1,_x_2,_x_3,_x_4,_x_5) _div(_div5(_x_0,_x_1,_x_2,_x_3,_x_4),_x_5)
#define _pow2(_x_) _mul(_x_,_x_)

IST_EXTERN_C_END

#endif // IST_MATH_H_INCLUDED
