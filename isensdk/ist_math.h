#ifndef IST_MATH_H_INCLUDED
#define IST_MATH_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

#ifdef IST_FLOAT_NATIVE
#  include "ist_float.h"
#endif // IST_FLOAT_NATIVE

#ifdef IST_FLOAT_IEEE754
#  include "ist_ieee754.h"
#endif // IST_FLOAT_IEEE754

#ifdef IST_FLOAT_QFT
#  include "ist_qft.h"
#else
#  define IST_SFP(_qft_) (_qft_)
#  define IST_QFP(_sft_) (_sft_)
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

#ifdef IST_FLOOR
#  define _floor(_x_) float_floor(_x_)
#endif // IST_FLOOR

#ifdef IST_EXP
#  define _exp(_x_) float_exp(_x_)
#endif // IST_EXP

#ifdef IST_TAN
#  define _tan(_x_) float_tan(_x_)
#endif // IST_TAN

#ifdef IST_SIN
#  define _sin(_x_) float_sin(_x_)
#endif // IST_SIN

#ifdef IST_COS
#  define _cos(_x_) float_cos(_x_)
#endif // IST_COS

#ifdef IST_ASIN
#  define _asin(_x_) float_asin(_x_)
#endif // IST_ASIN

#ifdef IST_ACOS
#  define _acos(_x_) float_acos(_x_)
#endif // IST_ACOS

INLINE ISTFLOAT _magnitude(CONST ISTFLOAT v[3])
{
	ISTFLOAT norm = _add3(_pow2(v[0]), _pow2(v[1]), _pow2(v[2]));
	return _sqrt(norm);
}

INLINE ISTVOID _cross_mul(CONST ISTFLOAT u[3], CONST ISTFLOAT v[3], ISTFLOAT res[3])
{
	res[0] = _sub(_mul(u[1], v[2]), _mul(u[2], v[1]));
	res[1] = _sub(_mul(u[2], v[0]), _mul(u[0], v[2]));
	res[2] = _sub(_mul(u[0], v[1]), _mul(u[1], v[0]));
}

INLINE ISTVOID _normalize(CONST ISTFLOAT v[3], ISTFLOAT res[3])
{
	ISTFLOAT mag = _magnitude(v);
	ISTFLOAT eps = _frac(1, 1000);
	ISTSHORT i;

	if (_gt(_abs(mag), eps) && _gt(_abs(_sub(mag, _one)), eps)) {
		for (i = 0; i < 3; ++i) {
			res[i] = _div(v[i], mag);
		}	
	} else {
		for (i = 0; i < 3; ++i) {
			res[i] = v[i];
		}	
	}
}

INLINE ISTFLOAT _dot_mul(CONST ISTFLOAT u[3], CONST ISTFLOAT v[3])
{
	ISTSHORT i;
	ISTFLOAT res = 0;

	for (i = 0; i < 3; ++i) {
		res = _add(res, _mul(u[i], v[i]));
	}
	return res;
}

INLINE ISTVOID _transport(CONST ISTFLOAT a[3][3], ISTFLOAT b[3][3])
{
	ISTSHORT i, j;

	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			b[i][j] = a[j][i];
		}
	}
}

INLINE ISTVOID _mat_mul(CONST ISTFLOAT a[3][3], CONST ISTFLOAT b[3][3], ISTFLOAT c[3][3])
{
	ISTSHORT i, j, k;

	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			c[i][j] = 0;
			for (k = 0; k < 3; ++k) 
				c[i][j] = _add(c[i][j], _mul(a[i][k], b[k][j]));
		}
	}
}

INLINE ISTFLOAT _magnitude_q(CONST ISTFLOAT q[4])
{
	ISTFLOAT norm = _add4(_pow2(q[3]), _pow2(q[0]), _pow2(q[1]), _pow2(q[2]));
	return _sqrt(norm);
}

