#include "ist_milieu.h"

#ifdef IST_ROTATION

#ifdef IST_MAGNET
#  define MagnetEngine Magnet
#  define IST_MagnetEngine IST_Magnet
#endif // IST_MAGNET

#ifdef IST_ACCEL
#  define AccelEngine Acceleration
#  define IST_AccelEngine IST_Acceleration
#endif // IST_ACCEL

#ifdef IST_ROTATION_ANGULAR_FILTERED
#  ifdef IST_ROTATION_ANGULAR_ALMA
#    define AngularFilter ALMAFilter
#    define IST_AngularFilter IST_ALMAFilter
#  endif // IST_ROTATION_ANGULAR_ALMA
#endif // IST_ROTATION_ANGULAR_FILTERED

#ifdef IST_ROTATION_VELOCITY_FILTERED
#  ifdef IST_ROTATION_VELOCITY_ALMA
#    define VelocityFilter ALMAFilter
#    define IST_VelocityFilter IST_ALMAFilter
#  endif // IST_ROTATION_VELOCITY_ALMA
#endif // IST_ROTATION_VELOCITY_FILTERED

#ifdef IST_ROTATION_FILTERED
  typedef ISTVOID (*ist_NewFilters)(Rotation *);
  typedef ISTVOID (*ist_DeleteFilters)(Rotation *);
  typedef ISTVOID (*ist_DoFilter)(Rotation *, ISTFLOAT dT);
#endif // IST_ROTATION_FILTERED
typedef ISTVOID (*ist_Compute)(Rotation *);

enum {
	T2 = 0,	// current time
	T1,		// current time - 1*time slice
	T0,		// current time - 2*time slice
	TMAX,
};

struct rotation {
	Rotation pub;
	ISTFLOAT mdata[3];
	ISTFLOAT gdata[3];
	ISTFLOAT dT;
	MagnetEngine *magnet;
	AccelEngine *accel;
	ist_Compute ComputeAngular;
	ISTBOOL is_valid;
	ISTBOOL is_inited;
#ifdef IST_ROTATION_VELOCITY
	ISTFLOAT mat[TMAX][3][3];
#endif // IST_ROTATION_VELOCITY
#ifdef IST_ROTATION_FILTERED
	ist_NewFilters NewFilters;
	ist_DeleteFilters DeleteFilters;
	ist_DoFilter DoFilter;
	ist_DoFilter DoVelocityFilter;
#  ifdef IST_ROTATION_ANGULAR_FILTERED
	AngularFilter *angular_filters[3];
#  endif // IST_ROTATION_ANGULAR_FILTERED
#  ifdef IST_ROTATION_VELOCITY_FILTERED
	VelocityFilter *velocity_filters[3];
#  endif // IST_ROTATION_VELOCITY_FILTERED
#endif // IST_ROTATION_FILTERED
#ifdef IST_ROTATION_VELOCITY
	ist_Compute ComputeVelocity;
#endif // IST_MAGNET_VELOCITY
};

STATIC Rotation *New(ISTVOID);
STATIC ISTVOID Delete(Rotation *self);
STATIC ISTBOOL Process(Rotation *self, ISTFLOAT mdata[3], ISTFLOAT gdata[3], ISTFLOAT dT);
STATIC ISTVOID ComputeAngular(Rotation *self);
#ifdef IST_ROTATION_FILTERED
  STATIC ISTVOID NewFilters(Rotation *self);
  STATIC ISTVOID DeleteFilters(Rotation *self);
  STATIC ISTVOID DoFilter(Rotation *self, ISTFLOAT dT);
  STATIC ISTVOID DoVelocityFilter(Rotation *self, ISTFLOAT dT);
#endif // IST_ROTATION_FILTERED
#ifdef IST_ROTATION_VELOCITY
  STATIC ISTVOID ComputeVelocity(Rotation *self);
#endif // IST_MAGNET_VELOCITY

STATIC Rotation ThisClass = {0};
STATIC Rotation *This = NULL;

