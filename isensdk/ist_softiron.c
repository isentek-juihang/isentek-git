struct softiron;

#include "ist_milieu.h"

typedef ISTVOID (*ist_ReceiveSamples)(struct softiron *s, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z);
typedef ISTVOID (*ist_UpdateField)(struct softiron *s);

#ifdef IST_SIRON_REALTIME
struct realtime {
	Ellipsoid *engine;
	ISTFLOAT x[IST_SIRON_QUAD][IST_SIRON_QUAD][IST_SIRON_QUAD];
	ISTFLOAT y[IST_SIRON_QUAD][IST_SIRON_QUAD][IST_SIRON_QUAD];
	ISTFLOAT z[IST_SIRON_QUAD][IST_SIRON_QUAD][IST_SIRON_QUAD];
	ISTINT index[IST_SIRON_QUAD][IST_SIRON_QUAD][IST_SIRON_QUAD];
	ISTFLOAT data[3][IST_SIRON_DBUF_MAX];
	ISTINT min;
	ISTINT max;
	ISTFLOAT rad_min;
	ISTFLOAT rad_max;
	ISTINT hz;
	ISTINT update;
	ISTINT loop;
	ISTINT count;
	ISTFLOAT covar;
	ISTFLOAT rad;
	ISTBOOL is_valid;
};
#endif

struct softiron {
	Softiron pub;
	Hardiron *hardiron;
#ifdef IST_SIRON_REALTIME
	ist_ReceiveSamples ReceiveSamples;
	ist_UpdateField UpdateField;
	struct realtime rt;
#endif
	ISTFLOAT input[3];
	ISTFLOAT bias[3];
	ISTFLOAT mat[3][3];
	ISTBOOL is_valid;
};

static Softiron *New();
static ISTVOID Delete(Softiron *self);
#ifdef IST_SIRON_REALTIME
  static ISTVOID SetActive(Softiron *self, ISTINT min, ISTINT max);
  static ISTVOID SetFreq(Softiron *self, ISTINT hz);
  static ISTVOID SetUpdateFreq(Softiron *self, ISTINT loop);
  static ISTVOID ReceiveSamples(struct softiron *s, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z);
  static ISTVOID UpdateField(struct softiron *s);
#else
  static ISTVOID SetOfflineMatrix(Softiron *self, const ISTFLOAT mat[3][3]);
#endif
static ISTBOOL Process(Softiron *self, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z);
static ISTBOOL GetBias(Softiron *self, ISTFLOAT bias[3]);
static ISTBOOL GetMatrix(Softiron *self, ISTFLOAT mat[3][3]);
static ISTBOOL GetData(Softiron *self, ISTFLOAT data[3]);
static ISTFLOAT GetRadius(Softiron *self);
static ISTVOID Enable(Softiron *self);
static ISTVOID Disable(Softiron *self);

static Softiron ThisClass = {0};
static Softiron *This = NULL;

Softiron *New()
{
	struct softiron *s;
	Hardiron *hardiron;
#ifdef IST_SIRON_REALTIME
	struct realtime *rt;
	ISTSHORT i, j, k;
#endif

	IST_DBG("Softiron::New()\n");
	s = (struct softiron *)ist_calloc(1, sizeof(struct softiron));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	hardiron = IST_Hardiron()->New();
	if (!hardiron) {
		goto EXIT;
	}
	s->hardiron = hardiron;
	IST_DBG("HIRON:mem:%u\n", __ist_mem_watch(0, 0));
#ifdef IST_SIRON_REALTIME
	s->ReceiveSamples = ReceiveSamples;
	s->UpdateField = UpdateField;
	rt = &s->rt;
	rt->engine = IST_Ellipsoid()->New(IST_SIRON_DBUF_MAX);
	if (!rt->engine) {
		goto EXIT_FREE_HARDIRON;
	}
	IST_DBG("SIRON:mem:%u\n", __ist_mem_watch(0, 0));
	rt->min = IST_SIRON_DATA_MIN;
	rt->max = IST_SIRON_DATA_MAX;
	rt->rad_min = _float(IST_SIRON_RAD_MIN);
	rt->rad_max = _float(IST_SIRON_RAD_MAX);
	rt->hz = IST_SIRON_HZ;
	rt->update = IST_SIRON_UPDATE_HZ;
	rt->rad = _float(IST_SIRON_RAD_MIN);
	rt->is_valid = ISTFALSE;
	for (i = 0; i < IST_SIRON_QUAD; ++i) {
		for (j = 0; j < IST_SIRON_QUAD; ++j) {
			for (k = 0; k < IST_SIRON_QUAD; ++k) {
				rt->index[i][j][k] = -1;
			}
		}
	}
#endif
	s->mat[0][0] = s->mat[1][1] = s->mat[2][2] = _one;	
	s->is_valid = ISTFALSE;
	return (Softiron *)s;

#ifdef IST_SIRON_REALTIME
EXIT_FREE_HARDIRON:
	IST_Hardiron()->Delete(hardiron);
	s->hardiron = hardiron = NULL;
#endif

EXIT:
	return (Softiron *)NULL;
}

