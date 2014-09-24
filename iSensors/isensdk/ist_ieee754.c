#include "ist_milieu.h"

#ifdef IST_FLOAT_IEEE754

#define _s float_sign
#define _e float_exponent
#define _m float_mantissa

enum {
	float_tininess_after_rounding = 0,
	float_tininess_before_rounding = 1
};

enum {
	float_round_nearest_even = 0,
	float_round_down = 1,
	float_round_up = 2,
	float_round_to_zero = 3
};

enum {
	float_flag_invalid =  1,
	float_flag_divbyzero = 4,
	float_flag_overflow = 8,
	float_flag_underflow = 16,
	float_flag_inexact = 32
};

enum {
	float_default_nan = 0xFFC00000,
	float_positive_infinity = 0x7F800000,
	float_negative_infinity = 0x8F800000
};

typedef struct {
	ISTSBIT8 sign;
	ISTUBIT32 high;
	ISTUBIT32 low;
} nan64;

ISTSBIT8 float_detect_tininess = float_tininess_after_rounding;
ISTSBIT8 float_rounding_mode = float_round_nearest_even;
ISTSBIT8 float_exception_flags = 0;

//** ------------------------------------------------------------------------
//** Raise exception
//** ------------------------------------------------------------------------
STATIC ISTVOID float_raise(ISTSBIT8 flags)
{
	float_exception_flags |= flags;
}

STATIC ISTUBIT32 float_mantissa(ISTFLOAT a)
{
	return (a & 0x007FFFFF);
}

STATIC ISTSBIT16 float_exponent(ISTFLOAT a)
{
	return (a >> 23) & 0xFF;
}

STATIC ISTSBIT8 float_sign(ISTFLOAT a)
{
	return (a >> 31);
}

STATIC ISTVOID shift_right_jamming(ISTUBIT32 a, ISTSBIT16 count, ISTUBIT32 *pz)
{
	ISTUBIT32 z;
	if (count == 0)
		z = a;
	else if (count < 32)
		z = (a >> count) | ((a << ((-count) & 31)) != 0);
	else
		z = (a != 0);
	*pz = z;
}

STATIC ISTVOID shift64_left(ISTUBIT32 a0, ISTUBIT32 a1, ISTSBIT16 count, ISTUBIT32 *pz0, ISTUBIT32 *pz1)
{
	*pz1 = a1 << count;
	*pz0 = (count == 0) ? a0 : (a0 << count) | (a1 >> ((-count) & 31));
}

STATIC ISTVOID add64(ISTUBIT32 a0, ISTUBIT32 a1, ISTUBIT32 b0, ISTUBIT32 b1, ISTUBIT32 *pz0, ISTUBIT32 *pz1)
{
	ISTUBIT32 z1 = a1 + b1;
	*pz1 = z1;
	*pz0 = a0 + b0 + (z1 < a1);
}

STATIC ISTVOID sub64(ISTUBIT32 a0, ISTUBIT32 a1, ISTUBIT32 b0, ISTUBIT32 b1, ISTUBIT32 *pz0, ISTUBIT32 *pz1)
{
	*pz1 = a1 - b1;
	*pz0 = a0 - b0 - (a1 < b1);
}

STATIC ISTVOID mul32to64(ISTUBIT32 a, ISTUBIT32 b, ISTUBIT32 *pz0, ISTUBIT32 *pz1)
{
	ISTUBIT16 ha = a >> 16;
	ISTUBIT16 la = a;
	ISTUBIT16 hb = b >>16;
	ISTUBIT16 lb = b;
	ISTUBIT32 z0 = ((ISTUBIT32)ha) * hb;
	ISTUBIT32 z1 = ((ISTUBIT32)la) * lb;
	ISTUBIT32 zma = ((ISTUBIT32)la) * hb;
	ISTUBIT32 zmb = ((ISTUBIT32)ha) * lb;

	zma += zmb;
	z0 += (((ISTUBIT32)(zma < zmb)) << 16) + (zma >> 16);
	zma <<= 16;
	z1 += zma;
	z0 += (z1 < zma);
	*pz1 = z1;
	*pz0 = z0;
}

STATIC ISTSBIT8 check_nan(ISTFLOAT a)
{
	return (0xFF000000 < (ISTUBIT32)(a << 1));
}

STATIC ISTSBIT8 check_signaling_nan(ISTFLOAT a)
{
	return (((a >> 22) & 0x1FF) == 0x1FE) && (a & 0x003FFFFF);
}

STATIC ISTFLOAT propagate_nan(ISTFLOAT a, ISTFLOAT b)
{
	ISTSBIT8 is_a_nan = check_nan(a);
	ISTSBIT8 is_a_snan = check_signaling_nan(a);
	ISTSBIT8 is_b_nan = check_nan(b);
	ISTSBIT8 is_b_snan = check_signaling_nan(b);

	a |= 0x00400000;
	b |= 0x00400000;
	if (is_a_snan | is_b_snan)
		float_raise(float_flag_invalid);
	if (is_a_snan) {
		if (is_b_snan)
			goto RETURN_LARGE_MANTISSA;
		return is_b_nan ? b : a;
	} else if (is_a_nan) {
		if (is_b_snan | (!is_b_nan))
			return a;
RETURN_LARGE_MANTISSA:
		if ((ISTUBIT32)(a << 1) < (ISTUBIT32)(b << 1))
			return b;
		if ((ISTUBIT32)(b << 1) < (ISTUBIT32)(a << 1))
			return a;
		return (a < b) ? a : b;
	} else
		return b;
}

