#include "ist_milieu.h"

#ifdef IST_MAGNET

#ifdef IST_SIRON
#  define MagnetEngine Softiron
#  define IST_MagnetEngine IST_Softiron
#else
#  define MagnetEngine Hardiron
#  define IST_MagnetEngine IST_Hardiron
#endif // IST_SIRON

#ifdef IST_MAGNET_DATA_FILTERED
#  ifdef IST_MAGNET_DATA_ALMA
#    define DataFilter ALMAFilter
#    define IST_DataFilter IST_ALMAFilter
#  endif // IST_MAGNET_DATA_ALMA
#endif // IST_MAGNET_DATA_FILTERED

#ifdef IST_MAGNET_BIAS_FILTERED
#  ifdef IST_MAGNET_BIAS_ALMA
#    define BiasFilter ALMAFilter
#    define IST_BiasFilter IST_ALMAFilter
#  endif // IST_MAGNET_BIAS_ALMA
#endif // IST_MAGNET_BIAS_FILTERED

#ifdef IST_MAGNET_RAD_FILTERED
#  ifdef IST_MAGNET_RAD_ALMA
#    define RadFilter ALMAFilter
#    define IST_RadFilter IST_ALMAFilter
#  endif // IST_MAGNET_RAD_ALMA
#endif // IST_MAGNET_RAD_FILTERED

#ifdef IST_MAGNET_VELOCITY_FILTERED
#  ifdef IST_MAGNET_VELOCITY_ALMA
#    define VelocityFilter ALMAFilter
#    define IST_VelocityFilter IST_ALMAFilter
#  endif // IST_MAGNET_RAD_ALMA
#endif // IST_MAGNET_VELOCITY_FILTERED

#ifdef IST_MAGNET_FILTERED
  typedef ISTVOID (*ist_NewFilters)(Magnet *);
  typedef ISTVOID (*ist_DeleteFilters)(Magnet *);
  typedef ISTVOID (*ist_DoFilter)(Magnet *, ISTFLOAT dT);
#endif // IST_MAGNET_FILTERED

#ifdef IST_MAGNET_VELOCITY
  typedef ISTVOID (*ist_ComputeVelocity)(Magnet *);
#endif // IST_MAGNET_VELOCITY

enum {
	T2 = 0,	// current time
	T1,		// current time - 1*time slice
	T0,		// current time - 2*time slice
	TMAX,
};

struct magnet {
	Magnet pub;
	MagnetEngine *engine;
	ISTFLOAT buf[3][TMAX];
	ISTBOOL is_first;
	ISTBOOL is_valid;
	ISTFLOAT dT;
#ifdef IST_MAGNET_FILTERED
	ist_NewFilters NewFilters;
	ist_DeleteFilters DeleteFilters;
	ist_DoFilter DoFilter;
	ist_DoFilter DoVelocityFilter;
#  ifdef IST_MAGNET_DATA_FILTERED
	DataFilter *data_filters[3];
#  endif // IST_MAGNET_DATA_FILTERED
#  ifdef IST_MAGNET_BIAS_FILTERED
	BiasFilter *bias_filters[3];
#  endif // IST_MAGNET_BIAS_FILTERED
#  ifdef IST_MAGNET_RAD_FILTERED
	RadFilter *rad_filter;
#  endif // IST_MAGNET_RAD_FILTERED 
#  ifdef IST_MAGNET_VELOCITY_FILTERED
	VelocityFilter *velocity_filters[3];
#  endif // IST_MAGNET_VELOCITY_FILTERED
#endif // IST_MAGNET_FILTERED
#ifdef IST_MAGNET_VELOCITY
	ist_ComputeVelocity ComputeVelocity;
	ISTBOOL is_velocity_valid;
#endif // IST_MAGNET_VELOCITY
};

