#include "ist_milieu.h"

#ifdef IST_BIQUAD_LPF

typedef ISTVOID (*ist_ComputeParams)(BiquadLowPassFilter *self, ISTFLOAT Q, ISTFLOAT fc);
typedef ISTVOID (*ist_ComputeSamplingPeriod)(BiquadLowPassFilter *self, ISTFLOAT dT);

struct biquad_lowpass {
	BiquadLowPassFilter pub;
	ist_ComputeParams ComputeParams;
	ist_ComputeSamplingPeriod ComputeSamplingPeriod;
	IST_WEAK BiquadLowPassFilter *next;
	ISTFLOAT Q;
	ISTFLOAT fc;
	ISTFLOAT dT;
	ISTFLOAT iQ;
	ISTFLOAT K;
	ISTFLOAT D;
	ISTFLOAT K2;
	ISTFLOAT KiQ;
	ISTFLOAT a0;
	ISTFLOAT a1;
	ISTFLOAT b1;
	ISTFLOAT b2;
	ISTFLOAT x1;
	ISTFLOAT x2;
	ISTFLOAT y1;
	ISTFLOAT y2;
	ISTBOOL is_first;
};

STATIC BiquadLowPassFilter *New(ISTVOID);
STATIC ISTVOID Delete(BiquadLowPassFilter *self);
STATIC ISTFLOAT Process(BiquadLowPassFilter *self, ISTFLOAT data);
STATIC ISTVOID SetQualityFactor(BiquadLowPassFilter *self, ISTFLOAT Q);
STATIC ISTVOID SetOmegaCutoff(BiquadLowPassFilter *self, ISTFLOAT fc);
STATIC ISTVOID SetSamplingPeriod(BiquadLowPassFilter *self, ISTFLOAT dT);
STATIC ISTVOID Cascade(BiquadLowPassFilter *self, BiquadLowPassFilter *next);
STATIC ISTVOID ComputeParams(BiquadLowPassFilter *self, ISTFLOAT Q, ISTFLOAT fc);
STATIC ISTVOID ComputeSamplingPeriod(BiquadLowPassFilter *self, ISTFLOAT dT);

STATIC BiquadLowPassFilter ThisClass = {0};
STATIC BiquadLowPassFilter *This = NULL;

ISTVOID ComputeParams(BiquadLowPassFilter *self, ISTFLOAT Q, ISTFLOAT fc)
{
	struct biquad_lowpass *s = (struct biquad_lowpass *)self;

	s->Q = Q;
	s->fc = fc;
	s->iQ = _div(_one, Q);
	s->fc = fc;
}

STATIC ISTVOID ComputeSamplingPeriod(BiquadLowPassFilter *self, ISTFLOAT dT)
{
	struct biquad_lowpass *s = (struct biquad_lowpass *)self;

	if (_gt(dT, 0)) {
		s->dT = dT;
	}
	s->K = _tan(_mul3(_pi, s->fc, dT));
	s->K2 = _pow2(s->K);
	s->KiQ = _mul(s->K, s->iQ);
	s->D = _div(_one, _add3(s->K2, s->KiQ, _one));
	s->a0 = _mul(s->K2, s->D);
	s->a1 = _mul(_float(2), s->a0);
	s->b1 = _mul3(_float(2), _sub(s->K2, _one), s->D);
	s->b2 = _mul(_add(_sub(s->K2, s->KiQ), _one), s->D);
}

BiquadLowPassFilter *New(ISTVOID)
{
	struct biquad_lowpass *s;

	s = (struct biquad_lowpass *)ist_calloc(1, sizeof(struct biquad_lowpass));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->ComputeParams = ComputeParams;
	s->ComputeSamplingPeriod = ComputeSamplingPeriod;
	s->is_first = ISTTRUE;
	s->dT = IST_BIQUAD_LPF_DT;
	s->ComputeParams((BiquadLowPassFilter *)s, IST_BIQUAD_LPF_Q, IST_BIQUAD_LPF_FC);
	s->ComputeSamplingPeriod((BiquadLowPassFilter *)s, IST_BIQUAD_LPF_DT);
	return (BiquadLowPassFilter *)s;

EXIT:
	return (BiquadLowPassFilter *)NULL;
}

ISTVOID Delete(BiquadLowPassFilter *self)
{
	struct biquad_lowpass *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct biquad_lowpass *)self;
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTFLOAT Process(BiquadLowPassFilter *self, ISTFLOAT data)
{
	struct biquad_lowpass *s;
	ISTFLOAT y;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct biquad_lowpass *)self;
	if (s->is_first) {
		s->x1 = s->x2 = data;
		s->y1 = s->y2 = data;
		s->is_first = ISTFALSE;
		y = data;
	} else {
		y = _sub3(_add(_mul(_add(data, s->x2), s->a0), _mul(s->x1, s->a1)), _mul(s->y1, s->b1), _mul(s->y2, s->b2));
		s->x2 = s->x1;
		s->y2 = s->y1;
		s->x1 = data;
		s->y1 = y;
	}
	if (s->next) {
		y = s->next->Process(s->next, y);
	}
	return y;

EXIT:
	return 0;
}

ISTVOID SetQualityFactor(BiquadLowPassFilter *self, ISTFLOAT Q)
{
	struct biquad_lowpass *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct biquad_lowpass *)self;
	s->ComputeParams(self, Q, s->fc);
	s->ComputeSamplingPeriod(self, s->dT);
	return;

EXIT:
	return;
}

ISTVOID SetOmegaCutoff(BiquadLowPassFilter *self, ISTFLOAT fc)
{
	struct biquad_lowpass *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct biquad_lowpass *)self;
	s->ComputeParams(self, s->Q, fc);
	s->ComputeSamplingPeriod(self, s->dT);
	return;

EXIT:
	return;
}

ISTVOID SetSamplingPeriod(BiquadLowPassFilter *self, ISTFLOAT dT)
{
	struct biquad_lowpass *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct biquad_lowpass *)self;
	s->ComputeSamplingPeriod(self, dT);
	return;

EXIT:
	return;
}

ISTVOID Cascade(BiquadLowPassFilter *self, BiquadLowPassFilter *next)
{
	struct biquad_lowpass *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	if (next == self) {
		goto EXIT;
	}
	s = (struct biquad_lowpass *)self;
	s->next = next;
	return;

EXIT:
	return;
}

BiquadLowPassFilter *IST_BiquadLowPassFilter(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.SetQualityFactor = SetQualityFactor;
		ThisClass.SetOmegaCutoff = SetOmegaCutoff;
		ThisClass.SetSamplingPeriod = SetSamplingPeriod;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_BIQUAD_LPF
