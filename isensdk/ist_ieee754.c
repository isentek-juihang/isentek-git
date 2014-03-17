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
	float32_default_nan = 0xFFC00000
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
	static const ISTSBIT16 sqrtoa[] = { /* square odd adjustment */
		0x0004, 0x0022, 0x005D, 0x00B1, 0x011D, 0x019F, 0x0236, 0x02E0,
		0x039C, 0x0468, 0x0545, 0x0631, 0x072B, 0x0832, 0x0946, 0x0A67
	};
	static const ISTSBIT16 sqrtea[] = { /* square even adjustment */
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
	static const ISTSBIT8 clz_table[32] = {
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
	static const ISTSBIT8 clz_high[] = {
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
		return (float32_default_nan);
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
			return (float32_default_nan);
		}
		return (pack(signz, 0xFF, 0));
	}
	if (expb == 0xFF) {
		if (mantb)
			return (propagate_nan(a, b));
		if ((expa | manta) == 0) {
			float_raise(float_flag_invalid);
			return (float32_default_nan);
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
			return (float32_default_nan);
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
				return (float32_default_nan);
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
	const ISTFLOAT threehalfs = frac_to_float(3, 2);

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
		return (float32_default_nan);
	}
	if (signa) {
		if ((expa | manta) == 0)
			return (a);
		float_raise(float_flag_invalid);
		return (float32_default_nan);
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
	static ISTFLOAT p[4] = {
		0xC15B0533,
		0xC1A40BFE,
		0xC107E9FB,
		0xBF566BD7
	};
	static ISTFLOAT q[5] = {
		0x422443E6,
		0x42AC5090,
		0x426E5052,
		0x4170624F,
		0x3F800000
	};
	static ISTFLOAT a[4] = {
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

#endif // IST_FLOAT_IEEE754
