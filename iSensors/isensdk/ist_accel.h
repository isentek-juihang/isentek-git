#ifndef IST_ACCEL_H_INCLUDED
#define IST_ACCEL_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _Acceleration;
typedef struct _Acceleration Acceleration;

struct _Acceleration {
	Acceleration *(*New)(ISTVOID);
	ISTVOID (*Delete)(Acceleration *);
	ISTBOOL (*Process)(Acceleration *, ISTFLOAT data[3], ISTFLOAT dT);
	IST_READ ISTBOOL IsObject;
	IST_READ ISTFLOAT Data[3];
};

Acceleration* IST_Acceleration(ISTVOID);

IST_EXTERN_C_END

#endif // IST_ACCEL_H_INCLUDED