STATIC ISTFLOAT pack(ISTSBIT8 sign, ISTSBIT16 exp, ISTUBIT32 mant)
{
	ISTFLOAT q;
	q = (((ISTUBIT32)sign) << 31) + (((ISTUBIT32)exp) << 23) + mant;
	return (q);
}

STATIC ISTUBIT32 div64to32(ISTUBIT32 a0, ISTUBIT32 a1, ISTUBIT32 b)
{
	ISTUBIT32 b0;
	ISTUBIT32 b1;
	ISTUBIT32 rem0;
	ISTUBIT32 rem1;
	ISTUBIT32 term0;
	ISTUBIT32 term1;
	ISTUBIT32 z;

	if (b <= a0)
		return 0xFFFFFFFF;
	b0 = b >> 16;
	z = (b0 << 16 <= a0) ? 0xFFFF0000 : (a0/b0) << 16;
	mul32to64(b, z, &term0, &term1);
	sub64(a0, a1, term0, term1, &rem0, &rem1);
	while (((ISTSBIT32)rem0) < 0) {
		z -= 0x10000;
		b1 = b << 16;
		add64(rem0, rem1, b0, b1, &rem0, &rem1);
	}
	rem0 = (rem0 << 16) | (rem1 >> 16);
	z |= (b0 << 16 <= rem0) ? 0xFFFF : (rem0/b0);
	return z;
}

STATIC ISTUBIT32 sqrt32(ISTSBIT16 exp, ISTUBIT32 a)
{
	STATIC CONST ISTSBIT16 sqrtoa[] = { /* square odd adjustment */
		0x0004, 0x0022, 0x005D, 0x00B1, 0x011D, 0x019F, 0x0236, 0x02E0,
		0x039C, 0x0468, 0x0545, 0x0631, 0x072B, 0x0832, 0x0946, 0x0A67
	};
	STATIC CONST ISTSBIT16 sqrtea[] = { /* square even adjustment */
		0x0A2D, 0x08AF, 0x075A, 0x0629, 0x051A, 0x0429, 0x0356, 0x029E,
		0x0200, 0x0179, 0x0109, 0x00AF, 0x0068, 0x0034, 0x0012, 0x0002
	};
	ISTSBIT8 index;
	ISTUBIT32 z;

	index = (a >> 27) & 15;
	if (exp & 1) {
		z = 0x4000 + (a >> 17) - sqrtoa[index];
		z = ((a/z) << 14) + (z << 15);
		a >>= 1;
	} else {
		z = 0x8000 + (a >> 17) - sqrtea[index];
		z = a/z + z;
		z = (0x20000 <= z) ? 0xFFFF8000 : (z << 15);
		if (z <= a) return (ISTUBIT32)(((ISTSBIT32)a) >> 1);
	}
	return ((div64to32(a, 0, z)) >> 1) + (z >> 1);
}