INLINE ISTVOID _quaternion(CONST ISTFLOAT m[3][3], ISTFLOAT q[4])
{
	ISTFLOAT fq0sq;            // q0^2
	ISTFLOAT recip4q0;         // 1/4q0
	ISTFLOAT fmag;             // quaternion magnitude
	ISTFLOAT SMALLQ0 = _frac(1, 100); // limit where rounding errors may appear
	ISTSHORT i;

	// typically 18 floating point operations
	// get q0^2 and q0
	fq0sq = _div(_add4(_one, m[0][0], m[1][1], m[2][2]), _float(4));
	q[3] = _sqrt(_abs(fq0sq));
	// normal case when q0 is not small meaning rotation angle not near 180 deg 
	if (_gt(q[3], SMALLQ0)) {
		// calculate q1 to q3
		recip4q0 = _div(_one, _mul(_float(4), q[3]));
		q[0] = _mul(recip4q0, _sub(m[0][2], m[2][1])); 
		q[1] = _mul(recip4q0, _sub(m[2][0], m[0][2])); 
		q[2] = _mul(recip4q0, _sub(m[0][1], m[1][0]));
	} else {
		// special case of near 180 deg corresponds to nearly symmetric matrix
		// which is not numerically well conditioned for division by small q0
		// instead get absolute values of q1 to q3 from leading diagonal
		q[0] = _sqrt(_abs(_sub(_div(_add(_one, m[0][0]), _float(2)), fq0sq))); 
		q[1] = _sqrt(_abs(_sub(_div(_add(_one, m[1][1]), _float(2)), fq0sq))); 
		q[2] = _sqrt(_abs(_sub(_div(_add(_one, m[2][2]), _float(2)), fq0sq))); 
		// first assume q1 is positive and ensure q2 and q3 are consistent with q1
		if (_lt(_add(m[0][1], m[1][0]), 0)) {
			// q1*q2 < 0 so q2 is negative
			q[1] = _neg(q[1]);
			if (_gt(_add(m[1][2], m[2][1]), 0)) {
				// q1*q2 < 0 and q2*q3 > 0 so q3 is also both negative
				q[2] = _neg(q[2]);
			}
		} else if (_gt(_add(m[0][1], m[1][0]), 0)) {
			if (_lt(_add(m[1][2], m[2][1]), 0)) {
				// q1*q2 > 0 and q2*q3 < 0 so q3 is negative
				q[2] = _neg(q[2]);
			}
		}
		// negate the vector components if q1 should be negative
		if (_lt(_sub(m[1][2], m[2][0]), 0)) {
			for (i = 0; i < 3; ++i) {
				q[i] = _neg(q[i]);
			}
		}
	}
	// finally re-normalize for safety
	fmag = _magnitude_q(q);
	if (_gt(fmag, SMALLQ0)) {
		// normal case
		for (i = 0; i < 4; ++i) {
			q[i] = _div(q[i], fmag);
		}
	} else {
		// severe rounding errors so return identity quaternion
		q[3] = _one;
		for (i = 0; i < 3; ++i) {
			q[i] = 0;
		}
	}
}

#ifdef IST_ASIN
INLINE ISTVOID _angular_q(const ISTFLOAT q[4], ISTFLOAT angular[3])
{
	ISTFLOAT sqw = _pow2(q[3]);
	ISTFLOAT sqx = _pow2(q[0]);
	ISTFLOAT sqy = _pow2(q[1]);
	ISTFLOAT sqz = _pow2(q[2]);
	ISTFLOAT unit = _add4(sqx, sqy, sqz, sqw);
	ISTFLOAT test = _sub(_mul(q[1], q[3]), _mul(q[2], q[0]));
	ISTFLOAT th = _mul(_frac(499, 1000), unit);

	if (_gt(test, th)) { // singularity at north pole
		angular[0] = 0;
		angular[1] = _pi_2;
		angular[2] = _neg(_mul(_float(2), _atan2(q[1], q[3])));
	} else if (_lt(test, _neg(th))) { // singularity at south pole
		angular[0] = 0;
		angular[1] = _neg(_pi_2);
		angular[2] = _mul(_float(2), _atan2(q[1], q[3]));
	} else {
		angular[0] = _atan2(_mul(_float(2), _add(_mul(q[0], q[3]), _mul(q[1], q[2]))), 
			_add4(_neg(sqx), _neg(sqy), sqz, sqw));
		angular[1] = _asin(_div(_mul(_float(2), test), unit));
		angular[2] = _atan2(_mul(_float(2), _add(_mul(q[2], q[3]), _mul(q[0], q[1]))),
			_add4(sqx, _neg(sqy), _neg(sqz), sqw));
	}
}
#endif // IST_ASIN

INLINE ISTVOID _matrix_a(CONST ISTFLOAT angular[3], ISTFLOAT mat[3][3])
{
	ISTFLOAT sx;
	ISTFLOAT sy;
	ISTFLOAT sz;
	ISTFLOAT cx;
	ISTFLOAT cy;
	ISTFLOAT cz;

	sx = _sin(angular[0]);
	cx = _cos(angular[0]);
	sy = _sin(angular[1]);
	cy = _cos(angular[1]);
	sz = _sin(angular[2]);
	cz = _cos(angular[2]);
	/*!
	* R(r) = [ 1      0      0   ]  R(p) = [ cos(p)  0 -sin(p) ]  R(y) = [  cos(y)  sin(y)  0 ]
	*        [ 0  cos(r)  sin(r) ]         [   0     1    0    ]         [ -sin(y)  cos(y)  0 ]
	*        [ 0 -sin(r)  cos(r) ]         [ sin(p)  0  cos(p) ]         [    0       0     1 ]
	*        
	* IF s = sin, c = cos, 1 = r, 2 = p, 3 = y
	* 
	* R = R(r)R(p)R(y) = [      c2c3        c2s3      -s2  ]
	*                    [ s1s2c3-c1s3  s1s2s3+c1c3  s1c2  ]
	*                    [ c1s2c3+s1s3  c1s2s3-s1c3  c1c2  ]
	*/
	mat[0][0] = _mul(cy, cz);
	mat[0][1] = _mul(cy, sz);
	mat[0][2] = _neg(sy);
	mat[1][0] = _sub(_mul3(sx, sy, cz), _mul(cx, sz));
	mat[1][1] = _add(_mul3(sx, sy, sz), _mul(cx, cz));
	mat[1][2] = _mul(sx, cy);
	mat[2][0] = _add(_mul3(cx, sy, cz), _mul(sx, sz));
	mat[2][1] = _sub(_mul3(cx, sy, sz), _mul(sx, cz));
	mat[2][2] = _mul(cx, cy);
}

IST_EXTERN_C_END

#endif // IST_MATH_H_INCLUDED
