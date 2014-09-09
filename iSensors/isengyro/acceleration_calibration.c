#include "acceleration_calibration.h"
#include <stdlib.h>
#include <string.h>

#define ToAcceleratorModel(_model_, _output_) do { \
	(_model_).m = 2.0f/((_output_).at_positive_gravity - (_output_).at_negative_gravity); \
	(_model_).b = -(((_model_).m)*((_output_).at_zero_gravity)); \
} while (0)

#define ToAcceleratorGravity(_model_, _input_) \
	((((_model_).m)*(_input_)) + ((_model_).b))

// y = mx + b
// y is output in g, x is output reading of accelerator, b is bias
struct accelerator_model {
	float m;
	float b;
};

struct acceleration_calibration {
	AccelerationCalibration pub;
	struct accelerator_model mx, my, mz;
	float g;
	float x, y, z;
	float dT;
};

static float ToGravity(struct accelerator_model model, float in);

static BOOL IsMeta();
static BOOL IsNotMeta();
static AccelerationCalibration *New(float g, AcceleratorMeasurement x, AcceleratorMeasurement y, AcceleratorMeasurement z);
static void Delete(AccelerationCalibration *self);
static BOOL Process(AccelerationCalibration *self, float x, float y, float z, float dT);
static void GetData(AccelerationCalibration *self, float *x, float *y, float *z);

static AccelerationCalibration ThisClass = {0};
static AccelerationCalibration *This = NULL;

float ToGravity(struct accelerator_model model, float in)
{
	return (model.m*in + model.b);
}

BOOL IsMeta()
{
	return TRUE;
}

BOOL IsNotMeta()
{
	return FALSE;
}

AccelerationCalibration *New(float g, AcceleratorMeasurement x, AcceleratorMeasurement y, AcceleratorMeasurement z)
{
	struct acceleration_calibration *s;

	if (!This)
		goto EXIT;

	if (g <= 0.0f)
		goto EXIT;

	s = (struct acceleration_calibration *)calloc(1, sizeof(struct acceleration_calibration));
	if (!s)
		goto EXIT;

	memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsMeta = IsNotMeta;

	ToAcceleratorModel(s->mx, x);
	ToAcceleratorModel(s->my, y);
	ToAcceleratorModel(s->mz, z);
	s->g = g;

	return (AccelerationCalibration *)s;

EXIT_DELETE_ACCELERATOR_CALIBRATION:
	Delete((AccelerationCalibration *)s);
	s = NULL;

EXIT:
	return NULL;
}

void Delete(AccelerationCalibration *self)
{
	struct acceleration_calibration *s;

	if (!self || self->IsMeta == IsMeta)
		goto EXIT;

	s = (struct acceleration_calibration *)self;

	free(s);
	s = NULL;

	return;

EXIT:
	return;
}

BOOL Process(AccelerationCalibration *self, float x, float y, float z, float dT)
{
	struct acceleration_calibration *s;

	if (!self || self->IsMeta == IsMeta)
		goto EXIT;

	s = (struct acceleration_calibration *)self;
	
	s->dT = dT;

	s->x = ToAcceleratorGravity(s->mx, x);
	s->y = ToAcceleratorGravity(s->my, y);
	s->z = ToAcceleratorGravity(s->mz, z);

	return TRUE;

EXIT:
	return FALSE;
}

void GetData(AccelerationCalibration *self, float *x, float *y, float *z)
{
	struct acceleration_calibration *s;

	if (!self || self->IsMeta == IsMeta)
		goto EXIT;

	s = (struct acceleration_calibration *)self;

	if (x)
		*x = s->g*s->x;

	if (y)
		*y = s->g*s->y;

	if (z)
		*z = s->g*s->z;

	return;

EXIT:
	return;
}

AccelerationCalibration * CAccelerationCalibration()
{
	if (!This) {
		ThisClass.IsMeta = IsMeta;
		ThisClass.IsMeta = IsMeta;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		ThisClass.GetData = GetData;
		This = &ThisClass;
	}
	return This;
}