STATIC Magnet *New(ISTVOID);
STATIC ISTVOID Delete(Magnet *self);
STATIC ISTBOOL Process(Magnet *self, ISTFLOAT data[3], ISTFLOAT dT);
#ifdef IST_MAGNET_FILTERED
  STATIC ISTVOID NewFilters(Magnet *self);
  STATIC ISTVOID DeleteFilters(Magnet *self);
  STATIC ISTVOID DoFilter(Magnet *self, ISTFLOAT dT);
  STATIC ISTVOID DoVelocityFilter(Magnet *self, ISTFLOAT dT);
#endif // IST_MAGNET_FILTERED
#ifdef IST_MAGNET_VELOCITY
  STATIC ISTVOID ComputeVelocity(Magnet *self);
#endif // IST_MAGNET_VELOCITY

STATIC Magnet ThisClass = {0};
STATIC Magnet *This = NULL;

#ifdef IST_MAGNET_FILTERED
ISTVOID NewFilters(Magnet *self)
{
	struct magnet *s = (struct magnet *)self;
#if defined(IST_MAGNET_DATA_FILTERED) \
	|| defined(IST_MAGNET_BIAS_FILTERED) \
	|| defined(IST_MAGNET_VELOCITY_FILTERED)
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
#ifdef IST_MAGNET_DATA_FILTERED
		s->data_filters[i] = IST_DataFilter()->New();
		if (!s->data_filters[i]) {
			goto EXIT_DELETE_FILTERS;
		}
		s->data_filters[i]->SetPeriod(s->data_filters[i], IST_MAGNET_DATA_ALMA_PERIOD);
#endif // IST_MAGNET_DATA_FILTERED
#ifdef IST_MAGNET_BIAS_FILTERED
		s->bias_filters[i] = IST_BiasFilter()->New();
		if (!s->bias_filters[i]) {
			goto EXIT_DELETE_FILTERS;
		}
		s->bias_filters[i]->SetPeriod(s->bias_filters[i], IST_MAGNET_BIAS_ALMA_PERIOD);
#endif // IST_MAGNET_BIAS_FILTERED
#ifdef IST_MAGNET_VELOCITY_FILTERED
		s->velocity_filters[i] = IST_VelocityFilter()->New();
		if (!s->velocity_filters[i]) {
			goto EXIT_DELETE_FILTERS;
		}
		s->velocity_filters[i]->SetPeriod(s->velocity_filters[i], IST_MAGNET_VELOCITY_ALMA_PERIOD);
#endif // IST_MAGNET_VELOCITY_FILTERED
	}
#endif // IST_MAGNET_DATA_FILTERED || IST_MAGNET_BIAS_FILTERED || IST_MAGNET_VELOCITY_FILTERED
#ifdef IST_MAGNET_RAD_FILTERED
	s->rad_filter = IST_RadFilter()->New();
	if (!s->rad_filter) {
		goto EXIT_DELETE_FILTERS;
	}
	s->rad_filter->SetPeriod(s->rad_filter, IST_MAGNET_RAD_ALMA_PERIOD);
#endif // IST_MAGNET_RAD_FILTERED
	return;

EXIT_DELETE_FILTERS:
	s->DeleteFilters(self);
	return;
}

ISTVOID DeleteFilters(Magnet *self)
{
	struct magnet *s = (struct magnet *)self;
#if defined(IST_MAGNET_DATA_FILTERED) \
	|| defined(IST_MAGNET_BIAS_FILTERED) \
	|| defined(IST_MAGNET_VELOCITY_FILTERED)
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
#ifdef IST_MAGNET_DATA_FILTERED
		if (s->data_filters[i]) {
			IST_DataFilter()->Delete(s->data_filters[i]);
			s->data_filters[i] = NULL;
		}
#endif // IST_MAGNET_DATA_FILTERED
#ifdef IST_MAGNET_BIAS_FILTERED
		if (s->bias_filters[i]) {
			IST_BiasFilter()->Delete(s->bias_filters[i]);
			s->bias_filters[i] = NULL;
		}
#endif // IST_MAGNET_BIAS_FILTERED
#ifdef IST_MAGNET_VELOCITY_FILTERED
		if (s->velocity_filters[i]) {
			IST_VelocityFilter()->Delete(s->velocity_filters[i]);
			s->velocity_filters[i] = NULL;
		}
#endif // IST_MAGNET_BIAS_FILTERED
	}
