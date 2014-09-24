#include "ist_milieu.h"

#ifdef IST_ALMA

typedef ISTFLOAT (*ist_Compute)(ALMAFilter *);
typedef ISTVOID (*ist_ComputeParams)(ALMAFilter *);

struct alma_filter {
	ALMAFilter pub;
	ist_Compute Compute;
	ist_ComputeParams ComputeParams;
	ISTFLOAT *buf;
	ISTINT len;
	ISTFLOAT *w;
	ISTFLOAT wsum;
	ISTINT period;
	ISTFLOAT offset;
	ISTINT sigma;
	ISTFLOAT m;
	ISTFLOAT s;
	ISTFLOAT s2;
};

STATIC ALMAFilter *New(ISTVOID);
STATIC ISTVOID Delete(ALMAFilter *self);
STATIC ISTFLOAT Process(ALMAFilter *self, ISTFLOAT data);
STATIC ISTVOID SetPeriod(ALMAFilter *self, ISTINT period);
STATIC ISTVOID SetSigma(ALMAFilter *self, ISTINT sigma);
STATIC ISTVOID SetOffset(ALMAFilter *self, ISTFLOAT offset);
STATIC ISTFLOAT Compute(ALMAFilter *self);
STATIC ISTVOID ComputeParams(ALMAFilter *self);

STATIC ALMAFilter ThisClass = {0};
STATIC ALMAFilter *This = NULL;

ISTFLOAT Compute(ALMAFilter *self)
{
	struct alma_filter *s;
	ISTFLOAT alma = 0;
	ISTINT i;

	s = (struct alma_filter *)self;
	for (i = 0; i < s->len; ++i) {
		alma = _add(alma, _mul(s->buf[i], s->w[i]));
	}
	return _div(alma, s->wsum);
}

ISTVOID ComputeParams(ALMAFilter *self) 
{
	struct alma_filter *s;
	ISTINT i;
	ISTFLOAT im;

	s = (struct alma_filter *)self;
	s->m = _floor(_mul(s->offset, _float(s->period - 1)));
	s->s = _float(s->period / s->sigma);
	s->s2 = _mul(_float(2), _pow2(s->s));
	if (s->w) {
		s->wsum = 0;
		for (i = 0; i < s->period; ++i) {
			im = _sub(_float(i), s->m);
			s->w[i] = _exp(_div(_neg(_pow2(im)), s->s2));
			s->wsum = _add(s->wsum, s->w[i]);
		}
	}
}

ALMAFilter *New(ISTVOID)
{
	struct alma_filter *s;
	
	s = (struct alma_filter *)ist_calloc(1, sizeof(struct alma_filter));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->Compute = Compute;
	s->ComputeParams = ComputeParams;
	s->period = IST_ALMA_PERIOD;
	s->offset = IST_ALMA_OFFSET;
	s->sigma = IST_ALMA_SIGMA;
	s->len = 0;
	s->buf = (ISTFLOAT *)ist_calloc(s->period, sizeof(ISTFLOAT));
	if (!s->buf) {
		goto EXIT_DELETE_ALMAFILTER;
	}
	ist_memset(s->buf, 0, sizeof(ISTFLOAT)*s->period);
	s->w = (ISTFLOAT *)ist_calloc(s->period, sizeof(ISTFLOAT));
	if (!s->w) {
		goto EXIT_DELETE_ALMAFILTER;
	}
	s->ComputeParams((ALMAFilter *)s);
	return (ALMAFilter *)s;

EXIT_DELETE_ALMAFILTER:
	Delete((ALMAFilter *)s);
	s = NULL;

EXIT:
	return (ALMAFilter *)NULL;
}

ISTVOID Delete(ALMAFilter *self)
{
	struct alma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct alma_filter *)self;
	if (s->w) {
		ist_free(s->w);
		s->w = NULL;
	}
	if (s->buf) {
		ist_free(s->buf);
		s->buf = NULL;
	}
	ist_free(s);
	s = NULL;

EXIT:
	return;
}

ISTFLOAT Process(ALMAFilter *self, ISTFLOAT data)
{
	struct alma_filter *s;
	ISTINT i;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct alma_filter *)self;
	for (i = 1; i < s->period; ++i) {
		s->buf[s->period - i] = s->buf[s->period - i - 1];
	}
	s->buf[0] = data;
	if (s->len < s->period) {
		s->len++;
	}
	return s->Compute(self);

EXIT:
	return 0;
}

ISTVOID SetPeriod(ALMAFilter *self, ISTINT period)
{
	struct alma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct alma_filter *)self;
	if (period <= 0) {
		period = IST_ALMA_PERIOD;
	}
	if ((period & 1) == 0) { // must be even
		++period;
	}
	s->buf = (ISTFLOAT *)ist_realloc(s->buf, sizeof(ISTFLOAT)*period);
	if (!s->buf) {
		goto EXIT;
	}
	s->w = (ISTFLOAT *)ist_realloc(s->w, sizeof(ISTFLOAT)*period);
	if (!s->w) {
		goto EXIT_DELETE_BUF;
	}
	s->period = period;
	if (s->len >= s->period) {
		s->len = s->period;
	}
	s->ComputeParams(self);
	return;

EXIT_DELETE_BUF:
	ist_free(s->buf);
	s->buf = NULL;
	s->period = 0;
	s->len = 0;

EXIT:
	return;
}

ISTVOID SetSigma(ALMAFilter *self, ISTINT sigma)
{
	struct alma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct alma_filter *)self;
	if (sigma <= 0) {
		sigma = IST_ALMA_SIGMA;
	}
	s->sigma = sigma;
	s->ComputeParams(self);
	return;

EXIT:
	return;
}

ISTVOID SetOffset(ALMAFilter *self, ISTFLOAT offset)
{
	struct alma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct alma_filter *)self;
	if (_lt(offset, 0)) {
		offset = 0;
	}
	s->offset = offset;
	s->ComputeParams(self);
	return;

EXIT:
	return;
}

ALMAFilter *IST_ALMAFilter(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		ThisClass.SetPeriod = SetPeriod;
		ThisClass.SetSigma = SetSigma;
		ThisClass.SetOffset =SetOffset;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_ALMA
