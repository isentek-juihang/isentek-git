#ifndef IST_QFT_H_INCLUDED
#define IST_QFT_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

// For Q format to IEEE754 floating point
#define IST_SFP_RES _div(_one,_float(1<<IST_QFT_N))
#define IST_SFP_MAX _sub(_float(1<<(IST_QFT_M-1)),IST_SFP_RES)
#define IST_SFP_MIN _neg(_float(1<<(IST_QFT_M-1)))
#define IST_SFP(_qft_) _div(_float((_qft_)),_float(1U<<IST_QFT_N))
// For IEEE754 to Q format fixed point
#define IST_QFP_MAX	((ISTQFT)(((ISTUBIT32)-1)>>1))
#define IST_QFP_MIN ((ISTQFT)(((ISTSBIT32)-1)<<31))
#define IST_QFP(_sft_) \
	(!_lt((_sft_),_sub(IST_SFP_MAX,IST_SFP_RES)) ? \
	 IST_QFP_MAX : \
	 (_le((_sft_),_add(IST_SFP_MIN,IST_SFP_RES)) ? \
	  IST_QFP_MIN : \
	  _int(_mul((_sft_),_float(1U<<IST_QFT_N)))))

IST_EXTERN_C_END

#endif // IST_QFT_H_INCLUDED