STATIC ISTSBIT8 clz(ISTUBIT32 a)
{
#ifdef IST_CLZ_TABLE
	STATIC CONST ISTSBIT8 clz_table[32] = {
		0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
		8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
	};
	a &= 0xffffffffU;
	if (!a) {
		return 32;
	}
	a |= a >> 1;
	a |= a >> 2;
	a |= a >> 4;
	a |= a >> 8;
	a |= a >> 16;
	return 31 - clz_table[(a * 0x07c4acddU) >> 27];
#else
	STATIC CONST ISTSBIT8 clz_high[] = {
		8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	ISTSBIT8 shift = 0;

	if (a < 0x10000) {
		shift += 16;
		a <<= 16;
	}
	if (a < 0x1000000) {
		shift += 8;
		a <<= 8;
	}
	shift += clz_high[a >> 24];
	return shift;
#endif
}

STATIC ISTVOID normalize(ISTUBIT32 bs, ISTSBIT16 *exp, ISTUBIT32 *mant)
{
	ISTSBIT8 shift = clz(bs) - 8;
	*mant = bs << shift;
	*exp = 1 - shift;
}

STATIC ISTFLOAT round_pack(ISTSBIT8 sign, ISTSBIT16 exp, ISTUBIT32 mant)
{
	ISTSBIT8 mode = float_rounding_mode;
	ISTSBIT8 rne = (mode == float_round_nearest_even);
	ISTSBIT8 increment = 0x40;
	ISTSBIT8 bits;
	ISTSBIT8 is_tiny;
	ISTFLOAT q;

	if (!rne) {
		if (mode == float_round_to_zero) {
			increment = 0;
		} else {
			increment = 0x7F;
			if (sign) {
				if (mode == float_round_up) 
					increment = 0;
			} else {
				if (mode == float_round_down) 
					increment = 0;
			}
		}
	}
	bits = mant & 0x7F;
	if (0xFD <= (ISTSBIT16)exp) {
		if ((0xFD < exp) || ((exp == 0xFD) && ((ISTSBIT32)(mant + increment) < 0))) {
			float_raise(float_flag_overflow | float_flag_inexact);
			q = pack(sign, 0xFF, 0) - (increment == 0);
			return q;
		}
		if (exp < 0) {
			is_tiny = (float_detect_tininess == float_tininess_before_rounding) \
				|| (exp < -1) || (mant + increment < 0x80000000);
			shift_right_jamming(mant, -exp, &mant);
			exp = 0;
			bits = mant & 0x7F;
			if (is_tiny && bits)
				float_raise(float_flag_underflow);
		}
	}
	if (bits)
		float_exception_flags |= float_flag_inexact;
	mant = (mant + increment) >> 7;
	mant &= ~(((bits ^ 0x40) == 0) & rne);
	if (mant == 0)
		exp = 0;
	return (pack(sign,exp,mant));
}

STATIC ISTFLOAT normalize_round_pack(ISTSBIT8 sign, ISTSBIT16 exp, ISTUBIT32 mant)
{
	ISTSBIT8 shift = clz(mant) - 1;
	return (round_pack(sign, exp - shift, mant << shift));
}

ISTFLOAT int_to_float(ISTINT a)
{
	ISTSBIT8 sign;

	if (a == 0)
		return (0);
	if (a == (ISTSBIT32)0x80000000)
		return (pack(1, 0x9E, 0));
	sign = (a < 0);
	return (normalize_round_pack(sign, 0x9C, sign ? (-a) : a));
}

ISTINT float_to_int(ISTFLOAT a)
{
	ISTSBIT8 sign = _s(a);
	ISTSBIT16 exp = _e(a);
	ISTSBIT16 shift = exp - 0x96;
	ISTUBIT32 mant = _m(a);
	ISTUBIT32 mant_extra;
	ISTINT z;
	ISTSBIT8 mode;

	if (0 <= shift) {
		if (0x9E <= exp) {
			if (a != 0xCF000000) {
				float_raise(float_flag_invalid);
				if (!sign || ((exp == 0xFF) && mant))
					return (0x7FFFFFFF);
			}
			return(ISTSBIT32)0x80000000;
		}
		z = (mant | 0x00800000) << shift;
		if (sign)
			z = -z;
	} else {
		if (exp < 0x7E) {
			mant_extra = exp | mant;
			z = 0;
		} else {
			mant |= 0x00800000;
			mant_extra = mant << (shift & 31);
			z = mant >> (-shift);
		}
		if (mant_extra)
			float_exception_flags |= float_flag_inexact;
		mode = float_rounding_mode;
		if (mode == float_round_nearest_even) {
			if ((ISTSBIT32)mant_extra < 0) {
				++z;
				if ((ISTUBIT32)(mant_extra << 1) == 0) z &= ~1;
			}
			if (sign)
				z = -z;
		} else {
			mant_extra = (mant_extra != 0);
			if (sign) {
				z += (mode == float_round_down) & mant_extra;
				z = - z;
			} else {
				z += (mode == float_round_up) & mant_extra;
			}
		}
	}
	return (z);
}

STATIC ISTFLOAT align_add(ISTFLOAT a, ISTFLOAT b, ISTSBIT8 sign)
{
	ISTSBIT16 expa = _e(a);
	ISTSBIT16 expb = _e(b);
	ISTSBIT16 expd = expa - expb;
	ISTUBIT32 manta = _m(a);
	ISTUBIT32 mantb = _m(b);
	ISTSBIT16 expz;
	ISTUBIT32 mantz;

	manta <<= 6;
	mantb <<= 6;
	if (0 < expd) {
		if (expa == 0xFF) {
			if (manta)
				return (propagate_nan(a, b));
			return (a);
		}
		if (expb == 0)
			--expd;
		else
			mantb |= 0x20000000;
		shift_right_jamming(mantb, expd, &mantb);
		expz = expa;
	} else if (expd < 0) {
		if (expb == 0xFF) {
			if (mantb)
				return (propagate_nan(a, b));
			return (pack(sign, 0xFF, 0));
		}
		if (expa == 0)
			++expd;
		else
			manta |= 0x20000000;
		shift_right_jamming(manta, - expd, &manta);
		expz = expb;
	} else {
		if (expa == 0xFF) {
			if (manta | mantb)
				return (propagate_nan(a, b));
			return (a);
		}
		if (expa == 0)
			return (pack(sign, 0, (manta + mantb) >> 6));
		mantz = 0x40000000 + manta + mantb;
		expz = expa;
		return (round_pack(sign, expz, mantz));
	}
	manta |= 0x20000000;
	mantz = (manta + mantb) << 1;
	--expz;
	if ((ISTSBIT32)mantz < 0) {
		mantz = manta + mantb;
		++expz;
	}
	return (round_pack(sign, expz, mantz));
}

STATIC ISTFLOAT align_sub(ISTFLOAT a, ISTFLOAT b, ISTSBIT8 sign)
{
	ISTSBIT16 expa = _e(a);
	ISTSBIT16 expb = _e(b);
	ISTSBIT16 expd = expa - expb;
	ISTUBIT32 manta = _m(a);
	ISTUBIT32 mantb = _m(b);
	ISTSBIT16 expz;
	ISTUBIT32 mantz;

	manta <<= 7;
	mantb <<= 7;
	if (0 < expd)
		goto A_EXP_BIGGER;
	if (expd < 0)
		goto B_EXP_BIGGER;
	if (expa == 0xFF) {
		if (manta | mantb)
			return (propagate_nan(a, b));
		float_raise(float_flag_invalid);
		return (float_default_nan);
	}
	if (expa == 0) {
		expa = 1;
		expb = 1;
	}
	if (mantb < manta)
		goto A_BIGGER;
	if (manta < mantb)
		goto B_BIGGER;
	return (pack(float_rounding_mode == float_round_down, 0, 0));

B_EXP_BIGGER:
	if (expb == 0xFF) {
		if (mantb)
			return (propagate_nan(a, b));
		return (pack(sign ^ 1, 0xFF, 0));
	}
	if (expa == 0) {
		++expd;
	} else {
		manta |= 0x40000000;
	}
	shift_right_jamming(manta, - expd, &manta);
	mantb |= 0x40000000;

B_BIGGER:
	mantz = mantb - manta;
	expz = expb - 1;
	sign ^= 1;
	return (normalize_round_pack(sign, expz, mantz));

A_EXP_BIGGER:
	if (expa == 0xFF) {
		if (manta)
			return (propagate_nan(a, b));
		return (a);
	}
	if (expb == 0) {
		--expd;
	} else {
		mantb |= 0x40000000;
	}
	shift_right_jamming(mantb, expd, &mantb);
	manta |= 0x40000000;

A_BIGGER:
	mantz = manta - mantb;
	expz = expa - 1;
	return (normalize_round_pack(sign, expz, mantz));
}

ISTFLOAT float_add(ISTFLOAT a, ISTFLOAT b)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT8 signb = _s(b);

	if (signa == signb)
		return (align_add(a, b, signa));
	else
		return (align_sub(a, b, signa));
}