#ifdef IST_ROTATION_QUATERNION
INLINE ISTBOOL ComputeQuaternion(Rotation *self)
{
	struct rotation *s = (struct rotation *)self;
	ISTFLOAT Gd[3];
	ISTFLOAT Bd[3];
	ISTFLOAT GdN[3];
	ISTFLOAT BdN[3];
	ISTFLOAT Cd[3];
	ISTFLOAT CdN[3];
	ISTFLOAT Td[3];
	ISTSHORT i;

	if (_eq(s->dT, 0)) {
		goto EXIT;
	}
	for (i = 0; i < 3; ++i) {
		Gd[i] = s->gdata[i];
		Bd[i] = s->mdata[i];
	}
	_normalize(Gd, GdN);
	_normalize(Bd, BdN);
	_cross_mul(Gd, Bd, Cd);
	_normalize(Cd, CdN);
	if (_lt(_magnitude(Cd), _frac(1, 10))) {
		// device is close to free fall (or in space?), or close to
		// magnetic north pole. Typical values are > 100.
		goto EXIT;
	}
	_cross_mul(GdN, CdN, Td);
	for (i = 0; i < 3; ++i) {
		self->Matrix[i][0] = _neg(Td[i]);
		self->Matrix[i][1] = CdN[i];
		self->Matrix[i][2] = BdN[i];
	}
	_quaternion(self->Matrix, self->Quaternion);
	_angular_q(self->Quaternion, self->Angular);
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}
#endif // IST_ROTATION_QUATERNION

#ifdef IST_ROTATION_MATRIX
#define SENSOR_SINGULAR_NONE	0x00000000
#define SENSOR_SINGULAR_0		0x00000001
#define SENSOR_SINGULAR_1		0x00000002
#define SENSOR_SINGULAR_2		0x00000004
#define SENSOR_SINGULAR_3		0x00000008
#define SENSOR_SINGULAR_4		0x0000000F
#define SENSOR_SINGULAR_5		0x00000010
#define SENSOR_SINGULAR_6		0x00000020
#define SENSOR_SINGULAR_7		0x00000040
#define SENSOR_SINGULAR_MASK	\
	(SENSOR_SINGULAR_0|SENSOR_SINGULAR_1|SENSOR_SINGULAR_2 \
	|SENSOR_SINGULAR_3|SENSOR_SINGULAR_4|SENSOR_SINGULAR_5 \
	|SENSOR_SINGULAR_6|SENSOR_SINGULAR_7)

#define IS_FLOAT_ROUND(_a_, _b_, _r_)	\
	_le(_abs(_sub(_a_, _b_)), _r_)

INLINE ISTUBIT32 GravitySingularFlags(ISTFLOAT x, ISTFLOAT y)
{
	ISTFLOAT eps = _frac(1, 100);

	if (IS_FLOAT_ROUND(x, _one, eps) && IS_FLOAT_ROUND(y, 0, eps)) {
		return (SENSOR_SINGULAR_0|SENSOR_SINGULAR_1);
	}
	if (IS_FLOAT_ROUND(x, 0, eps) && IS_FLOAT_ROUND(y, _one, eps)) {
		return (SENSOR_SINGULAR_2|SENSOR_SINGULAR_3);
	}
	if (IS_FLOAT_ROUND(x, _neg(_one), eps) && IS_FLOAT_ROUND(y, 0, eps)) {
		return (SENSOR_SINGULAR_4|SENSOR_SINGULAR_5);
	}
	if (IS_FLOAT_ROUND(x, 0, eps) && IS_FLOAT_ROUND(y, _neg(_one), eps)) {
		return (SENSOR_SINGULAR_6|SENSOR_SINGULAR_7);
	}
	return SENSOR_SINGULAR_NONE;
}