ISTVOID Delete(Softiron *self)
{
	struct softiron *s;
#ifdef IST_SIRON_REALTIME
	struct realtime *rt;
#endif

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
#ifdef IST_SIRON_REALTIME
	rt = &s->rt;
	IST_Ellipsoid()->Delete(rt->engine);
	rt->engine = NULL;
#endif
	IST_Hardiron()->Delete(s->hardiron);
	s->hardiron = NULL;
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

#ifdef IST_SIRON_REALTIME
ISTVOID SetActive(Softiron *self, ISTINT min, ISTINT max)
{
	struct softiron *s;
	struct realtime *rt;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	rt = &s->rt;
	if (max > IST_SIRON_DBUF_MAX) {
		max = IST_SIRON_DBUF_MAX;
	}
	rt->max = max;
	if (min > max) {
		min = max;
	}
	rt->min = min;

EXIT:
	return;
}

ISTVOID SetFreq(Softiron *self, ISTINT hz)
{
	struct softiron *s;
	struct realtime *rt;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	rt = &s->rt;
	if (hz > rt->loop) {
		hz = rt->loop;
	}
	rt->hz = hz;

EXIT:
	return;
}

ISTVOID SetUpdateFreq(Softiron *self, ISTINT update)
{
	struct softiron *s;
	struct realtime *rt;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	rt = &s->rt;
	if (update >= 0) {
		if (update < rt->hz) {
			update = rt->hz;
		}
	}
	rt->update = update;

EXIT:
	return;
}

ISTVOID ReceiveSamples(struct softiron *s, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z)
{
	struct realtime *rt;
	ISTFLOAT ix, iy, iz;
	ISTSHORT i, j, k;
	ISTINT previndex;
	ISTINT oldestindex;
	ISTINT oldesti, oldestj, oldestk;
	const ISTFLOAT deg90 = _float(90);
	const ISTFLOAT rad2deg = _frac(5958360, 103993);
	const ISTFLOAT deg2idx = _frac(IST_SIRON_QUAD, 180);

	if (!s->is_valid) {
		goto EXIT;
	}
	ix = _sub(x, s->bias[0]);
	iy = _sub(y, s->bias[1]);
	iz = _sub(z, s->bias[2]);
	i = _int(_mul(_add(_mul(_atan2(ix, _abs(iy)), rad2deg), deg90), deg2idx));
	j = _int(_mul(_add(_mul(_atan2(iy, _abs(iz)), rad2deg), deg90), deg2idx));
	k = _int(_mul(_add(_mul(_atan2(iz, _abs(ix)), rad2deg), deg90), deg2idx));
	if (i >= IST_SIRON_QUAD) {
		i = IST_SIRON_QUAD - 1;
	}
	if (j >= IST_SIRON_QUAD) {
		j = IST_SIRON_QUAD - 1;
	}
	if (k >= IST_SIRON_QUAD) {
		k = IST_SIRON_QUAD - 1;
	}
	rt = &s->rt;
	previndex = rt->index[i][j][k];
	rt->index[i][j][k] = rt->loop++;
	rt->x[i][j][k] = x;
	rt->y[i][j][k] = y;
	rt->z[i][j][k] = z;
	if (previndex == -1) {
		if (rt->count < rt->max) {
			++rt->count;
		} else {
			oldestindex = rt->loop;
			oldesti = oldestj = oldestk = 0;
			for (i = 0; i < IST_SIRON_QUAD; ++i) {
				for (j = 0; j < IST_SIRON_QUAD; ++j) {
					for (k = 0; k < IST_SIRON_QUAD; ++k) {
						if ((rt->index[i][j][k] != -1) && (rt->index[i][j][k] < oldestindex)) {
							oldesti = i;
							oldestj = j;
							oldestk = k;
							oldestindex = rt->index[oldesti][oldestj][oldestk];
						}
					}
				}
			}
			rt->index[oldesti][oldestj][oldestk] = -1;
		}
	}

EXIT:
	return;
}

ISTVOID UpdateField(struct softiron *s)
{
	struct realtime *rt;
	Hardiron *hardiron;
	Ellipsoid *engine;
	ISTFLOAT covar;
	ISTFLOAT rad;
	//ISTFLOAT bias_mag;
	ISTFLOAT mat[3][3];
	ISTFLOAT bias[3];
	ISTINT ndata = 0;
	ISTSHORT i, j, k;

	hardiron = s->hardiron;
	if (!hardiron || !s->is_valid) {
		goto EXIT;
	}
	rt = &s->rt;
	engine = rt->engine;
	if (!engine) {
		goto EXIT;
	}
	if (rt->count >= rt->min) {
		if ((!rt->is_valid) ||
			(rt->is_valid && !(rt->loop % rt->hz) && (rt->loop <= rt->max)) ||
			(rt->is_valid && (rt->update >= 0) && !(rt->loop % rt->update))) {
			for (i = 0; i < IST_SIRON_QUAD; ++i) {
				for (j = 0; j < IST_SIRON_QUAD; ++j) {
					for (k = 0; k < IST_SIRON_QUAD; ++k) {
						if ((rt->index[i][j][k] != -1) && (ndata < IST_SIRON_DBUF_MAX)) {
							rt->data[0][ndata] = rt->x[i][j][k];
							rt->data[1][ndata] = rt->y[i][j][k];
							rt->data[2][ndata] = rt->z[i][j][k];
							++ndata;
						}
					}
				}
			}
			if (ndata > 0) {
				engine->SetRadius(engine, hardiron->GetRadius(hardiron));
				if (engine->Process(engine, rt->data[0], rt->data[1], rt->data[2], ndata)) {
					engine->GetMatrix(engine, mat);
					engine->GetBias(engine, &bias[0], &bias[1], &bias[2]);
					covar = engine->GetCovar(engine);
					rad = engine->GetRadius(engine);
#if 1
					IST_DBG("W[3][3]:\n");
					for (i = 0; i < 3; ++i) {
						for (j = 0; j < 3; ++j) {
							IST_DBG_F(mat[i][j]);
							IST_DBG(", ");
						}
						IST_DBG("\n");
					}
					IST_DBG("V[3]:");
					for (i = 0; i < 3; ++ i) {
						IST_DBG_F(bias[i]);
						IST_DBG(", ");
					}
					IST_DBG("\n");
#endif
					if (rt->covar == 0) {
						rt->covar = covar;
					}
					//bias_mag = _sqrt(_add3(_mul(bias[0], bias[0]), _mul(bias[1], bias[1]), _mul(bias[2], bias[2])));
					if (_le(covar, rt->covar) && _ge(rad, rt->rad_min) && _le(rad, rt->rad_max)) {
						IST_DBG("*** ACCEPT SOFT ***\n");
						rt->covar = covar;
						rt->rad = rad;
						for (i = 0; i < 3; ++i) {
							s->bias[i] = bias[i];
							for (j = 0; j < 3; ++j) {
								s->mat[i][j] = mat[i][j];
							}
						}
						if (!rt->is_valid) {
							rt->is_valid = ISTTRUE;
						}
					}
					IST_DBG("P = ");
					IST_DBG_F(rt->covar);
					IST_DBG(" -> ");
					rt->covar = _add(rt->covar, _mul4(rt->covar, _frac(1, 300), _float(rt->update), _frac(1, rt->hz)));
					IST_DBG_F(rt->covar);
					IST_DBG("\n");
				}
			}
		}
	}

EXIT:
	return;
}

#else // IST_SOFTIRON_REALTIME

ISTVOID SetOfflineMatrix(Softiron *self, const ISTFLOAT mat[3][3])
{
	struct softiron *s;
	ISTSHORT i, j;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			s->mat[i][j] = mat[i][j];
		}
	}