ISTFLOAT float_sub(ISTFLOAT a, ISTFLOAT b)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT8 signb = _s(b);

	if (signa == signb)
		return (align_sub(a, b, signa));
	else
		return (align_add(a, b, signa));
}

ISTFLOAT float_mul(ISTFLOAT a, ISTFLOAT b)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT8 signb = _s(b);
	ISTSBIT8 signz = signa ^ signb;
	ISTSBIT16 expa = _e(a);
	ISTSBIT16 expb = _e(b);
	ISTUBIT32 manta = _m(a);
	ISTUBIT32 mantb = _m(b);
	ISTSBIT16 expz;
	ISTUBIT32 mantz0;
	ISTUBIT32 mantz1;

	if (expa == 0xFF) {
		if (manta || ((expb == 0xFF) && mantb)) {
			return (propagate_nan(a, b));
		}
		if ((expb | mantb) == 0) {
			float_raise(float_flag_invalid);
			return (float_default_nan);
		}
		return (pack(signz, 0xFF, 0));
	}
	if (expb == 0xFF) {
		if (mantb)
			return (propagate_nan(a, b));
		if ((expa | manta) == 0) {
			float_raise(float_flag_invalid);
			return (float_default_nan);
		}
		return (pack(signz, 0xFF, 0));
	}
	if (expa == 0) {
		if (manta == 0)
			return (pack(signz, 0, 0));
		normalize(manta, &expa, &manta);
	}
	if (expb == 0) {
		if (mantb == 0)
			return (pack(signz, 0, 0));
		normalize(mantb, &expb, &mantb);
	}
	expz = expa + expb - 0x7F;
	manta = (manta | 0x00800000) << 7;
	mantb = (mantb | 0x00800000) << 8;
	mul32to64(manta, mantb, &mantz0, &mantz1);
	mantz0 |= (mantz1 != 0);
	if (0 <= (ISTSBIT32)(mantz0 << 1)) {
		mantz0 <<= 1;
		--expz;
	}
	return (round_pack(signz, expz, mantz0));
}

ISTFLOAT float_div(ISTFLOAT a, ISTFLOAT b)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT8 signb = _s(b);
	ISTSBIT8 signz = signa ^ signb;
	ISTSBIT16 expa = _e(a);
	ISTSBIT16 expb = _e(b);
	ISTUBIT32 manta = _m(a);
	ISTUBIT32 mantb = _m(b);
	ISTSBIT16 expz;
	ISTUBIT32 mantz;
	ISTUBIT32 rem0;
	ISTUBIT32 rem1;
	ISTUBIT32 term0;
	ISTUBIT32 term1;

	if (expa == 0xFF) {
		if (manta)
			return (propagate_nan(a, b));
		if (expb == 0xFF) {
			if (mantb)
				return (propagate_nan(a, b));
			float_raise(float_flag_invalid);
			return (float_default_nan);
		}
		return (pack(signz, 0xFF, 0));
	}
	if (expb == 0xFF) {
		if (mantb)
			return (propagate_nan(a, b));
		return (pack(signz, 0, 0));
	}
	if (expb == 0) {
		if (mantb == 0) {
			if ((expa | manta) == 0) {
				float_raise(float_flag_invalid);
				return (float_default_nan);
			}
			float_raise(float_flag_divbyzero);
			return (pack(signz, 0xFF, 0));
		}
		normalize(mantb, &expb, &mantb);
	}
	if (expa == 0) {
		if (manta == 0)
			return (pack(signz, 0, 0));
		normalize(manta, &expa, &manta);
	}
	expz = expa - expb + 0x7D;
	manta = (manta | 0x00800000) << 7;
	mantb = (mantb | 0x00800000) << 8;
	if (mantb <= (manta + manta)) {
		manta >>= 1;
		++expz;
	}
	mantz = div64to32(manta, 0, mantb);
	if ((mantz & 0x3F) <= 2) {
		mul32to64(mantb, mantz, &term0, &term1);
		sub64(manta, 0, term0, term1, &rem0, &rem1);
		while ((ISTSBIT32)rem0 < 0) {
			--mantz;
			add64(rem0, rem1, 0, mantb, &rem0, &rem1);
		}
		mantz |= (rem1 != 0);
	}
	return (round_pack(signz, expz, mantz));
}

#ifdef IST_ENABLE_FAST_INVERSE_SQRT

INLINE ISTFLOAT frac_to_float(ISTINT a, ISTINT b)
{
	return float_div(int_to_float(a), int_to_float(b));
}

INLINE ISTFLOAT float_fast_inverse_sqrt(ISTFLOAT a)
{
	ISTUBIT32 i;
	ISTFLOAT x2;
	ISTFLOAT y;
	CONST ISTFLOAT threehalfs = frac_to_float(3, 2);

	x2 = float_mul(a, frac_to_float(1, 2));
	y = a;
	i = (ISTUBIT32)a;
	i = 0x5f3759df - (i >> 1);
	y = i;
	y = float_mul(y, float_sub(threehalfs, float_mul(float_mul(x2, y), y)));
	return y;
}

