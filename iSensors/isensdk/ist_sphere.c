#include "ist_milieu.h"

#ifdef IST_SPHERE

#define IST_SPHERE_DATAMAX IST_HIRON_DBUF_MAX
#define IST_SPHERE_OFFSET_NUM IST_HIRON_ODBUF_MAX
#define IST_SPHERE_RADIUS_MIN _float(IST_MAG_MIN)	// 10 uT
#define IST_SPHERE_VARIATION_RATIO _frac(3, 20)		// 0.15

typedef ISTBOOL (*ist_FitModel)(Sphere *, ISTFLOAT data[3]);

struct sphere_model {
	ISTFLOAT data[IST_SPHERE_DATAMAX][3];
	ISTSHORT num;
	ISTFLOAT offsets[IST_SPHERE_OFFSET_NUM][3];
	ISTFLOAT p4s[4][3];
	ISTFLOAT mean[3];
	ISTFLOAT var[3];	// variation
	ISTFLOAT rad;
	ISTFLOAT rad_min;	// radius threshold
	ISTFLOAT ratio;		// variation threshold, ratio of offset
};

struct sphere {
	Sphere pub;
	ist_FitModel FitModel;
	struct sphere_model model; 
	ISTBOOL is_valid;
	ISTBOOL is_enable;
};

STATIC Sphere *New(ISTVOID);
STATIC ISTVOID Delete(Sphere *self);
STATIC ISTBOOL Process(Sphere *self, ISTFLOAT data[3]);
STATIC ISTVOID SetRadiusMin(Sphere *self, ISTFLOAT bound);
STATIC ISTVOID Enable(Sphere *self);
STATIC ISTVOID Disable(Sphere *self);
STATIC ISTVOID RecieveData(struct sphere_model *model, ISTFLOAT data[3]);
STATIC ISTBOOL IsInitedVector(ISTFLOAT v[3]);
STATIC ISTBOOL GetAvailableVectorNum(struct sphere_model *model);
STATIC ISTFLOAT GetDistance(CONST ISTFLOAT x[3], CONST ISTFLOAT y[3]);
STATIC ISTVOID Get4Points(struct sphere_model *model);
STATIC ISTFLOAT GetDet(ISTFLOAT a[4][4], ISTSHORT n);
STATIC ISTBOOL GetParams(struct sphere_model *model, ISTFLOAT radius);
STATIC ISTBOOL GetSphereFrom4Points(struct sphere_model *model);
STATIC ISTBOOL FitModel(Sphere *self, ISTFLOAT data[3]);
STATIC ISTVOID Reset(Sphere *s);

STATIC Sphere ThisClass = {0};
STATIC Sphere *This = (Sphere *)NULL;

ISTVOID RecieveData(struct sphere_model *model, ISTFLOAT data[3])
{
	ISTSHORT i, j;

	for (j = 0; j < 3; ++j) {
		for (i = 1; i < IST_SPHERE_DATAMAX; ++i) {
			model->data[IST_SPHERE_DATAMAX - i][j] = model->data[IST_SPHERE_DATAMAX - i - 1][j];
		}
	}
	for (j = 0; j < 3; ++j) {
		model->data[0][j] = data[j];
	}
}

ISTBOOL IsInitedVector(ISTFLOAT v[3])
{
	ISTSHORT i;

	for (i = 0; i < 3; i++) {
		if (_le(_max, v[i])) {
			return ISTTRUE;
		}
	}
	return ISTFALSE;
}

ISTBOOL GetAvailableVectorNum(struct sphere_model *model)
{
	ISTSHORT i, ndata = 0;

	for (i = IST_SPHERE_DATAMAX; 3 < i; --i) {
		if (!IsInitedVector(model->data[i - 1])) {
			ndata = i;
			break;
		}
	}
	model->num = ndata;
	if (ndata < 4) {
		return ISTFALSE;
	}
	return ISTTRUE;
}

ISTFLOAT GetDistance(CONST ISTFLOAT x[3], CONST ISTFLOAT y[3])
{
	ISTSHORT i;
	ISTFLOAT tmp, r = 0;

	for (i = 0; i < 3; ++i) {
		tmp = _sub(x[i], y[i]);
		r = _add(r, _pow2(tmp));
	}
	if (_le(r, 0)) {
		return 0;
	}
	return _sqrt(r);
}

