#ifndef IST_SPHERE_H_INCLUDED
#define IST_SPHERE_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _Sphere;
typedef struct _Sphere Sphere;

struct _Sphere {
	Sphere *(*New)(void);
	ISTVOID (*Delete)(Sphere *);
	ISTBOOL (*Process)(Sphere *, ISTFLOAT data[3]);
	ISTVOID (*SetRadiusMin)(Sphere *, ISTFLOAT min);
	ISTVOID (*Enable)(Sphere *);
	ISTVOID (*Disable)(Sphere *);
	IST_READ ISTBOOL IsObject;
	IST_READ ISTFLOAT Bias[3];
	IST_READ ISTFLOAT Radius;
};

Sphere * IST_Sphere(void);

IST_EXTERN_C_END

#endif // IST_SPHERE_H_INCLUDED