ISTFLOAT float_sqrt(ISTFLOAT a)
{
	return float_div(float_one(), float_fast_inverse_sqrt(a));
}

#else // IST_ENABLE_FAST_INVERSE_SQRT

ISTFLOAT float_sqrt(ISTFLOAT a)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT16 expa = _e(a);
	ISTUBIT32 manta = _m(a);
	ISTSBIT16 expz;
	ISTUBIT32 mantz;
	ISTUBIT32 rem0;
	ISTUBIT32 rem1;
	ISTUBIT32 term0;
	ISTUBIT32 term1;

	if (expa == 0xFF) {
		if (manta)
			return (propagate_nan(a, 0));
		if (!signa)
			return (a);
		float_raise(float_flag_invalid);
		return (float_default_nan);
	}
	if (signa) {
		if ((expa | manta) == 0)
			return (a);
		float_raise(float_flag_invalid);
		return (float_default_nan);
	}
	if (expa == 0) {
		if (manta == 0)
			return (0);
		normalize(manta, &expa, &manta);
	}
	expz = ((expa - 0x7F) >> 1) + 0x7E;
	manta = (manta | 0x00800000) << 8;
	mantz = sqrt32(expa, manta) + 2;
	if ((mantz & 0x7F) <= 5) {
		if (mantz < 2) {
			mantz = 0x7FFFFFFF;
			return (round_pack(0, expz, mantz));
		} else {
			manta >>= expa & 1;
			mul32to64(mantz, mantz, &term0, &term1);
			sub64(manta, 0, term0, term1, &rem0, &rem1);
			while ((ISTSBIT32)rem0 < 0) {
				--mantz;
				shift64_left(0, mantz, 1, &term0, &term1);
				term1 |= 1;
				add64(rem0, rem1, term0, term1, &rem0, &rem1);
			}
			mantz |= ((rem0|rem1) != 0);
		}
	}
	shift_right_jamming(mantz, 1, &mantz);
	return (round_pack(0, expz, mantz));
}

#endif // IST_ENABLE_FAST_INVERSE_SQRT

ISTBOOL float_eq(ISTFLOAT a, ISTFLOAT b)
{
	if (((_e(a) == 0xFF) && _m(a)) || ((_e(b) == 0xFF) && _m(b))) {
		if (check_signaling_nan(a) || check_signaling_nan(b))
			float_raise(float_flag_invalid);
		return (0);
	}
	return ((ISTBOOL)((a == b) || ((ISTUBIT32)((a | b) << 1) == 0)));
}

ISTBOOL float_le(ISTFLOAT a, ISTFLOAT b)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT8 signb = _s(b);

	if (((_e(a) == 0xFF) && _m(a)) || ((_e(b) == 0xFF) && _m(b))) {
		float_raise(float_flag_invalid);
		return (0);
	}
	if (signa != signb)
		return (signa || ((ISTUBIT32)((a | b)<<1) == 0));
	return ((ISTBOOL)((a == b) || (signa ^ (a < b))));
}

ISTBOOL float_lt(ISTFLOAT a, ISTFLOAT b)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT8 signb = _s(b);

	if (((_e(a) == 0xFF) && _m(a)) || ((_e(b) == 0xFF) && _m(b))) {
		float_raise(float_flag_invalid);
		return (0);
	}
	if (signa != signb)
		return signa && ((ISTUBIT32)((a | b)<<1) != 0);
	return ((ISTBOOL)((a != b) && (signa ^ (a < b))));
}

ISTFLOAT float_abs(ISTFLOAT a)
{
	return (_s(a) ? float_negate(a) : a);
}

ISTFLOAT float_copysign(ISTFLOAT a, ISTFLOAT b)
{
	return (_s(b) ? (((ISTUBIT32)a) | ((ISTUBIT32)0x80000000)) : (((ISTUBIT32)a) & ((ISTUBIT32)0x7FFFFFFF)));
}

#define POLYNOM1(x, a)  (float_add(float_mul((a)[1],(x)),(a)[0]))
#define POLYNOM2(x, a)  (float_add(float_mul(POLYNOM1((x),(a)+1),(x)),(a)[0]))
#define POLYNOM3(x, a)  (float_add(float_mul(POLYNOM2((x),(a)+1),(x)),(a)[0]))
#define POLYNOM4(x, a)  (float_add(float_mul(POLYNOM3((x),(a)+1),(x)),(a)[0]))

ISTFLOAT float_atan(ISTFLOAT x)
{
	STATIC ISTFLOAT p[4] = {
		0xC15B0533,
		0xC1A40BFE,
		0xC107E9FB,
		0xBF566BD7
	};
	STATIC ISTFLOAT q[5] = {
		0x422443E6,
		0x42AC5090,
		0x426E5052,
		0x4170624F,
		0x3F800000
	};
	STATIC ISTFLOAT a[4] = {
		0,
		0x3F060A92,  /* pi/6 */
		0x3FC90FDB,
		0x3F860A92   /* pi/3 */
	};
	ISTSBIT8 sign = _s(x);
	ISTINT n;
	ISTFLOAT g;

	if (check_signaling_nan(x)) {
		float_raise(float_flag_invalid);
		return (0);
	}
	if (sign)
		x = float_negate(x);
	if (!float_le(x, 0x3F800000)) {
		x = float_div(0x3F800000, x);
		n = 2;
	} else
		n = 0;
	if (!float_le(x, 0x3E8930A3)) {       /* 2-sqtr(3) */
		n = n + 1;
		x = float_div(float_add(float_sub(float_sub(
			float_mul(0x3F3B67AF, x), 0x3F000000), 0x3F000000), x), 
			float_add(0x3FDDB3D7, x));
	}
	g = float_mul(x, x);
	x = float_add(x, float_div(float_mul(
		float_mul(x, g), POLYNOM3(g, p)), POLYNOM4(g, q)));
	if (n > 1) {
		x = float_negate(x);
	}
	x = float_add(x, a[n]);
	return (sign ? float_negate(x) : x);
}

