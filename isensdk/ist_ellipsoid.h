#ifndef IST_ELLIPSOID_H_INCLUDED
#define IST_ELLIPSOID_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"
#include "ist_math.h"

IST_EXTERN_C_BEGIN

#define IST_ELLIPSOID_RADIUS 100

struct _Ellipsoid;
typedef struct _Ellipsoid Ellipsoid;

struct _Ellipsoid {
	Ellipsoid *(*New)(ISTUINT max);
	ISTVOID (*Delete)(Ellipsoid *);
	ISTBOOL (*Process)(Ellipsoid *, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z, ISTUINT num);
	ISTVOID (*SetRadius)(Ellipsoid *, ISTFLOAT radius);
	ISTVOID (*Enable)(Ellipsoid *);
	ISTVOID (*Disable)(Ellipsoid *);
	IST_READ ISTBOOL IsObject;
	IST_READ ISTFLOAT Radius;
	IST_READ ISTFLOAT Matrix[3][3];
	IST_READ ISTFLOAT Bias[3];
	IST_READ ISTFLOAT Covariant;
};

Ellipsoid *IST_Ellipsoid(ISTVOID);

IST_EXTERN_C_END

#endif // IST_ELLIPSOID_H_INCLUDED
