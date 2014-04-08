#ifndef ISENTEK_ACCELERATION_CALIBRATION_H
#define ISENTEK_ACCELERATION_CALIBRATION_H

#include "typedefs.h"

EXTERN_C_BEGIN

typedef struct _AcceleratorOutput {
	float at_positive_gravity;
	float at_negative_gravity;
	float at_zero_gravity;
} AcceleratorMeasurement;

typedef struct _AccelerationCalibration {
	/* class methods */
	BOOL (*IsMeta)();
	struct _AccelerationCalibration *(*New)(float g, AcceleratorMeasurement x, AcceleratorMeasurement y, AcceleratorMeasurement z);
	void (*Delete)(struct _AccelerationCalibration *self);
	/* object methods */
	BOOL (*Process)(struct _AccelerationCalibration *self, float x, float y, float z, float dT);
	void (*GetData)(struct _AccelerationCalibration *self, float *x, float *y, float *z);
} AccelerationCalibration;

AccelerationCalibration * CAccelerationCalibration();

EXTERN_C_END

#endif // ISENTEK_ACCELERATION_CALIBRATION_H