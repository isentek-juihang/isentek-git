#include "ist_milieu.h"

#ifdef IST_SIRON

struct softiron;

typedef ISTVOID (*ist_ReceiveSamples)(Softiron *, ISTFLOAT data[3]);
typedef ISTVOID (*ist_UpdateField)(Softiron *);

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
	ISTBOOL is_valid;
};

STATIC ISTVOID ReceiveSamples(Softiron *self, ISTFLOAT data[3]);
STATIC ISTVOID UpdateField(Softiron *self);

struct softiron {
	Softiron pub;
	Hardiron *hardiron;
	ISTFLOAT input[3];
	ISTBOOL is_valid;
#ifdef IST_SIRON_REALTIME
	ist_ReceiveSamples ReceiveSamples;
	ist_UpdateField UpdateField;
	struct realtime rt;
#endif // IST_SIRON_REALTIME
};

STATIC Softiron *New(ISTVOID);
STATIC ISTVOID Delete(Softiron *self);
STATIC ISTBOOL Process(Softiron *self, ISTFLOAT data[3]);
STATIC ISTVOID Enable(Softiron *self);
STATIC ISTVOID Disable(Softiron *self);

STATIC Softiron ThisClass = {0};
STATIC Softiron *This = NULL;

Softiron *New(ISTVOID)
{
	struct softiron *s;
	Hardiron *hardiron;
	ISTSHORT i, j, k;

	IST_DBG("Softiron::New()\n");
	s = (struct softiron *)ist_calloc(1, sizeof(struct softiron));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->pub.Matrix[0][0] = s->pub.Matrix[1][1] = s->pub.Matrix[2][2] = _one;
	hardiron = IST_Hardiron()->New();
	if (!hardiron) {
		goto EXIT;
	}
	s->hardiron = hardiron;
	s->is_valid = ISTFALSE;
	IST_DBG("HIRON:mem:%u\n", __ist_mem_watch(0, 0));
#ifdef IST_SIRON_REALTIME
	s->ReceiveSamples = ReceiveSamples;
	s->UpdateField = UpdateField;
	s->rt.engine = IST_Ellipsoid()->New(IST_SIRON_DBUF_MAX);
	if (!s->rt.engine) {
		IST_Hardiron()->Delete(hardiron);
		s->hardiron = hardiron = NULL;
		goto EXIT;
	}
	IST_DBG("SIRON:mem:%u\n", __ist_mem_watch(0, 0));
	s->rt.min = IST_SIRON_DATA_MIN;
	s->rt.max = IST_SIRON_DATA_MAX;
	s->rt.rad_min = _float(IST_SIRON_RAD_MIN);
	s->rt.rad_max = _float(IST_SIRON_RAD_MAX);
	s->rt.hz = IST_SIRON_HZ;
	s->rt.update = IST_SIRON_UPDATE_HZ;
	s->rt.is_valid = ISTFALSE;
	for (i = 0; i < IST_SIRON_QUAD; ++i) {
		for (j = 0; j < IST_SIRON_QUAD; ++j) {
			for (k = 0; k < IST_SIRON_QUAD; ++k) {
				s->rt.index[i][j][k] = -1;
			}
		}
	}
#else // IST_SIRON_REALTIME
	IST_UNUSED(i);
	IST_UNUSED(j);
	IST_UNUSED(k);
#endif // IST_SIRON_REALTIME
	return (Softiron *)s;

EXIT:
	return (Softiron *)NULL;
}