INLINE ISTUBIT32 MagnetSingularFlags(ISTFLOAT x, ISTFLOAT y)
{
	if (IS_FLOAT_ROUND(_add(_pow2(x), _pow2(y)), _one, _frac(1, 100))) {
		if (_ge(x, 0) && _ge(y, 0)) {
			return (SENSOR_SINGULAR_0|SENSOR_SINGULAR_7);
		} else if (_ge(x, 0) && _lt(y, 0)) {
			return (SENSOR_SINGULAR_1|SENSOR_SINGULAR_2);
		} else if (_lt(x, 0) && _lt(y, 0)) {
			return (SENSOR_SINGULAR_3|SENSOR_SINGULAR_4|SENSOR_SINGULAR_5);
		} else if (_lt(x, 0) && _ge(y, 0)) {
			return SENSOR_SINGULAR_6;
		}
	}
	return SENSOR_SINGULAR_NONE;
}

INLINE ISTUBIT32 AngularSingularFlags(ISTFLOAT x, ISTFLOAT y, ISTFLOAT z)
{
	ISTFLOAT eps = _frac(1, 100);

	if (IS_FLOAT_ROUND(x, _neg(_pi_2), eps) && IS_FLOAT_ROUND(y, _neg(_pi_2), eps) && IS_FLOAT_ROUND(z, 0, eps)) {
		return (SENSOR_SINGULAR_0|SENSOR_SINGULAR_1);
	}
	if (IS_FLOAT_ROUND(x, 0, eps) && IS_FLOAT_ROUND(y, _neg(_pi_2), eps) && IS_FLOAT_ROUND(z, _neg(_pi_2), eps)) {
		return SENSOR_SINGULAR_0;
	}
	if (IS_FLOAT_ROUND(x, _pi_2, eps) && IS_FLOAT_ROUND(y, _neg(_pi_2), eps) && IS_FLOAT_ROUND(z, _pi, eps)) {
		return (SENSOR_SINGULAR_0|SENSOR_SINGULAR_1);
	}
	if (IS_FLOAT_ROUND(x, _pi, eps) && IS_FLOAT_ROUND(y, _neg(_pi_2), eps) && IS_FLOAT_ROUND(z, _pi_2, eps)) {
		return SENSOR_SINGULAR_0;
	}
	if (IS_FLOAT_ROUND(x, 0, eps) && IS_FLOAT_ROUND(y, _neg(_pi_2), eps) && IS_FLOAT_ROUND(z, _pi_2, eps)) {
		return SENSOR_SINGULAR_1;
	}
	if (IS_FLOAT_ROUND(x, _pi, eps) && IS_FLOAT_ROUND(y, _neg(_pi_2), eps) && IS_FLOAT_ROUND(z, _neg(_pi_2), eps)) {
		return SENSOR_SINGULAR_1;
	}
	if (IS_FLOAT_ROUND(x, _neg(_pi_2), eps) && IS_FLOAT_ROUND(y, 0, eps) && IS_FLOAT_ROUND(z, 0, eps)) {
		return SENSOR_SINGULAR_2;
	}
	if (IS_FLOAT_ROUND(x, _pi_2, eps) && IS_FLOAT_ROUND(y, _pi, eps) && IS_FLOAT_ROUND(z, _pi, eps)) {
		return SENSOR_SINGULAR_2;
	}
	if (IS_FLOAT_ROUND(x, _neg(_pi_2), eps) && IS_FLOAT_ROUND(y, 0, eps) && IS_FLOAT_ROUND(z, _pi, eps)) {
		return SENSOR_SINGULAR_3;
	}
	if (IS_FLOAT_ROUND(x, _pi_2, eps) && IS_FLOAT_ROUND(y, _pi, eps) && IS_FLOAT_ROUND(z, 0, eps)) {
		return SENSOR_SINGULAR_3;
	}
	if (IS_FLOAT_ROUND(x, _neg(_pi_2), eps) && IS_FLOAT_ROUND(y, _pi_2, eps) && IS_FLOAT_ROUND(z, 0, eps)) {
		return SENSOR_SINGULAR_4;
	}
	if (IS_FLOAT_ROUND(x, 0, eps) && IS_FLOAT_ROUND(y, _pi_2, eps) && IS_FLOAT_ROUND(z, _pi_2, eps)) {
		return SENSOR_SINGULAR_4;
	}
	if (IS_FLOAT_ROUND(x, _pi_2, eps) && IS_FLOAT_ROUND(y, _pi_2, eps) && IS_FLOAT_ROUND(z, _pi, eps)) {
		return SENSOR_SINGULAR_4;
	}
	if (IS_FLOAT_ROUND(x, _pi, eps) && IS_FLOAT_ROUND(y, _pi_2, eps) && IS_FLOAT_ROUND(z, _neg(_pi_2), eps)) {
		return SENSOR_SINGULAR_4;
	}
	if (IS_FLOAT_ROUND(x, _neg(_pi_2), eps) && IS_FLOAT_ROUND(y, _pi_2, eps) && IS_FLOAT_ROUND(z, _pi, eps)) {
		return SENSOR_SINGULAR_5;
	}
	if (IS_FLOAT_ROUND(x, 0, eps) && IS_FLOAT_ROUND(y, _pi_2, eps) && IS_FLOAT_ROUND(z, _neg(_pi_2), eps)) {
		return SENSOR_SINGULAR_5;
	}
	if (IS_FLOAT_ROUND(x, _pi_2, eps) && IS_FLOAT_ROUND(y, _pi_2, eps) && IS_FLOAT_ROUND(z, 0, eps)) {
		return SENSOR_SINGULAR_5;
	}
	if (IS_FLOAT_ROUND(x, _pi, eps) && IS_FLOAT_ROUND(y, _pi_2, eps) && IS_FLOAT_ROUND(z, _pi_2, eps)) {
		return SENSOR_SINGULAR_5;
	}
	if (IS_FLOAT_ROUND(x, _neg(_pi_2), eps) && IS_FLOAT_ROUND(y, _pi, eps) && IS_FLOAT_ROUND(z, 0, eps)) {
		return (SENSOR_SINGULAR_6|SENSOR_SINGULAR_7);
	}
	if (IS_FLOAT_ROUND(x, _pi_2, eps) && IS_FLOAT_ROUND(y, 0, eps) && IS_FLOAT_ROUND(z, _pi, eps)) {
		return (SENSOR_SINGULAR_6|SENSOR_SINGULAR_7);
	}
	return SENSOR_SINGULAR_NONE;
}

