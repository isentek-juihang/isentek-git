#ifndef IST_SPHERE_H_INCLUDED
#define IST_SPHERE_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

typedef struct _Sphere {
	struct _Sphere *(*New)();
	ISTVOID (*Delete)(struct _Sphere *);
	ISTBOOL (*Process)(struct _Sphere *, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z);
	ISTBOOL (*GetBias)(struct _Sphere *, ISTFLOAT bias[3]);
	ISTFLOAT (*GetRadius)(struct _Sphere *);
	ISTVOID (*SetRadiusMin)(struct _Sphere *, ISTFLOAT min);
	ISTVOID (*Enable)(struct _Sphere *);
	ISTVOID (*Disable)(struct _Sphere *);
	IST_READ ISTBOOL IsObject;
} Sphere;

Sphere * IST_Sphere();

IST_EXTERN_C_END

#endif // IST_SPHERE_H_INCLUDED