ISTVOID Delete(Softiron *self)
{
	struct softiron *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
#ifdef IST_SIRON_REALTIME
	IST_Ellipsoid()->Delete(s->rt.engine);
	s->rt.engine = NULL;
#endif // IST_SIRON_REALTIME
	IST_Hardiron()->Delete(s->hardiron);
	s->hardiron = NULL;
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

#ifdef IST_SIRON_REALTIME
ISTVOID ReceiveSamples(Softiron *self, ISTFLOAT data[3])
{
	struct softiron *s;
	ISTFLOAT ix, iy, iz;
	ISTSHORT i, j, k;
	ISTINT previndex;
	ISTINT oldestindex;
	ISTINT oldesti, oldestj, oldestk;
	CONST ISTFLOAT deg90 = _float(90);
	CONST ISTFLOAT rad2deg = _frac(5958360, 103993);
	CONST ISTFLOAT deg2idx = _frac(IST_SIRON_QUAD, 180);

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (!s->is_valid) {
		goto EXIT;
	}
	ix = _sub(data[0], self->Bias[0]);
	iy = _sub(data[1], self->Bias[1]);
	iz = _sub(data[2], self->Bias[2]);
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
	previndex = s->rt.index[i][j][k];
	s->rt.index[i][j][k] = s->rt.loop++;
	s->rt.x[i][j][k] = data[0];
	s->rt.y[i][j][k] = data[1];
	s->rt.z[i][j][k] = data[2];
	if (previndex == -1) {
		if (s->rt.count < s->rt.max) {
			++s->rt.count;
		} else {
			oldestindex = s->rt.loop;
			oldesti = oldestj = oldestk = 0;
			for (i = 0; i < IST_SIRON_QUAD; ++i) {
				for (j = 0; j < IST_SIRON_QUAD; ++j) {
					for (k = 0; k < IST_SIRON_QUAD; ++k) {
						if ((s->rt.index[i][j][k] != -1) && (s->rt.index[i][j][k] < oldestindex)) {
							oldesti = i;
							oldestj = j;
							oldestk = k;
							oldestindex = s->rt.index[oldesti][oldestj][oldestk];
						}
					}
				}
			}
			s->rt.index[oldesti][oldestj][oldestk] = -1;
		}
	}

EXIT:
	return;
}

ISTVOID UpdateField(Softiron *self)
{
	struct softiron *s;
	Hardiron *hardiron;
	Ellipsoid *engine;
	ISTINT num = 0;
	ISTSHORT i, j, k;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}

	s = (struct softiron *)self;
	hardiron = s->hardiron;
	if (!hardiron || !s->is_valid) {
		goto EXIT;
	}
	engine = s->rt.engine;
	if (!engine) {
		goto EXIT;
	}
	if (s->rt.count >= s->rt.min) {
		if ((!s->rt.is_valid) ||
			(s->rt.is_valid && !(s->rt.loop % s->rt.hz) && (s->rt.loop <= s->rt.max)) ||
			(s->rt.is_valid && (s->rt.update >= 0) && !(s->rt.loop % s->rt.update))) {
			for (i = 0; i < IST_SIRON_QUAD; ++i) {
				for (j = 0; j < IST_SIRON_QUAD; ++j) {
					for (k = 0; k < IST_SIRON_QUAD; ++k) {
						if ((s->rt.index[i][j][k] != -1) && (num < IST_SIRON_DBUF_MAX)) {
							s->rt.data[0][num] = s->rt.x[i][j][k];
							s->rt.data[1][num] = s->rt.y[i][j][k];
							s->rt.data[2][num] = s->rt.z[i][j][k];
							++num;
						}
					}
				}
			}
			if (num > 0) {
				engine->SetRadius(engine, hardiron->Radius);
				if (engine->Process(engine, s->rt.data[0], s->rt.data[1], s->rt.data[2], num)) {
#ifdef IST_DEBUG
					IST_DBG("W[3][3]:\n");
					for (i = 0; i < 3; ++i) {
						for (j = 0; j < 3; ++j) {
							IST_DBG_F(engine->Matrix[i][j]);
							IST_DBG(", ");
						}
						IST_DBG("\n");
					}
					IST_DBG("V[3]:");
					for (i = 0; i < 3; ++ i) {
						IST_DBG_F(engine->Bias[i]);
						IST_DBG(", ");
					}
					IST_DBG("\n");
#endif
					if (s->rt.covar == 0) {
						s->rt.covar = engine->Covariant;
					}
					if (_le(engine->Covariant, s->rt.covar) && \
						_ge(engine->Radius, s->rt.rad_min) && _le(engine->Radius, s->rt.rad_max)) {
						IST_DBG("*** ACCEPT SOFT ***\n");
						s->rt.covar = engine->Covariant;
						self->Radius = engine->Radius;
						for (i = 0; i < 3; ++i) {
							self->Bias[i] = engine->Bias[i];
							for (j = 0; j < 3; ++j) {
								self->Matrix[i][j] = engine->Matrix[i][j];
							}
						}
						if (!s->rt.is_valid) {
							s->rt.is_valid = ISTTRUE;
						}
					}
					IST_DBG("P = ");
					IST_DBG_F(s->rt.covar);
					IST_DBG(" -> ");
					s->rt.covar = _add(s->rt.covar, _mul4(s->rt.covar, _frac(1, 300), _float(s->rt.update), _frac(1, s->rt.hz)));
					IST_DBG_F(s->rt.covar);
					IST_DBG("\n");
				}
			}
		}
	}

EXIT:
	return;
}