ISTVOID Get4Points(struct sphere_model *model)
{
	ISTFLOAT dv[IST_SPHERE_DATAMAX][3];
	ISTFLOAT cross[3] = {0};
	ISTFLOAT tmpv[3] = {0};
	ISTSHORT ndata = model->num;
	ISTSHORT i, j;
	ISTFLOAT dist, tmp;

	// Point 0
	for (j = 0; j < 3; ++j) {
		model->p4s[0][j] = model->data[0][j];
	}
	// Point 1
	dist = 0;
	for (i = 1; i < ndata; i++) {
		tmp = GetDistance(model->data[i], model->p4s[0]);
		if (_lt(dist, tmp)) {
			dist = tmp;
			for (j = 0; j < 3; j++) {
				model->p4s[1][j] = model->data[i][j];
			}
		}
	}
	// Point 2
	dist = 0;
	for (j = 0; j < 3; j++) {
		dv[0][j] = _sub(model->p4s[1][j], model->p4s[0][j]);
	}
	for (i = 1; i < ndata; i++) {
		for (j = 0; j < 3; j++) {
			dv[i][j] = _sub(model->data[i][j], model->p4s[0][j]);
		}
		tmpv[0] = _sub(_mul(dv[0][1], dv[i][2]), _mul(dv[0][2], dv[i][1]));
		tmpv[1] = _sub(_mul(dv[0][2], dv[i][0]), _mul(dv[0][0], dv[i][2]));
		tmpv[2] = _sub(_mul(dv[0][0], dv[i][1]), _mul(dv[0][1], dv[i][0]));
		tmp = _add3(_pow2(tmpv[0]), _pow2(tmpv[1]), _pow2(tmpv[2]));
		if (_lt(dist, tmp)) {
			dist = tmp;
			for (j = 0; j < 3; j++) {
				model->p4s[2][j] = model->data[i][j];
				cross[j] = tmpv[j];
			}
		}
	}
	// Point 3
	dist = 0;
	for (i = 1; i < ndata; i++) {
		tmp = _add3(_mul(dv[i][0], cross[0]), _mul(dv[i][1], cross[1]), _mul(dv[i][2], cross[2]));
		tmp = _abs(tmp);
		if (_lt(dist, tmp)) {
			dist = tmp;
			for (j = 0; j < 3; j++) {
				model->p4s[3][j] = model->data[i][j];
			}
		}
	}
	return;
}

ISTFLOAT GetDet(ISTFLOAT a[4][4], ISTSHORT n)
{
	ISTSHORT i, j, j1, j2;
	ISTFLOAT det = 0;
	ISTFLOAT mat[4][4] = {{0}};

	if (n == 2) { /* terminate recursion */
		det = _sub(_mul(a[0][0], a[1][1]), _mul(a[1][0], a[0][1]));
	} else {
		det = 0;
		for (j1 = 0; j1 < n; j1++) { /* do each column */
			for (i = 1; i < n; i++) { /* create minor */
				j2 = 0;
				for (j = 0; j < n; j++) {
					if (j == j1) {
						continue;
					}
					mat[i-1][j2] = a[i][j];
					j2++;
				}
			}
			/* sum (+/-)cofactor * minor */
			if ((j1 % 2) == 0) {
				det = _add(det, _mul(a[0][j1], GetDet(mat, n-1)));
			} else {
				det = _sub(det, _mul(a[0][j1], GetDet(mat, n-1)));
			}
		}
	}
	return det;
}