#endif // IST_MAGNET_DATA_FILTERED || IST_MAGNET_BIAS_FILTERED || IST_MAGNET_VELOCITY_FILTERED
#ifdef IST_MAGNET_RAD_FILTERED
	if (s->rad_filter) {
		IST_RadFilter()->Delete(s->rad_filter);
		s->rad_filter = NULL;
	}
#endif // IST_MAGNET_RAD_FILTERED
	return;
}

ISTVOID DoFilter(Magnet *self, ISTFLOAT dT)
{
	struct magnet *s = (struct magnet *)self;
#if defined(IST_MAGNET_DATA_FILTERED) \
	|| defined(IST_MAGNET_BIAS_FILTERED)
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
#ifdef IST_MAGNET_DATA_FILTERED
		self->Data[i] = s->data_filters[i]->Process(s->data_filters[i], self->Data[i]);
#endif // IST_MAGNET_DATA_FILTERED
#ifdef IST_MAGNET_BIAS_FILTERED
		self->Bias[i] = s->bias_filters[i]->Process(s->bias_filters[i], self->Bias[i]);
#endif // IST_MAGNET_BIAS_FILTERED
	}
#endif // IST_MAGNET_DATA_FILTERED || IST_MAGNET_BIAS_FILTERED
#ifdef IST_MAGNET_RAD_FILTERED
	self->Radius = s->rad_filter->Process(s->rad_filter, self->Radius);
#endif // IST_MAGNET_RAD_FILTERED
}

STATIC ISTVOID DoVelocityFilter(Magnet *self, ISTFLOAT dT)
{
	struct magnet *s = (struct magnet *)self;
#ifdef IST_MAGNET_VELOCITY_FILTERED
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
		self->Velocity[i] = s->velocity_filters[i]->Process(s->velocity_filters[i], self->Velocity[i]);
	}
#endif // IST_MAGNET_VELOCITY_FILTERED
}
#endif // IST_MAGNET_FILTERED

#ifdef IST_MAGNET_VELOCITY
ISTVOID ComputeVelocity(Magnet *self)
{
	struct magnet *s = (struct magnet *)self;
	ISTFLOAT H0[3];
	ISTFLOAT H1[3];
	ISTFLOAT H2[3];
	ISTFLOAT DH1[3];
	ISTFLOAT DH2[3];
	ISTFLOAT Raxis[3];
	ISTFLOAT HR1[3];
	ISTFLOAT HR2[3];
	ISTFLOAT temp[3];
	ISTFLOAT norm;
	ISTSHORT i, j;

	if (s->is_first) {
		s->is_first = ISTFALSE;
		for (i = 0; i < 3; ++i) {
			for (j = 0; j < TMAX; ++j) {
				s->buf[i][j] = self->Data[i];
			}
		}
	} else {
		for (i = 0; i < 3; ++i) {
			for (j = 0; j < TMAX - 1; ++j) {
				s->buf[i][j] = s->buf[i][j + 1];
			}
			s->buf[i][T0] = self->Data[i];
		}
	}
	if (_le(s->dT, _frac(1, 1000))) {
		goto EXIT;
	}
	for (i = 0; i < 3; ++i) {
		H0[i] = s->buf[i][T2];
		H1[i] = s->buf[i][T1];
		H2[i] = s->buf[i][T0];
		DH1[i] = _sub(H1[i], H0[i]);
		DH2[i] = _sub(H2[i], H1[i]);
	}
	norm = _magnitude(H0);
	norm = _add(norm, _magnitude(H1));
	norm = _add(norm, _magnitude(H2));
	norm = _div(norm, _float(3));
	_cross_mul(DH1, DH2, Raxis);
	_normalize(Raxis, Raxis);
	norm = _dot_mul(Raxis, H2);
	for (i = 0; i < 3; ++i) {
		HR2[i] = _mul(norm, Raxis[i]);
		HR2[i] = _sub(H2[i], HR2[i]);
	}
	norm = _dot_mul(Raxis, H1);
	for (i = 0; i < 3; ++i) {
		HR1[i] = _mul(norm, Raxis[i]);
		HR1[i] = _sub(H1[i], HR1[i]);
	}	
	for (i = 0; i < 3; ++i) {
		HR1[i] = _sub(HR2[i], HR1[i]);
	}
	norm = _dot_mul(HR2, HR2);
	_cross_mul(HR2, HR1, temp);
	for (i = 0; i < 3; ++i) {
		temp[i] = _div(temp[i], _mul(norm, s->dT));
		temp[i] = _neg(temp[i]);
	}
	for (i = 0; i < 3; ++i) {
		self->Velocity[i] = temp[i];
	}
	return;

EXIT:
	return;
}
#endif // IST_MAGNET_VELOCITY