ISTFLOAT float_atan2(ISTFLOAT y, ISTFLOAT x)
{
	ISTFLOAT absx;
	ISTFLOAT absy;
	ISTFLOAT val;

	if (x == 0 && y == 0) {
		float_raise(float_flag_invalid);
		return (0);
	}
	absy = (_s(y) ? float_negate(y) : y);
	absx = (_s(x) ? float_negate(x) : x);
	if (float_eq(float_sub(absy, absx), absy))
		return _s(y) ? float_negate(float_pi_2()) : float_pi_2(); /* -pi/2 or pi/2 */
	if (float_eq(float_sub(absx, absy), absx))
		val = 0;
	else
		val = float_atan(float_div(y, x));
	if (!_s(x))
		return val;
	if (_s(y))
		return float_sub(val, float_pi());    /* val - pi */
	return float_add(val, float_pi());    /* val + pi */
}

#if defined(IST_FLOOR)
ISTFLOAT float_round_to_int(ISTFLOAT a, ISTSBIT8 mode)
{
	ISTSBIT8 sign = _s(a);
	ISTSBIT16 exp = _e(a);
	ISTUBIT32 lmask;
	ISTUBIT32 rmask;
	ISTFLOAT z;

	if (0x96 <= exp) {
		if ((exp == 0xFF) && _m(a)) {
			return propagate_nan(a, a);
		}
		return a;
	}
	if (exp <= 0x7E) {
		if ((ISTUBIT32)(a << 1) == 0) {
			return a;
		}
		float_exception_flags |= float_flag_inexact;
		switch (mode) {
		case float_round_nearest_even:
			if ((exp == 0x7E) && _m(a)) {
				return round_pack(sign, 0x7F, 0);
			}
			break;
		case float_round_down:
			return sign ? 0xBF800000 : 0;
		case float_round_up:
			return sign ? 0x80000000 : 0x3F800000;
		}
		return round_pack(sign, 0, 0);
	}
	lmask = 1;
	lmask <<= 0x96 - exp;
	rmask = lmask - 1;
	z = a;
	//mode = float_rounding_mode;
	if (mode == float_round_nearest_even) {
		z += lmask >> 1;
		if ((z & rmask) == 0) {
			z &= ~ lmask;
		}
	} else if (mode != float_round_to_zero) {
		if (_s(z) ^ (mode == float_round_up)) {
			z += rmask;
		}
	}
	z &= ~ rmask;
	if (z != a) {
		float_exception_flags |= float_flag_inexact;
	}
	return z;
}
#endif // IST_FLOOR

#ifdef IST_FLOOR
ISTFLOAT float_floor(ISTFLOAT a)
{
	ISTFLOAT r = float_round_to_int(a, float_round_down);
	if (float_le(r, 0)) {
		return float_add(r, (float_le(r, a) ? 0 : float_negate(float_one())));
	}
	return r;
}
#endif // IST_FLOOR

#if defined(IST_EXP) || defined(IST_ASIN) || defined(IST_ACOS)
INLINE ISTFLOAT float_ldexp(ISTFLOAT a, ISTINT pw2)
{
	return (((((a >> 23) & 0x000000FF) + pw2) & 0xFF) << 23) | (a & 0x807FFFFF);
}
#endif // IST_EXP || IST_ASIN || IST_ACOS

#ifdef IST_EXP
ISTFLOAT float_exp(ISTFLOAT a)
{
#define EXP_P(z) float_add(float_mul(P1, z), P0)
#define EXP_Q(z) float_add(float_mul(Q1, z), Q0)
	STATIC CONST ISTFLOAT P0 = 0x3E800000; // 0.2499999995E+0
	STATIC CONST ISTFLOAT P1 = 0x3B885308; // 0.4160288626E-2
	STATIC CONST ISTFLOAT Q0 = 0x3F000000; // 0.5000000000E+0
	STATIC CONST ISTFLOAT Q1 = 0x3D4CBF5B; // 0.4998717877E-1
	STATIC CONST ISTFLOAT C1 = 0x3F318000; // 0.693359375
	STATIC CONST ISTFLOAT C2 = 0xB95E8083; // -2.1219444005469058277e-4
	STATIC CONST ISTFLOAT BIGX = 0x42B17218; // ln(HUGE_VALF)
	STATIC CONST ISTFLOAT EXPEPS = 0x33D6BF95;	// exp(1.0E-7)=0.0000001
	STATIC CONST ISTFLOAT K1 = 0x3FB8AA3B; // 1/ln(2) = 1.4426950409
	STATIC CONST ISTFLOAT HUGE_VALF = 0x7F7FFFFF; // 3.402823466e+38 
	ISTSBIT8 signa = _s(a);
	ISTSBIT16 expa = _e(a);
	ISTUBIT32 manta = _m(a);
	ISTINT n;
	ISTFLOAT an;
	ISTFLOAT g;
	ISTFLOAT r;
	ISTFLOAT z;
	ISTFLOAT y;
	ISTSBIT8 sign;

	if (expa == 0xFF) {
		if (manta)
			return (propagate_nan(a, 0));
		if (!signa)
			return (a);
		float_raise(float_flag_invalid);
		return (float_default_nan);
	}
	if (float_lt(a, 0)) { 
		y = float_negate(a); 
		sign = 1; 
	} else {
		y = a;
		sign = 0;
	}
	if (float_lt(y, EXPEPS)) {
		return float_one();
	}
	if (!float_le(y, BIGX)) {
		if (sign) {
#ifdef IST_OS_WIN32
			return 0;
#else // IST_OS_WIN32
			return HUGE_VALF;
#endif // IST_OS_WIN32
		} else {
			return 0;
		}
	}
	z = float_mul(y, K1);
	n = float_to_int(z);
	if (float_lt(n, 0)) {
		--n;
	}
	if (!float_lt(float_sub(z, n), Q0)) {
		++n;
	}
	an = int_to_float(n);
	g = float_sub(float_sub(y, float_mul(an, C1)), float_mul(an, C2));
	z = float_mul(g, g);
	r = float_mul(EXP_P(z), g);
	r = float_add(Q0, float_div(r, float_sub(EXP_Q(z), r)));
	n++;
	z = float_ldexp(r, n);
	if (sign) {
		return float_div(float_one(), z);
	}
	return z;
}
#endif // IST_EXP