#endif // IST_SIRON_REALTIME

ISTBOOL Process(Softiron *self, ISTFLOAT data[3])
{
	struct softiron *s;
	Hardiron *hardiron;
	ISTSHORT i, j;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	hardiron = s->hardiron;
	for (i = 0; i < 3; ++i) {
		s->input[i] = data[i];
	}
#ifdef IST_SIRON_REALTIME
	if (!s->rt.is_valid || ((s->rt.update < 0) && (s->rt.loop > s->rt.max))) {
		s->is_valid |= hardiron->Process(hardiron, data);
		if (s->is_valid) {
			for (i = 0; i < 3; ++i) {
				self->Bias[i] = hardiron->Bias[i];
			}
			self->Radius = hardiron->Radius;
			IST_DBG("*** ACCEPT HARD ***\n");
			IST_DBG("V[3]:\n");
			IST_DBG_F(self->Bias[0]);
			IST_DBG(",");
			IST_DBG_F(self->Bias[1]);
			IST_DBG(",");
			IST_DBG_F(self->Bias[2]);
			IST_DBG("\n");
		}
	}
	if (s->is_valid) {
		s->ReceiveSamples(self, data);
		s->UpdateField(self);
	}
#else // IST_SIRON_REALTIME
	s->is_valid |= hardiron->Process(hardiron, data);
	if (s->is_valid) {
		hardiron->GetBias(hardiron, self->Bias);
		self->Radius = hardiron->GetRadius(hardiron);
		IST_DBG("*** ACCEPT HARD ***\n");
		IST_DBG("V[3]:\n");
		IST_DBG_F(self->Bias[0]);
		IST_DBG(",");
		IST_DBG_F(self->Bias[1]);
		IST_DBG(",");
		IST_DBG_F(self->Bias[2]);
		IST_DBG("\n");
	}
#endif // IST_SIRON_REALTIME
	if (s->is_valid) {
		IST_DBG("Data[]:\n");
		for (i = 0; i < 3; ++i) {
			self->Data[i] = 0;
			for (j = 0; j < 3; ++j) {
				self->Data[i] = _add(self->Data[i], _mul(self->Matrix[i][j], _sub(s->input[j], self->Bias[j])));
			}
			IST_DBG_F(self->Data[i]);
			IST_DBG("%s", i < 2 ? ", " : " ");
		}
		IST_DBG("\n");
	}
	return s->is_valid;

EXIT:
	return ISTFALSE;
}

ISTVOID Enable(Softiron *self)
{
	struct softiron *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (s->hardiron) {
		s->hardiron->Enable(s->hardiron);
	}
#ifdef IST_SIRON_REALTIME
	if (s->rt.engine) {
		s->rt.engine->Enable(s->rt.engine);
	}
#endif // IST_SIRON_REALTIME

EXIT:
	return;
}

ISTVOID Disable(Softiron *self)
{
	struct softiron *s;
	ISTSHORT i, j, k;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct softiron *)self;
	if (s->hardiron) {
		s->hardiron->Disable(s->hardiron);
	}
#ifdef IST_SIRON_REALTIME
	if (s->rt.engine) {
		s->rt.engine->Disable(s->rt.engine);
	}
	s->rt.count = 0;
	s->rt.covar = 0;
	s->rt.loop = 0;
	s->rt.is_valid = ISTFALSE;
	for (i = 0; i < IST_SIRON_QUAD; ++i) {
		for (j = 0; j < IST_SIRON_QUAD; ++j) {
			for (k = 0; k < IST_SIRON_QUAD; ++k) {
				s->rt.index[i][j][k] = -1;
			}
		}
	}
#else
	IST_UNUSED(i);
	IST_UNUSED(j);
	IST_UNUSED(k);
#endif // IST_SIRON_REALTIME

EXIT:
	return;
}

Softiron * IST_Softiron(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		ThisClass.Enable = Enable;
		ThisClass.Disable = Disable;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_SIRON
