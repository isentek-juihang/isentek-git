#include "ist_milieu.h"

#ifdef IST_RMA

struct rma_filter {
	RMAFilter pub;
	ISTINT period;
	ISTFLOAT total_w;
	ISTFLOAT w;
	ISTFLOAT avg;
};

STATIC RMAFilter *New(ISTVOID);
STATIC ISTVOID Delete(RMAFilter *self);
STATIC ISTFLOAT Process(RMAFilter *self, ISTFLOAT data);
STATIC ISTVOID SetPeriod(RMAFilter *self, ISTINT period);
STATIC ISTVOID SetWeight(RMAFilter *self, ISTFLOAT weight);

STATIC RMAFilter ThisClass = {0};
STATIC RMAFilter *This = NULL;

RMAFilter *New(ISTVOID)
{
	struct rma_filter *s;

	s = (struct rma_filter *)ist_calloc(1, sizeof(struct rma_filter));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->avg = 0;
	s->total_w = 0;
	s->w = IST_RMA_WEIGHT;
	s->period = IST_RMA_PERIOD;
	return (RMAFilter *)s;

EXIT:
	return (RMAFilter *)NULL;
}

ISTVOID Delete(RMAFilter *self)
{
	struct rma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct rma_filter *)self;
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTFLOAT Process(RMAFilter *self, ISTFLOAT data)
{
	struct rma_filter *s;
	ISTFLOAT alpha;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	if (s->period < 0) {
		if (_le(s->total_w, _sub(_max, s->w))) {
			s->total_w = _add(s->total_w, s->w);
		} else {
			s->total_w = _mul(_float(2), s->w);
		}
	} else {
		s->total_w = _mul(_float(s->period), s->w);
	}
	if (_eq(s->total_w, s->w)) {
		s->avg = data;
	} else {
		alpha = _div(_sub(s->total_w, s->w), s->total_w);
		s->avg = _add(_mul(alpha, s->avg), _mul(data, _sub(_one, alpha)));
	}
	return s->avg;

EXIT:
	return 0;
}

ISTVOID SetPeriod(RMAFilter *self, ISTINT period)
{
	struct rma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct rma_filter *)self;
	s->period = period;
	s->w = _one;
	return;

EXIT:
	return;
}

ISTVOID SetWeight(RMAFilter *self, ISTFLOAT weight)
{
	struct rma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct rma_filter *)self;
	s->w = _abs(weight);
	return;

EXIT:
	return;
}

RMAFilter *IST_RMAFilter(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		ThisClass.SetPeriod = SetPeriod;
		ThisClass.SetWeight = SetWeight;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_CMA