#ifdef IST_TAN
INLINE ISTFLOAT float_tancot(ISTFLOAT x, ISTBOOL iscotan)
{
#define TAN_P(f,g) float_add(float_mul(float_mul(P1,g),f),f)
#define TAN_Q(g) float_add(float_mul(float_add(float_mul(Q2,g),Q1),g),Q0)
	STATIC CONST ISTFLOAT P0 = 0x3F800000; // 0.100000000E+1
	STATIC CONST ISTFLOAT P1 = 0xBDC433B8; // -0.958017723E-1
	STATIC CONST ISTFLOAT Q0 = 0x3F800000; // 0.100000000E+1
	STATIC CONST ISTFLOAT Q1 = 0xBEDBB7AF; // -0.429135777E+0
	STATIC CONST ISTFLOAT Q2 = 0x3C1F3375; // 0.971685835E-2
	STATIC CONST ISTFLOAT C1 = 0x3FC90000; // 1.5703125
	STATIC CONST ISTFLOAT C2 = 0x39FDAA22; // 4.83826794897E-4
	STATIC CONST ISTFLOAT YMAX = 0x45C90800; // 6433.0 A reasonable choice for YMAX is the integer part of B**(t/2)*PI/2:
	STATIC CONST ISTFLOAT TWO_O_PI = 0x3F22F983; // 0.6366197724
	ISTFLOAT f;
	ISTFLOAT g;
	ISTFLOAT xn;
	ISTFLOAT xnum;
	ISTFLOAT xden;
	ISTINT n;

	if (!float_le(float_abs(x), YMAX)) {
		float_raise(float_flag_invalid);
		return 0;
	}
	/*Round x*2*PI to the nearest integer*/
	n = float_to_int(float_add(float_mul(x, TWO_O_PI), ((! float_le(x, 0)) ? 0x3F000000 : 0xBF000000))); /*works for +-x*/
	xn = int_to_float(n);
	xnum = int_to_float(float_to_int(x));
	xden = float_sub(x, xnum);
	f = float_sub(float_add(float_sub(xnum, float_mul(xn, C1)), xden), float_mul(xn, C2));
	if (float_lt(float_abs(f), float_eps())) {
		xnum = f;
		xden = float_one();
	} else {
		g = float_mul(f, f);
		xnum = TAN_P(f,g);
		xden = TAN_Q(g);
	}
	if (n & 1) { //xn is odd
		if (iscotan) {
			return float_negate(float_div(xnum, xden));
		}
		return float_negate(float_div(xden, xnum));
	}
	if (iscotan) {
		return float_div(xden, xnum);
	}
	return float_div(xnum, xden);
}

ISTFLOAT float_tan(ISTFLOAT a)
{
	return float_tancot(a, ISTFALSE);
}
#endif // IST_TAN

#if defined(IST_SIN) || defined(IST_COS)
INLINE ISTFLOAT float_sincos(ISTFLOAT x, ISTBOOL iscos)
{
	STATIC CONST ISTFLOAT r1 = 0xBE2AAAA4; // -0.1666665668E+0
	STATIC CONST ISTFLOAT r2 = 0x3C08873E; // 0.8333025139E-2
	STATIC CONST ISTFLOAT r3 = 0xB94FB222; // -0.1980741872E-3
	STATIC CONST ISTFLOAT r4 = 0x362E9C5B; // 0.2601903036E-5
	STATIC CONST ISTFLOAT C1 = 0x40490000; // 3.140625
	STATIC CONST ISTFLOAT C2 = 0x3A7DAA22; // 9.676535897E-4
	STATIC CONST ISTFLOAT YMAX = 0x46490C00; // 12867.0
	STATIC CONST ISTFLOAT HALF_PI = 0x3FC90FDB;// 1.5707963268 
	STATIC CONST ISTFLOAT iPI = 0x3EA2F983; // 0.3183098862
	STATIC CONST ISTFLOAT EPS2 = 0x337FFFF3; // 59.6046E-9
	ISTFLOAT y;
	ISTFLOAT f;
	ISTFLOAT r;
	ISTFLOAT g;
	ISTFLOAT XN;
	ISTINT N;
	ISTBOOL sign;

	if (iscos) {
		y = float_add(float_abs(x), HALF_PI);
		sign = ISTFALSE;
	} else {
		if (float_lt(x, 0)) {
			y = float_negate(x);
			sign = ISTTRUE;
		} else {
			y = x;
			sign = ISTFALSE;
		}
	}
	if (!float_le(y, YMAX)) {
		float_raise(float_flag_invalid);
		return 0;
	}
	/*Round y/PI to the nearest integer*/
	N = float_to_int(float_add(float_mul(y, iPI), 0x3F000000));
	/*If N is odd change sign*/
	if (N & 1) {
		sign = !sign;
	}
	XN = int_to_float(N);
	/*Cosine required? (is done here to keep accuracy)*/
	if (iscos) {
		XN = float_sub(XN, 0x3F000000);
	}
	y = float_abs(x);
	r = int_to_float(float_to_int(y));
	g = float_sub(y, r);
	f = float_sub(float_add(float_sub(r, float_mul(XN, C1)), g), float_mul(XN, C2));
	g = float_mul(f, f);
	if (!float_le(g, EPS2)) { //Used to be if(fabsf(f)>EPS)
		r = float_mul(float_add(float_mul(float_add(float_mul(float_add(float_mul(r4, g), r3), g), r2), g), r1), g);
		f = float_add(f, float_mul(f, r));
	}
	return (sign ? float_negate(f) : f);
}
#endif // IST_SIN || IST_COS