INLINE ISTBOOL IsSingular(ISTFLOAT angle[3], ISTFLOAT gdata[3], ISTFLOAT mdata[3])
{
	ISTUBIT32 flags = SENSOR_SINGULAR_MASK;
	ISTFLOAT g[3];
	ISTFLOAT m[3];

	_normalize(gdata, g);
	_normalize(mdata, m);

	flags &= GravitySingularFlags(g[0], g[1]);
	flags &= MagnetSingularFlags(m[0], m[1]);
	flags &= AngularSingularFlags(angle[0], angle[1], angle[2]);
	if (!(flags & SENSOR_SINGULAR_MASK)) {
		goto EXIT;
	}
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

INLINE ISTBOOL ComputeMatrix(Rotation *self)
{
	struct rotation *s = (struct rotation *)self;
	ISTFLOAT ax;
	ISTFLOAT ay;
	ISTFLOAT az;
	ISTFLOAT mx;
	ISTFLOAT my;
	ISTFLOAT mz;
	ISTFLOAT dx;
	ISTFLOAT dy;
	ISTFLOAT dz;
	ISTFLOAT sx;
	ISTFLOAT sy;
	ISTFLOAT sz;
	ISTFLOAT cx;
	ISTFLOAT cy;
	ISTFLOAT cz;
	ISTFLOAT angle[3];
	ISTFLOAT aout[3];
	ISTFLOAT mout[3];
	ISTSHORT i;

	if (_eq(s->dT, 0)) {
		goto EXIT;
	}
	for (i =0; i < 3; ++i) {
		aout[i] = s->gdata[i];
		mout[i] = s->mdata[i];
	}
	ax = aout[0];
	ay = aout[1];
	az = aout[2];
	mx = mout[0];
	my = mout[1];
	mz = mout[2];
	dx = _atan2(ay, az);
	sx = _sin(dx);
	cx = _cos(dx);
	dy = _atan(_neg(_div(ax, _add(_mul(ay, sx), _mul(az, cx)))));
	sy = _sin(dy);
	cy = _cos(dy);
	dz = _atan2(_sub(_mul(mz, sx), _mul(my, cx)), _add3(_mul(mx, cy), _mul3(my, sy, sx), _mul3(mz, sy, cx)));
	sz = _sin(dz);
	cz = _cos(dz);
	angle[0] = dx;
	angle[1] = dy;
	angle[2] = dz;
	if (!IsSingular(angle, aout, mout)) {
		for (i = 0; i < 3; ++i) {
			self->Angular[i] = angle[i];
		}
		/*!
		 * R(r) = [ 1      0      0   ]  R(p) = [ cos(p)  0 -sin(p) ]  R(y) = [  cos(y)  sin(y)  0 ]
		 *        [ 0  cos(r)  sin(r) ]         [   0     1    0    ]         [ -sin(y)  cos(y)  0 ]
		 *        [ 0 -sin(r)  cos(r) ]         [ sin(p)  0  cos(p) ]         [    0       0     1 ]
		 *        
		 * IF s = sin, c = cos, 1 = r, 2 = p, 3 = y
		 * 
		 * R = R(r)R(p)R(y) = [      c2c3        c2s3      -s2  ]
		 *                    [ s1s2c3-c1s3  s1s2s3+c1c3  s1c2  ]
		 *                    [ c1s2c3+s1s3  c1s2s3-s1c3  c1c2  ]
		 */
		self->Matrix[0][0] = _mul(cy, cz);
		self->Matrix[0][1] = _mul(cy, sz);
		self->Matrix[0][2] = _neg(sy);
		self->Matrix[1][0] = _sub(_mul3(sx, sy, cz), _mul(cx, sz));
		self->Matrix[1][1] = _add(_mul3(sx, sy, sz), _mul(cx, cz));
		self->Matrix[1][2] = _mul(sx, cy);
		self->Matrix[2][0] = _add(_mul3(cx, sy, cz), _mul(sx, sz));
		self->Matrix[2][1] = _sub(_mul3(cx, sy, sz), _mul(sx, cz));
		self->Matrix[2][2] = _mul(cx, cy);
		_quaternion(self->Matrix, self->Quaternion);
		_angular_q(self->Quaternion, self->Angular);	
	}
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}
#endif // IST_ROTATION_MATRIX

ISTVOID ComputeAngular(Rotation *self)
{
	struct rotation *s = (struct rotation *)self;
	ISTBOOL ret;

#ifdef IST_ROTATION_MATRIX
	ret = ComputeMatrix(self);
#elif defined(IST_ROTATION_QUATERNION)
	ret = ComputeQuaternion(self);
#endif
	if (!ret) {
		goto EXIT;
	}
	if (!s->is_valid) {
		s->is_valid = ISTTRUE;
	}
	return;

EXIT:
	return;
}

#ifdef IST_ROTATION_FILTERED
ISTVOID NewFilters(Rotation *self)
{
	struct rotation *s = (struct rotation *)self;
#if defined(IST_ROTATION_ANGULAR_FILTERED) \
	|| defined(IST_ROTATION_VELOCITY_FILTERED)
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
#ifdef IST_ROTATION_ANGULAR_FILTERED
		s->angular_filters[i] = IST_AngularFilter()->New();
		if (!s->angular_filters[i]) {
			goto EXIT_DELETE_FILTERS;
		}
		s->angular_filters[i]->SetPeriod(s->angular_filters[i], IST_ROTATION_ANGULAR_ALMA_PERIOD);
#endif // IST_ROTATION_ANGULAR_FILTERED
#ifdef IST_ROTATION_VELOCITY_FILTERED
		s->velocity_filters[i] = IST_VelocityFilter()->New();
		if (!s->velocity_filters[i]) {
			goto EXIT_DELETE_FILTERS;
		}
		s->velocity_filters[i]->SetPeriod(s->velocity_filters[i], IST_ROTATION_VELOCITY_ALMA_PERIOD);
#endif // IST_ROTATION_VELOCITY_FILTERED
	}
	return;

EXIT_DELETE_FILTERS:
	s->DeleteFilters(self);
#endif // IST_ROTATION_ANGULAR_FILTERED || IST_ROTATION_VELOCITY_FILTERED
	return;
}