Magnet *New(ISTVOID)
{
	struct magnet *s;

	s = (struct magnet *)ist_calloc(1, sizeof(struct magnet));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->dT = _frac(1, IST_MAGNET_HZ);
#ifdef IST_MAGNET_FILTERED
	s->NewFilters = NewFilters;
	s->DeleteFilters = DeleteFilters;
	s->DoFilter = DoFilter;
	s->DoVelocityFilter = DoVelocityFilter;
	s->NewFilters((Magnet *)s);
#endif // IST_MAGNET_FILTERED
#ifdef IST_MAGNET_VELOCITY
	s->ComputeVelocity = ComputeVelocity;
#endif // IST_MAGNET_VELOCITY
	s->engine = IST_MagnetEngine()->New();
	if (!s->engine) {
		goto EXIT_DELETE_MAGNET;
	}
	s->engine->Enable(s->engine);
	s->is_first = ISTTRUE;
	s->is_valid = ISTFALSE;
	return (Magnet *)s;

EXIT_DELETE_MAGNET:
	Delete((Magnet *)s);
	s = NULL;

EXIT:
	return (Magnet *)NULL;
}

ISTVOID Delete(Magnet *self)
{
	struct magnet *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct magnet *)self;
	if (s->engine) {
		IST_MagnetEngine()->Delete(s->engine);
		s->engine = NULL;
	}
#ifdef IST_MAGNET_FILTERED
	s->DeleteFilters(self);
#endif // IST_MAGNET_FILTERED
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTBOOL Process(Magnet *self, ISTFLOAT data[3], ISTFLOAT dT)
{
	struct magnet *s;
	ISTSHORT i, j;
	ISTBOOL ret;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct magnet *)self;
	if (_gt(dT, 0)) {
		s->dT = dT;
	}
	ret = s->engine->Process(s->engine, data);
	if (!ret) {
		goto EXIT;
	}
	self->Radius = s->engine->Radius;
	for (i = 0; i < 3; ++i) {
		self->Data[i] = s->engine->Data[i];
		self->Bias[i] = s->engine->Bias[i];
		for (j = 0; j < 3; ++j) {
			self->Matrix[i][j] = s->engine->Matrix[i][j];
		}
	}
	if (!s->is_valid) {
		s->is_valid = ISTTRUE;
	}
#ifdef IST_MAGNET_FILTERED
	s->DoFilter(self, dT);
#endif // IST_MAGNET_FILTERED
#ifdef IST_MAGNET_VELOCITY
	s->ComputeVelocity(self);
#  ifdef IST_MAGNET_VELOCITY_FILTERED
	s->DoVelocityFilter(self, s->dT);
#  endif // IST_MAGNET_VELOCITY_FILTERED
#endif // IST_MAGNET_VELOCITY
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

Magnet *IST_Magnet(ISTVOID)
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

#endif // IST_MAGNET
