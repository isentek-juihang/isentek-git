#include "ist_milieu.h"

struct hardiron {
	Hardiron pub;
	Sphere *engine;
	ISTFLOAT input[3];
};

static Hardiron *New();
static ISTVOID Delete(Hardiron *self);
static ISTBOOL Process(Hardiron *self, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z);
static ISTBOOL GetBias(Hardiron *self, ISTFLOAT bias[3]);
static ISTFLOAT GetRadius(Hardiron *self);
static ISTBOOL GetData(Hardiron *self, ISTFLOAT data[3]);
static ISTVOID Enable(Hardiron *self);
static ISTVOID Disable(Hardiron *self);

static Hardiron ThisClass = {0};
static Hardiron *This = (Hardiron *)NULL;

Hardiron *New()
{
	struct hardiron *s;
	Sphere *engine;

	s = (struct hardiron *)ist_calloc(1, sizeof(struct hardiron));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	engine = IST_Sphere()->New();
	if (!engine) {
		goto EXIT;
	}
	engine->SetRadiusMin(engine, _float(IST_HIRON_RAD_MIN));
	s->engine = engine;
	return (Hardiron *)s;

EXIT:
	return (Hardiron *)NULL;
}

ISTVOID Delete(Hardiron *self)
{
	struct hardiron *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct hardiron *)self;
	IST_Sphere()->Delete(s->engine);
	s->engine = NULL;
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTBOOL Process(Hardiron *self, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z)
{
	struct hardiron *s;
	Sphere *engine;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct hardiron *)self;
	engine = s->engine;
	if (!engine) {
		goto EXIT;
	}
	s->input[0] = x;
	s->input[1] = y;
	s->input[2] = z;
	return engine->Process(engine, x, y, z);

EXIT:
	return ISTFALSE;
}

ISTBOOL GetBias(Hardiron *self, ISTFLOAT bias[3])
{
	struct hardiron *s;
	Sphere *engine;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct hardiron *)self;
	engine = s->engine;
	if (!engine) {
		goto EXIT;
	}
	return engine->GetBias(engine, bias);

EXIT:
	return ISTFALSE;
}

ISTFLOAT GetRadius(Hardiron *self)
{
	struct hardiron *s;
	Sphere *engine;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct hardiron *)self;
	engine = s->engine;
	if (!engine) {
		goto EXIT;
	}
	return engine->GetRadius(engine);

EXIT:
	return 0;
}

ISTBOOL GetData(Hardiron *self, ISTFLOAT data[3])
{
	struct hardiron *s;
	Sphere *engine;
	ISTFLOAT bias[3];
	ISTSHORT i;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct hardiron *)self;
	engine = s->engine;
	if (!engine) {
		goto EXIT;
	}
	if (!engine->GetBias(engine, bias)) {
		goto EXIT;
	}
	for (i = 0; i < 3; ++i) {
		data[i] = _sub(s->input[i], bias[i]);
	}
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

ISTVOID Enable(Hardiron *self)
{
	struct hardiron *s;
	Sphere *engine;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct hardiron *)self;
	engine = s->engine;
	if (!engine) {
		goto EXIT;
	}
	engine->Enable(engine);
	return;

EXIT:
	return;
}

ISTVOID Disable(Hardiron *self)
{
	struct hardiron *s;
	Sphere *engine;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct hardiron *)self;
	engine = s->engine;
	if (!engine) {
		goto EXIT;
	}
	engine->Enable(engine);
	return;

EXIT:
	return;
}

Hardiron * IST_Hardiron()
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		ThisClass.GetBias = GetBias;
		ThisClass.GetRadius = GetRadius;
		ThisClass.GetData = GetData;
		ThisClass.Enable = Enable;
		ThisClass.Disable = Disable;
		This = &ThisClass;
	}
	return (Hardiron *)This;
}