ISTVOID DeleteFilters(Rotation *self)
{
	struct rotation *s = (struct rotation *)self;
#if defined(IST_ROTATION_ANGULAR_FILTERED) \
	|| defined(IST_ROTATION_VELOCITY_FILTERED)
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
#ifdef IST_ROTATION_ANGULAR_FILTERED
		if (s->angular_filters[i]) {
			IST_AngularFilter()->Delete(s->angular_filters[i]);
			s->angular_filters[i] = NULL;
		}
#endif // IST_ROTATION_ANGULAR_FILTERED
#ifdef IST_ROTATION_VELOCITY_FILTERED
		if (s->velocity_filters[i]) {
			IST_VelocityFilter()->Delete(s->velocity_filters[i]);
			s->velocity_filters[i] = NULL;
		}
#endif // IST_ROTATION_VELOCITY_FILTERED
	}
#endif // IST_ROTATION_ANGULAR_FILTERED || IST_ROTATION_VELOCITY_FILTERED
	return;
}

ISTVOID DoFilter(Rotation *self, ISTFLOAT dT)
{
	struct rotation *s = (struct rotation *)self;
#ifdef IST_ROTATION_ANGULAR_FILTERED
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
		self->Angular[i] = s->angular_filters[i]->Process(s->angular_filters[i], self->Angular[i]);
	}
