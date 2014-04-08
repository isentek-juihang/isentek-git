#ifndef IST_ROTATION_H_INCLUDED
#define IST_ROTATION_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _Rotation;
typedef struct _Rotation Rotation;

struct _Rotation {
	Rotation *(*New)(ISTVOID);
	ISTVOID (*Delete)(Rotation *);
	ISTBOOL (*Process)(Rotation *, ISTFLOAT mdata[3], ISTFLOAT gdata[3], ISTFLOAT dT);
	IST_READ ISTBOOL IsObject;
	IST_READ ISTFLOAT Angular[3];
	IST_READ ISTFLOAT Matrix[3][3];
	IST_READ ISTFLOAT Quaternion[4]; // [ x, y, z, w ]
	IST_READ ISTFLOAT Velocity[3];
};

Rotation *IST_Rotation(ISTVOID);

IST_EXTERN_C_END

#endif // IST_MAGNET_H_INCLUDED