EXIT:
	return;
}
#endif // IST_SOFTIRON_REALTIME

ISTBOOL Process(Softiron *self, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z)
{
	struct softiron *s;
	Hardiron *hardiron;
#ifdef IST_SIRON_REALTIME
	struct realtime *rt;
#endif

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	hardiron = s->hardiron;
	if (!hardiron) {
		goto EXIT;
	}
	s->input[0] = x;
	s->input[1] = y;
	s->input[2] = z;
#ifdef IST_SIRON_REALTIME
	rt = &s->rt;
	if (!rt->is_valid || ((rt->update < 0) && (rt->loop > rt->max))) {
		s->is_valid |= hardiron->Process(hardiron, x, y, z);
		if (s->is_valid) {
			hardiron->GetBias(hardiron, s->bias);
			IST_DBG("*** ACCEPT HARD ***\n");
			IST_DBG("V[3]:\n");
			IST_DBG_F(s->bias[0]);
			IST_DBG(",");
			IST_DBG_F(s->bias[1]);
			IST_DBG(",");
			IST_DBG_F(s->bias[2]);
			IST_DBG("\n");
		}
	}
	if (s->is_valid) {
		s->ReceiveSamples(s, x, y, z);
		s->UpdateField(s);
	}
#else
	s->is_valid |= hardiron->Process(hardiron, x, y, z);
	if (s->is_valid) {
		hardiron->GetBias(hardiron, s->bias);
		IST_DBG("*** ACCEPT HARD ***\n");
		IST_DBG("V[3]:\n");
		IST_DBG_F(s->bias[0]);
		IST_DBG(",");
		IST_DBG_F(s->bias[1]);
		IST_DBG(",");
		IST_DBG_F(s->bias[2]);
		IST_DBG("\n");
	}
#endif
	return s->is_valid;

EXIT:
	return ISTFALSE;
}