#endif // IST_ROTATION_ANGULAR_FILTERED
	return;
}

ISTVOID DoVelocityFilter(Rotation *self, ISTFLOAT dT)
{
	struct rotation *s = (struct rotation *)self;
#ifdef IST_ROTATION_VELOCITY_FILTERED
	ISTSHORT i;

	for (i = 0; i < 3; ++i) {
		self->Velocity[i] = s->velocity_filters[i]->Process(s->velocity_filters[i], self->Velocity[i]);
	}
#endif // IST_ROTATION_ANGULAR_FILTERED
	return;
}
#endif // IST_ROTATION_FILTERED

#ifdef IST_ROTATION_VELOCITY
#ifdef IST_ROTATION_MATRIX
INLINE ISTVOID ComputeVelocityFromMatrix(Rotation *self)
{
	// from T1 to T0
	struct rotation *s = (struct rotation *)self;
	ISTFLOAT m[3][3];
	ISTFLOAT r[3][3];
	ISTSHORT i, j;

	if (s->is_inited == ISTFALSE) {
		for (i = 0; i < 3; ++i) {
			for (j = 0; j < 3; ++j) {
				s->mat[T2][i][j] = s->mat[T1][i][j] = s->mat[T0][i][j] = self->Matrix[i][j];
			}
		}
		s->is_inited = ISTTRUE;
		goto EXIT;
	} else {
		for (i = 0; i < 3; ++i) {
			for (j = 0; j < 3; ++j) {
				s->mat[T2][i][j] = s->mat[T1][i][j];
				s->mat[T1][i][j] = s->mat[T0][i][j];
				s->mat[T0][i][j] = self->Matrix[i][j];
			}
		}
	}
	if (_le(_abs(s->dT), _frac(1, 1000))) {
		goto EXIT;
	}
	_transport(s->mat[T1], m);
	_mat_mul(s->mat[T0], m, r);
	for (i = 0; i < 3; ++i) {
		r[i][i] = _sub(r[i][i], _one);
	}
	if (_gt(_abs(s->dT), _frac(1, 100000))) {
		self->Velocity[0] = _neg(_div(_mul(_frac(1, 2), _sub(r[2][1], r[1][2])), s->dT));
		self->Velocity[1] = _neg(_div(_mul(_frac(1, 2), _sub(r[0][2], r[2][0])), s->dT));
		self->Velocity[2] = _neg(_div(_mul(_frac(1, 2), _sub(r[1][0], r[0][1])), s->dT));
	}
	return;

EXIT:
	return;
}
#endif // IST_ROTATION_MATRIX

