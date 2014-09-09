#include "ist_milieu.h"

#ifdef IST_ACCEL

#ifdef IST_ACCEL_DATA_FILTERED
#  ifdef IST_ACCEL_DATA_ALMA
#    define DataFilter ALMAFilter
#    define IST_DataFilter IST_ALMAFilter
#  endif // IST_ACCEL_DATA_ALMA
#endif // IST_ACCEL_DATA_FILTERED

#ifdef IST_ACCEL_FILTERED
  typedef ISTVOID (*ist_NewFilters)(Acceleration *);
  typedef ISTVOID (*ist_DeleteFilters)(Acceleration *);
  typedef ISTVOID (*ist_DoFilter)(Acceleration *, ISTFLOAT data[3], ISTFLOAT dT);
#endif // IST_ACCEL_FILTERED

struct acceleration {
	Acceleration pub;
#ifdef IST_ACCEL_FILTERED
	ist_NewFilters NewFilters;
	ist_DeleteFilters DeleteFilters;
	ist_DoFilter DoFilter;
	ISTFLOAT dT;
#  ifdef IST_ACCEL_DATA_FILTERED
	DataFilter *data_filters[3];
#  endif // IST_ACCEL_DATA_FILTERED
#endif // IST_ACCEL_FILTERED
};

STATIC Acceleration *New(ISTVOID);
STATIC ISTVOID Delete(Acceleration *self);
STATIC ISTBOOL Process(Acceleration *self, ISTFLOAT data[3], ISTFLOAT dT);
#ifdef IST_ACCEL_FILTERED
  STATIC ISTVOID NewFilters(Acceleration *self);
  STATIC ISTVOID DeleteFilters(Acceleration *self);
  STATIC ISTVOID DoFilter(Acceleration *self, ISTFLOAT data[3], ISTFLOAT dT);
#endif // IST_ACCEL_FILTERED

STATIC Acceleration ThisClass = {0};
STATIC Acceleration *This = NULL;

#ifdef IST_ACCEL_FILTERED
ISTVOID NewFilters(Acceleration *self)
{
	struct acceleration *s = (struct acceleration *)self;
#ifdef IST_ACCEL_DATA_FILTERED
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
		s->data_filters[i] = IST_DataFilter()->New();
		if (!s->data_filters[i]) {
			goto EXIT_DELETE_FILTERS;
		}
	}
	return;

EXIT_DELETE_FILTERS:
	s->DeleteFilters(self);
#endif // IST_ACCEL_DATA_FILTERED
	return;
}

ISTVOID DeleteFilters(Acceleration *self)
{
	struct acceleration *s = (struct acceleration *)self;
#ifdef IST_ACCEL_DATA_FILTERED
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
		if (s->data_filters[i]) {
			IST_DataFilter()->Delete(s->data_filters[i]);
			s->data_filters[i] = NULL;
		}
	}
#endif // IST_ACCEL_DATA_FILTERED
}

ISTVOID DoFilter(Acceleration *self, ISTFLOAT data[3], ISTFLOAT dT)
{
	struct acceleration *s = (struct acceleration *)self;
#ifdef IST_ACCEL_DATA_FILTERED
	ISTSHORT i;

	if (_gt(dT, 0)) {
		s->dT = dT;
	}
	for (i = 0; i < 3; ++i) {
		self->Data[i] = s->data_filters[i]->Process(s->data_filters[i], data[i]);
	}
#endif // IST_ACCEL_DATA_FILTERED
}
#endif // IST_MAGNET_FILTERED

Acceleration *New(ISTVOID)
{
	struct acceleration *s;

	s = (struct acceleration *)ist_calloc(1, sizeof(struct acceleration));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
#ifdef IST_ACCEL_FILTERED
	s->NewFilters = NewFilters;
	s->DeleteFilters = DeleteFilters;
	s->DoFilter = DoFilter;
	s->dT = _frac(1, IST_ACCEL_HZ);
	s->NewFilters((Acceleration *)s);
#endif // IST_ACCEL_FILTERED
	return (Acceleration *)s;

EXIT:
	return (Acceleration *)NULL;
}

ISTVOID Delete(Acceleration *self)
{
	struct acceleration *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct acceleration *)self;
#ifdef IST_ACCEL_FILTERED
	s->DeleteFilters(self);
#endif // IST_ACCEL_FILTERED
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTBOOL Process(Acceleration *self, ISTFLOAT data[3], ISTFLOAT dT)
{
	struct acceleration *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct acceleration *)self;
#ifdef IST_ACCEL_FILTERED
	s->DoFilter(self, data, dT);
#else
	ist_memcpy(self->Data, data, sizeof(data));
#endif
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

Acceleration* IST_Acceleration(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_ACCEL