ISTBOOL GetParams(struct sphere_model *model, ISTFLOAT rad)
{
	ISTSHORT i, j;
	ISTFLOAT min[3] = {0};
	ISTFLOAT max[3] = {0};
	ISTFLOAT mean[3] = {0};
	ISTFLOAT var[3] = {0};
	ISTFLOAT tmpf;
	ISTFLOAT two = _float(2);

	for (j = 0; j < 3; j++) {
		min[j] = model->offsets[0][j];
		max[j] = model->offsets[0][j];
		for (i = 1; i < IST_SPHERE_OFFSET_NUM; i++) {
			if (_lt(model->offsets[i][j], min[j])) {
				min[j] = model->offsets[i][j];
			}
			if (_gt(model->offsets[i][j], max[j])) {
				max[j] = model->offsets[i][j];
			}
		}
		mean[j] = _div(_add(max[j], min[j]), two);
		var[j] = _sub(max[j], min[j]);
	}
	tmpf = _mul(rad, model->ratio);
	if (_ge(var[0], tmpf) || _ge(var[1], tmpf) || _ge(var[2], tmpf)) {
		goto EXIT;
	}
	for (j = 0; j < 3; ++j) {
		model->mean[j] = mean[j];
		model->var[j] = var[j];
	}
	model->rad = rad;
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

ISTBOOL GetSphereFrom4Points(struct sphere_model *model)
{
	ISTFLOAT a[4][4] = {{0}};
	ISTFLOAT ho[3] = {0};
	ISTFLOAT norm = 0;
	ISTFLOAT rad = 0;
	ISTFLOAT tmpf = 0;
	ISTFLOAT two = _float(2);
	ISTFLOAT m11, m12, m13, m14, m15;
	ISTINT i, j;

	// Get sphere from 4 points
	for (i = 0; i < 4; i++) { /* find minor 11 */
		a[i][0] = model->p4s[i][0];
		a[i][1] = model->p4s[i][1];
		a[i][2] = model->p4s[i][2];
		a[i][3] = _one;
	}
	m11 = GetDet(a, 4);
	for (i = 0; i < 4; i++) { /* find minor 12 */
		a[i][0] = _add(_add(_mul(model->p4s[i][0], model->p4s[i][0]),
			_mul(model->p4s[i][1], model->p4s[i][1])),
			_mul(model->p4s[i][2], model->p4s[i][2]));
		a[i][1] = model->p4s[i][1];
		a[i][2] = model->p4s[i][2];
		a[i][3] = _one;
	}
	m12 = GetDet(a, 4);
	for (i = 0; i < 4; i++) { /* find minor 13 */
		a[i][0] = _add(_add(_mul(model->p4s[i][0], model->p4s[i][0]),
			_mul(model->p4s[i][1], model->p4s[i][1])),
			_mul(model->p4s[i][2], model->p4s[i][2]));
		a[i][1] = model->p4s[i][0];
		a[i][2] = model->p4s[i][2];
		a[i][3] = _one;
	}
	m13 = GetDet(a, 4);
	for (i = 0; i < 4; i++) { /* find minor 14 */
		a[i][0] = _add(_add(_mul(model->p4s[i][0], model->p4s[i][0]),
			_mul(model->p4s[i][1], model->p4s[i][1])),
			_mul(model->p4s[i][2], model->p4s[i][2]));
		a[i][1] = model->p4s[i][0];
		a[i][2] = model->p4s[i][1];
		a[i][3] = _one;
	}
	m14 = GetDet(a, 4);
	for (i = 0; i < 4; i++) { /* find minor 15 */
		a[i][0] = _add(_add(_mul(model->p4s[i][0], model->p4s[i][0]),
			_mul(model->p4s[i][1], model->p4s[i][1])),
			_mul(model->p4s[i][2], model->p4s[i][2]));
		a[i][1] = model->p4s[i][0];
		a[i][2] = model->p4s[i][1];
		a[i][3] = model->p4s[i][2];
	}
	m15 = GetDet(a, 4);
	if (_eq(m11, 0)) {
		rad = 0;
	} else { /* center of sphere */
		ho[0] = _div(_div(m12, m11), two);
		ho[1] = _neg(_div(_div(m13, m11), two));
		ho[2] = _div(_div(m14, m11), two);
		norm = _sub(_add(_add(_mul(ho[0], ho[0]),
			_mul(ho[1], ho[1])),
			_mul(ho[2], ho[2])),
			_div(m15, m11));
		if (_ge(norm, 0)) {
			rad = _sqrt(norm);
		}
	}
	if (_le(rad, 0)) {
		goto EXIT;
	}
	// Check distance
	for (i = 0; i < 4; i++) {
		for (j = (i + 1); j < 4; j++) {
			tmpf = GetDistance(model->p4s[i], model->p4s[j]);
			if (_lt(tmpf, rad) || _lt(tmpf, model->rad_min)) {
				goto EXIT;
			}
		}
	}
	// Update offset 
	for (i = 1; i < IST_SPHERE_OFFSET_NUM; i++) {
		for (j = 0; j < 3; ++j) {
			model->offsets[IST_SPHERE_OFFSET_NUM - i][j] = model->offsets[IST_SPHERE_OFFSET_NUM - i - 1][j];
		}
	}
	for (j = 0; j < 3; ++j) {
		model->offsets[0][j] = ho[j];
	}
	for (i = (IST_SPHERE_DATAMAX >> 1); i < IST_SPHERE_DATAMAX; i++) {
		for (j = 0; j < 3; ++j) {
			model->data[i][j] = _max;
		}
	}
	// Check offset buffer full
	if (IsInitedVector(model->offsets[IST_SPHERE_OFFSET_NUM - 1])) {
		goto EXIT;
	}
	// Calculate mean bias and radius
	if (!GetParams(model, rad)) {
		goto EXIT;
	}
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

ISTBOOL FitModel(Sphere *self, ISTFLOAT data[3])
{
	struct sphere *s;

	s = (struct sphere *)self;
	RecieveData(&s->model, data);
	if (!GetAvailableVectorNum(&s->model)) {
		goto EXIT;
	}
	Get4Points(&s->model);
	if (!GetSphereFrom4Points(&s->model)) {
		goto EXIT;
	}

	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

ISTVOID Reset(Sphere *self)
{
	struct sphere *s;
	struct sphere_model *model;
	ISTSHORT i, j;

	s = (struct sphere *)self;
	model = &s->model;
	for (i = 0; i < IST_SPHERE_DATAMAX; ++i) {
		for (j = 0; j < 3; ++j) {
			model->data[i][j] = _max;
		}
	}
	for (i = 0; i < IST_SPHERE_OFFSET_NUM; ++i) {
		for (j = 0; j < 3; ++j) {
			model->offsets[i][j] = _max;
		}
	}
	model->rad = 0;
	model->rad_min = IST_SPHERE_RADIUS_MIN;
	model->ratio = IST_SPHERE_VARIATION_RATIO;
	s->is_valid = ISTFALSE;
	s->is_enable = ISTFALSE;
}

Sphere *New(ISTVOID)
{
	struct sphere *s;

	s = (struct sphere *)ist_calloc(1, sizeof(struct sphere));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->FitModel = FitModel;
	Reset((Sphere *)s);
	return (Sphere *)s;

EXIT:
	return (Sphere *)NULL;
}

ISTVOID Delete(Sphere *self)
{
	struct sphere *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct sphere *)self;
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTBOOL Process(Sphere *self, ISTFLOAT data[3])
{
	struct sphere *s;
	ISTBOOL res;
	ISTSHORT i;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct sphere *)self;
	if (s->is_enable) {
		res = s->FitModel(self, data);
		if (res) {
			for (i = 0; i < 3; ++i) {
				self->Bias[i] = s->model.mean[i];
			}
			self->Radius = s->model.rad;
			if (!s->is_valid) {
				s->is_valid = ISTTRUE;
			}
		}
	}
	return s->is_valid;

EXIT:
	return ISTFALSE;
}

ISTVOID SetRadiusMin(Sphere *self, ISTFLOAT bound)
{
	struct sphere *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct sphere *)self;
	s->model.rad_min = bound;

EXIT:
	return;
}

ISTVOID Enable(Sphere *self)
{
	struct sphere *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct sphere *)self;
	s->is_enable = ISTTRUE;
	return;

EXIT:
	return;
}

ISTVOID Disable(Sphere *self)
{
	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	Reset(self);
	return;

EXIT:
	return;
}

Sphere * IST_Sphere(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		ThisClass.SetRadiusMin = SetRadiusMin;
		ThisClass.Enable = Enable;
		ThisClass.Disable = Disable;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_SPHERE