ISTVOID ComputeVelocity(Rotation *self)
{
	ComputeVelocityFromMatrix(self);
}
#endif // IST_MAGNET_VELOCITY

Rotation *New(ISTVOID)
{
	struct rotation *s;

	s = (struct rotation *)ist_calloc(1, sizeof(struct rotation));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->dT = _frac(1, IST_ROTATION_HZ);
#ifdef IST_ROTATION_FILTERED
	s->NewFilters = NewFilters;
	s->DeleteFilters = DeleteFilters;
	s->DoFilter = DoFilter;
	s->DoVelocityFilter = DoVelocityFilter;
	s->NewFilters((Rotation *)s);
#endif // IST_ROTATION_FILTERED
#ifdef IST_ROTATION_VELOCITY
	s->ComputeVelocity = ComputeVelocity;
#endif // IST_ROTATION_VELOCITY
	s->ComputeAngular = ComputeAngular;
	s->magnet = IST_MagnetEngine()->New();
	if (!s->magnet) {
		goto EXIT_DELETE_ROTATION;
	}
	s->accel = IST_AccelEngine()->New();
	if (!s->accel) {
		goto EXIT_DELETE_ROTATION;
	}
	s->is_valid = ISTFALSE;
	s->is_inited = ISTFALSE;
	return (Rotation *)s;

EXIT_DELETE_ROTATION:
	Delete((Rotation *)s);
	s = NULL;

EXIT:
	return (Rotation *)NULL;
}

ISTVOID Delete(Rotation *self)
{
	struct rotation *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct rotation *)self;
	if (s->accel) {
		IST_AccelEngine()->Delete(s->accel);
		s->accel = NULL;
	}
	if (s->magnet) {
		IST_MagnetEngine()->Delete(s->magnet);
		s->magnet = NULL;
	}
#ifdef IST_ROTATION_FILTERED
	s->DeleteFilters(self);
#endif // IST_ROTATION_FILTERED
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTBOOL Process(Rotation *self, ISTFLOAT mdata[3], ISTFLOAT gdata[3], ISTFLOAT dT)
{
	struct rotation *s;
	ISTSHORT i;
	ISTBOOL ret;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct rotation *)self;
	if (_gt(dT, 0)) {
		s->dT = dT;
	}
	ret = s->accel->Process(s->accel, gdata, dT);
	if (!ret) {
		goto EXIT;
	}
	ret = s->magnet->Process(s->magnet, mdata, dT);
	if (!ret) {
		goto EXIT;
	}
	for (i = 0; i < 3; ++i) {
		s->mdata[i] = s->magnet->Data[i];
		s->gdata[i] = s->accel->Data[i];
	}
	s->ComputeAngular(self);
	if (!s->is_valid) {
		goto EXIT;
	}
#ifdef IST_ROTATION_FILTERED
	s->DoFilter(self, dT);
#endif // IST_ROTATION_FILTERED
#ifdef IST_ROTATION_VELOCITY
	s->ComputeVelocity(self);
#  ifdef IST_ROTATION_VELOCITY_FILTERED
	s->DoVelocityFilter(self, s->dT);
#  endif // IST_ROTATION_VELOCITY_FILTERED
#endif // IST_ROTATION_VELOCITY
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

Rotation *IST_Rotation(ISTVOID)
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

#endif // IST_ROTATION