ISTBOOL GetBias(Softiron *self, ISTFLOAT bias[3])
{
	struct softiron *s;
	ISTSHORT i;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (!s->is_valid) {
		goto EXIT;
	}
	IST_DBG("bias[3]= ");
	for (i = 0; i < 3; ++i) {
		bias[i] = s->bias[i];
		IST_DBG_F(bias[i]);
		IST_DBG("%s", i < 2 ? ", " : " ");
	}
	IST_DBG("\n");
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

ISTBOOL GetMatrix(Softiron *self, ISTFLOAT mat[3][3])
{
	struct softiron *s;
	ISTSHORT i, j;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (!s->is_valid) {
		goto EXIT;
	}
#ifdef IST_SIRON_REALTIME
	if (!s->rt.is_valid) {
		goto EXIT;
	}
#endif
	IST_DBG("mat[3][3]=\n");
	for (i = 0; i < 3; ++i) {
		IST_DBG("\t");
		for (j = 0; j < 3; ++j) {
			IST_DBG_F(s->mat[i][j]);
			IST_DBG("%s", j < 2 ? ", " : " ");
			mat[i][j] = s->mat[i][j];
		}
		IST_DBG("\n");
	}
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

ISTBOOL GetData(Softiron *self, ISTFLOAT data[3])
{
	struct softiron *s;
	ISTSHORT i;
#ifdef IST_SIRON_REALTIME
	ISTSHORT j;
#endif

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (!s->is_valid) {
		goto EXIT;
	}
	IST_DBG("data[3]= ");
#ifdef IST_SIRON_REALTIME
	if (!s->rt.is_valid) {
		for (i = 0; i < 3; ++i) {
			data[i] = _sub(s->input[i], s->bias[i]);
			IST_DBG_F(data[i]);
			IST_DBG("%s", i < 2 ? ", " : " ");
		}
	} else {
		for (i = 0; i < 3; ++i) {
			data[i] = 0;
			for (j = 0; j < 3; ++j) {
				data[i] = _add(data[i], _mul(s->mat[i][j], _sub(s->input[j], s->bias[j])));
			}
			IST_DBG_F(data[i]);
			IST_DBG("%s", i < 2 ? ", " : " ");
		}
	}
#else
	for (i = 0; i < 3; ++i) {
		data[i] = _sub(s->input[i], s->bias[i]);
		IST_DBG_F(data[i]);
		IST_DBG("%s", i < 2 ? ", " : " ");
	}
#endif
	IST_DBG("\n");
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

ISTFLOAT GetRadius(Softiron *self)
{
	struct softiron *s;
	Hardiron *hardiron;
#ifdef IST_SIRON_REALTIME
	struct realtime *rt;
#endif

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (!s->is_valid) {
		goto EXIT;
	}
	hardiron = s->hardiron;
	if (!hardiron) {
		goto EXIT;
	}
#ifdef IST_SIRON_REALTIME
	rt = &s->rt;
	if (rt->is_valid && (rt->update >= 0)) {
		IST_DBG("radius=");IST_DBG_F(rt->rad);IST_DBG("\n");
		return rt->rad;
	}
#endif
	IST_DBG("radius=");IST_DBG_F(hardiron->GetRadius(hardiron));IST_DBG("\n");
	return hardiron->GetRadius(hardiron);

EXIT:
	return 0;
}

ISTVOID Enable(Softiron *self)
{
	struct softiron *s;
#ifdef IST_SIRON_REALTIME
	struct realtime *rt;
#endif

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (s->hardiron) {
		s->hardiron->Enable(s->hardiron);
	}
#ifdef IST_SIRON_REALTIME
	rt = &s->rt;
	if (rt->engine) {
		rt->engine->Enable(rt->engine);
	}
#endif

EXIT:
	return;
}

ISTVOID Disable(Softiron *self)
{
	struct softiron *s;
#ifdef IST_SIRON_REALTIME
	struct realtime *rt;
	ISTSHORT i, j, k;
#endif

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (s->hardiron) {
		s->hardiron->Disable(s->hardiron);
	}
#ifdef IST_SIRON_REALTIME
	rt = &s->rt;
	if (rt->engine) {
		rt->engine->Disable(rt->engine);
	}
	rt->count = 0;
	rt->covar = 0;
	rt->loop = 0;
	rt->rad = _float(IST_SIRON_RAD_MIN);
	rt->is_valid = ISTFALSE;
	for (i = 0; i < IST_SIRON_QUAD; ++i) {
		for (j = 0; j < IST_SIRON_QUAD; ++j) {
			for (k = 0; k < IST_SIRON_QUAD; ++k) {
				rt->index[i][j][k] = -1;
			}
		}
	}
#endif

EXIT:
	return;
}

Softiron * IST_Softiron()
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
#ifdef IST_SIRON_REALTIME
		ThisClass.SetActive = SetActive;
		ThisClass.SetFreq = SetFreq;
		ThisClass.SetUpdateFreq = SetUpdateFreq;
#else
		ThisClass.SetOfflineMatrix = SetOfflineMatrix;
#endif
		ThisClass.Process = Process;
		ThisClass.GetBias = GetBias;
		ThisClass.GetMatrix = GetMatrix;
		ThisClass.GetData = GetData;
		ThisClass.GetRadius = GetRadius;
		ThisClass.Enable = Enable;
		ThisClass.Disable = Disable;
		This = &ThisClass;
	}
	return This;
}