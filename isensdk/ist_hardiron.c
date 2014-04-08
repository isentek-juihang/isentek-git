#include "ist_milieu.h"

#ifdef IST_HIRON

struct hardiron {
	Hardiron pub;
	Sphere *engine;
	ISTFLOAT input[3];
};

STATIC Hardiron *New(ISTVOID);
STATIC ISTVOID Delete(Hardiron *self);
STATIC ISTBOOL Process(Hardiron *self, ISTFLOAT data[3]);
STATIC ISTVOID Enable(Hardiron *self);
STATIC ISTVOID Disable(Hardiron *self);

STATIC Hardiron ThisClass = {0};
STATIC Hardiron *This = (Hardiron *)NULL;

Hardiron *New(ISTVOID)
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

ISTBOOL Process(Hardiron *self, ISTFLOAT data[3])
{
	struct hardiron *s;
	ISTSHORT i;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct hardiron *)self;
	for (i = 0; i < 3; ++i) {
		s->input[i] = data[i];
	}
	if (!s->engine->Process(s->engine, data)) {
		goto EXIT;
	}
	for (i = 0; i < 3; ++i) {
		self->Bias[i] = s->engine->Bias[i];
		self->Data[i] = _sub(s->input[i], self->Bias[i]);
	}
	self->Radius = s->engine->Radius;
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

Hardiron * IST_Hardiron(ISTVOID)
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
	return (Hardiron *)This;
}

#endif // IST_HIRON
