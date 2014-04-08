#include "ist_milieu.h"

#ifdef IST_MMA

struct mma_filter {
	MMAFilter pub;
	RMAFilter *filter;
};

STATIC MMAFilter *New();
STATIC ISTVOID Delete(MMAFilter *self);
STATIC ISTFLOAT Process(MMAFilter *self, ISTFLOAT data);
STATIC ISTVOID SetPeriod(MMAFilter *self, ISTINT period);

STATIC MMAFilter ThisClass = {0};
STATIC MMAFilter *This = NULL;

MMAFilter *New()
{
	struct mma_filter *s;

	s = (struct mma_filter *)ist_calloc(1, sizeof(struct mma_filter));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->filter = IST_RMAFilter()->New();
	if (!s->filter) {
		goto EXIT;
	}
	s->filter->SetWeight(s->filter, IST_MMA_WEIGHT);
	s->filter->SetPeriod(s->filter, IST_MMA_PERIOD);
	return (MMAFilter *)s;

EXIT:
	return (MMAFilter *)NULL;
}

ISTVOID Delete(MMAFilter *self)
{
	struct mma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct mma_filter *)self;
	if (s->filter) {
		IST_RMAFilter()->Delete(s->filter);
		s->filter = NULL;
	}
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTFLOAT Process(MMAFilter *self, ISTFLOAT data)
{
	struct mma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct mma_filter*)self;
	return s->filter->Process(s->filter, data);

EXIT:
	return 0;
}

ISTVOID SetPeriod(MMAFilter *self, ISTINT period)
{
	struct mma_filter *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct mma_filter *)self;
	s->filter->SetPeriod(s->filter, period < 0 ? -period : period);
	return;

EXIT:
	return;
}

MMAFilter *IST_MMAFilter()
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

#endif // IST_MMA
