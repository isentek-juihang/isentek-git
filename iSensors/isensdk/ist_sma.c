#include "ist_milieu.h"

#ifdef IST_SMA

typedef ISTVOID (*ist_NewBuffer)(SMAFilter *, ISTINT period);
typedef ISTVOID (*ist_DeleteBuffer)(SMAFilter *);

struct sma_filter {
	SMAFilter pub;
	ist_NewBuffer NewBuffer;
	ist_DeleteBuffer DeleteBuffer;
	ISTFLOAT *buf;
	ISTINT len;
	ISTINT period;
	ISTFLOAT avg;
};

STATIC SMAFilter *New(ISTVOID);
STATIC ISTVOID Delete(SMAFilter *self);
STATIC ISTFLOAT Process(SMAFilter *self, ISTFLOAT data);
STATIC ISTVOID SetPeriod(SMAFilter *self, ISTINT period);
STATIC ISTVOID NewBuffer(SMAFilter *self, ISTINT period);
STATIC ISTVOID DeleteBuffer(SMAFilter *self);

STATIC SMAFilter ThisClass = {0};
STATIC SMAFilter *This = NULL;

ISTVOID NewBuffer(SMAFilter *self, ISTINT period)
{
	struct sma_filter *s = (struct sma_filter *)self;
	ISTINT i;

	if (period <= 0) {
		period = IST_SMA_PERIOD;
	}
	s->period = period;
	if (s->len > s->period) {
		s->len = s->period;
		if (s->buf) {
			for (i = 0; i < s->len; ++i) {
				s->avg = _add(s->avg, s->buf[i]);
			}
			s->avg = _div(s->avg, _float(s->len));
		}
	}
	s->buf = (ISTFLOAT *)ist_realloc(s->buf, sizeof(ISTFLOAT)*s->period);
	if (!s->buf) {
		goto EXIT;
	}
	return;

EXIT:
	return;
}

ISTVOID DeleteBuffer(SMAFilter *self)
{
	struct sma_filter *s = (struct sma_filter *)self;
	
	if (s->buf) {
		ist_free(s->buf);
		s->buf = NULL;
	}
	s->period = 0;
	s->len = 0;
	s->avg = 0;
}

SMAFilter *New(ISTVOID)
{
	struct sma_filter *s;
	
	s = (struct sma_filter *)ist_calloc(1, sizeof(struct sma_filter));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->NewBuffer = NewBuffer;
	s->DeleteBuffer = DeleteBuffer;
	s->avg = 0;
	s->len = 0;
	s->NewBuffer((SMAFilter *)s, IST_SMA_PERIOD);
	return (SMAFilter *)s;

EXIT:
	return (SMAFilter *)NULL;
}

ISTVOID Delete(SMAFilter *self)
{
	struct sma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct sma_filter *)self;
	s->DeleteBuffer(self);
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTFLOAT Process(SMAFilter *self, ISTFLOAT data)
{
	struct sma_filter *s;
	ISTINT i;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	if (s->len == 0) {
		s->avg = data;
	} else if (s->len < s->period){
		s->avg = _div(_add(_mul(s->avg, _float(s->len)), data), _float(s->len + 1));
	} else {
		s->avg = _add(_sub(s->avg, _div(s->buf[s->period - 1], _float(s->period))), _div(data, _float(s->period)));
	}
	for (i = 1; i < s->period; ++i) {
		s->buf[s->period - i] = s->buf[s->period - 1 - i];
	}
	s->buf[0] = data;
	if (s->len < s->period) {
		s->len++;
	}
	return s->avg;

EXIT:
	return 0;
}

ISTVOID SetPeriod(SMAFilter *self, ISTINT period)
{
	struct sma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s->NewBuffer(self, period);
	return;

EXIT:
	return;
}

SMAFilter *IST_SMAFilter(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		ThisClass.SetPeriod = SetPeriod;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_SMA
