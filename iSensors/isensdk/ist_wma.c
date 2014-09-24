#include "ist_milieu.h"

#ifdef IST_WMA

typedef ISTVOID (*ist_NewBuffer)(WMAFilter *, ISTINT period);
typedef ISTVOID (*ist_DeleteBuffer)(WMAFilter *);

struct wma_filter {
	WMAFilter pub;
	ist_NewBuffer NewBuffer;
	ist_DeleteBuffer DeleteBuffer;
	ISTFLOAT *buf;
	ISTINT period;
	ISTINT len;
	ISTFLOAT total;
	ISTFLOAT numerator;
	ISTFLOAT avg;
};

STATIC WMAFilter *New(ISTVOID);
STATIC ISTVOID Delete(WMAFilter *self);
STATIC ISTFLOAT Process(WMAFilter *self, ISTFLOAT data);
STATIC ISTVOID SetPeriod(WMAFilter *self, ISTINT period);
STATIC ISTVOID NewBuffer(WMAFilter *self, ISTINT period);
STATIC ISTVOID DeleteBuffer(WMAFilter *self);

STATIC WMAFilter ThisClass = {0};
STATIC WMAFilter *This = NULL;

ISTVOID NewBuffer(WMAFilter *self, ISTINT period)
{
	struct wma_filter *s = (struct wma_filter *)self;
	ISTINT i;

	if (period <= 0) {
		period = IST_SMA_PERIOD;
	}
	s->period = period;
	if (s->len > s->period) {
		s->len = s->period;
		if (s->buf) {
			for (i = 0; i < s->len; ++i) {
				s->numerator = _add(s->numerator, _mul(_float(i + 1), s->buf[s->len - i - 1]));
				s->total = _add(s->total, s->buf[s->len - i - 1]);
			}
			s->avg = _div(s->numerator, _float(s->len * (s->len + 1) / 2));
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

ISTVOID DeleteBuffer(WMAFilter *self)
{
	struct wma_filter *s = (struct wma_filter *)self;

	if (s->buf) {
		ist_free(s->buf);
		s->buf = NULL;
	}
	s->period = 0;
	s->len = 0;
	s->avg = 0;
	s->total = 0;
	s->numerator = 0;
}

WMAFilter *New(ISTVOID)
{
	struct wma_filter *s;

	s = (struct wma_filter *)ist_calloc(1, sizeof(struct wma_filter));
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
	s->total = 0;
	s->numerator = 0;
	s->NewBuffer((WMAFilter *)s, IST_WMA_PERIOD);
	return (WMAFilter *)s;

EXIT:
	return (WMAFilter *)NULL;
}

ISTVOID Delete(WMAFilter *self)
{
	struct wma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct wma_filter *)self;
	s->DeleteBuffer(self);
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTFLOAT Process(WMAFilter *self, ISTFLOAT data)
{
	struct wma_filter *s;
	ISTINT i;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	if (s->len == 0) {
		s->total = data;
		s->numerator = data;
		s->avg = data;
	} else if (s->len < s->period){
		s->total = _add(s->total, data);
		s->numerator = _add(s->numerator, _mul(_float(s->len + 1), data));
		s->avg = _div(s->numerator, _float((s->len + 1) * (s->len + 2) / 2));
	} else {
		s->numerator = _add(s->numerator, _mul(_float(s->len), data));
		s->numerator = _sub(s->numerator, s->total);
		s->total = _add(s->total, data);
		s->total = _sub(s->total, s->buf[s->len - 1]);
		s->avg = _div(s->numerator, _float(s->len * (s->len + 1) / 2));
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

ISTVOID SetPeriod(WMAFilter *self, ISTINT period)
{
	struct wma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s->NewBuffer(self, period);
	return;

EXIT:
	return;
}

WMAFilter *IST_WMAFilter(ISTVOID)
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

#endif // IST_WMA
