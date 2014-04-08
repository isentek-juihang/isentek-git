#ifndef IST_BQDLPF_H_INCLUDED
#define IST_BQDLPF_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _BiquadLowPassFilter;
typedef struct _BiquadLowPassFilter BiquadLowPassFilter;

struct _BiquadLowPassFilter {
	BiquadLowPassFilter *(*New)(ISTVOID);
	ISTVOID (*Delete)(BiquadLowPassFilter *);
	ISTFLOAT (*Process)(BiquadLowPassFilter *, ISTFLOAT data);
	ISTVOID (*SetQualityFactor)(BiquadLowPassFilter *, ISTFLOAT Q);
	ISTVOID (*SetOmegaCutoff)(BiquadLowPassFilter *, ISTFLOAT fc);
	ISTVOID (*SetSamplingPeriod)(BiquadLowPassFilter *, ISTFLOAT dT);
	ISTVOID (*Cascade)(BiquadLowPassFilter *, BiquadLowPassFilter *);
	IST_READ ISTBOOL IsObject;
};

BiquadLowPassFilter *IST_BiquadLowPassFilter(ISTVOID);

IST_EXTERN_C_END

#endif // IST_BQDLPF_H_INCLUDED