#ifdef IST_SIN
ISTFLOAT float_sin(ISTFLOAT a)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT16 expa = _e(a);
	ISTUBIT32 manta = _m(a);

	if (expa == 0xFF) {
		if (manta) {
			return (propagate_nan(a, 0));
		}
		if (!signa) {
			return (a);
		}
		float_raise(float_flag_invalid);
		return (float_default_nan);
	}
	if (float_eq(a, 0)) {
		return 0;
	}
	return float_sincos(a, ISTFALSE);
}
#endif // IST_SIN

#ifdef IST_COS
ISTFLOAT float_cos(ISTFLOAT a)
{
	ISTSBIT8 signa = _s(a);
	ISTSBIT16 expa = _e(a);
	ISTUBIT32 manta = _m(a);

	if (expa == 0xFF) {
		if (manta) {
			return (propagate_nan(a, 0));
		}
		if (!signa) {
			return (a);
		}
		float_raise(float_flag_invalid);
		return (float_default_nan);
	}
	if (float_eq(a, 0)) {
		return float_one();
	}
	return float_sincos(a, ISTTRUE);
}
#endif // IST_COS

#if defined(IST_ASIN) || defined(IST_ACOS)
INLINE ISTFLOAT float_asincos(ISTFLOAT x, ISTBOOL isacos)
{
#define ASINCOS_P(g) float_add(float_mul(P2,g),P1)
#define ASINCOS_Q(g) float_add(float_mul(float_add(float_mul(Q2,g),Q1),g),Q0)
	STATIC CONST ISTFLOAT P1 = 0x3F6F166B; // 0.933935835E+0
	STATIC CONST ISTFLOAT P2 = 0xBF012065; // -0.504400557E+0
	STATIC CONST ISTFLOAT Q0 = 0x40B350F0; // 0.560363004E+1
	STATIC CONST ISTFLOAT Q1 = 0xC0B18D0B; // -0.554846723E+1
	STATIC CONST ISTFLOAT Q2 = 0x3F800000; // 0.100000000E+1
	STATIC CONST ISTFLOAT EPS = 0x39800000; // 244.14062E-6
	ISTFLOAT y;
	ISTFLOAT g;
	ISTFLOAT r;
	ISTUCHAR i;
	ISTBOOL quartPI = isacos;
	STATIC CONST ISTFLOAT a[2] = { 0, 0x3F490FDB };
	STATIC CONST ISTFLOAT b[2] = { 0x3FC90FDB, 0x3F490FDB };

	y = float_abs(x);
	if (float_lt(y, EPS)) {
		r = y;
	} else {
		if (!float_le(y, 0x3F000000)) {
			quartPI = !isacos;
			if (!float_le(y, float_one())) {
				float_raise(float_flag_invalid);
				return 0;
			}
			g = float_add(float_sub(0x3F000000, y), 0x3F000000);
			g = float_ldexp(g, -1);
			y = float_sqrt(g);
			y = float_negate(float_add(y, y));
		} else {
			g = float_mul(y, y);
		}
		r = float_add(y, float_mul(y, float_div(float_mul(ASINCOS_P(g), g), ASINCOS_Q(g))));
	}
	i = quartPI;
	if (isacos) {
		if (float_lt(x, 0)) {
			r = float_add(float_add(b[i], r), b[i]);
		} else {
			r = float_add(float_sub(a[i], r), a[i]);
		}
	} else {
		r = float_add(float_add(a[i], r), a[i]);
		if (float_lt(x, 0)) {
			r = float_negate(r);
		}
	}
	return r;
}
#endif // IST_ASIN || IST_ACOS

#ifdef IST_ASIN
ISTFLOAT float_asin(ISTFLOAT a)
{
	STATIC CONST ISTFLOAT HALF_PI = 0x3FC90FDB;// 1.5707963268 

	if (float_eq(a, float_one())) {
		return HALF_PI;
	} else if (float_eq(a, float_negate(float_one()))) {
		return float_negate(HALF_PI);
	} else if (float_eq(a, 0)) {
		return 0;
	}
	return float_asincos(a, ISTFALSE);
}
#endif // IST_ASIN

#ifdef IST_ACOS
ISTFLOAT float_acos(ISTFLOAT a)
{
	STATIC CONST ISTFLOAT HALF_PI = 0x3FC90FDB; // 1.5707963268 
	STATIC CONST ISTFLOAT PI = 0x40490FDB; // 3.1415926536 

	if (float_eq(a, float_one())) { 
		return 0;
	}else if (float_eq(a, float_negate(float_one()))) {
		return PI;
	} else if (float_eq(a, 0)) { 
		return HALF_PI;
	}
	return float_asincos(a, ISTTRUE);
}
#endif // IST_ACOS

#endif // IST_FLOAT_IEEE754
