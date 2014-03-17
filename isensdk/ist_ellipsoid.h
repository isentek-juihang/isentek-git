#ifndef IST_ELLIPSOID_H_INCLUDED
#define IST_ELLIPSOID_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"
#include "ist_math.h"

#ifdef IST_ELLIPSOID

IST_EXTERN_C_BEGIN

#define IST_ELLIPSOID_RADIUS 100

typedef struct _Ellipsoid {
	struct _Ellipsoid *(*New)(ISTUINT max);
	ISTVOID (*Delete)(struct _Ellipsoid *);
	ISTBOOL (*Process)(struct _Ellipsoid *, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z, ISTUINT num);
	ISTVOID (*SetRadius)(struct _Ellipsoid *, ISTFLOAT radius);
	ISTFLOAT (*GetRadius)(struct _Ellipsoid *);
	ISTVOID (*GetMatrix)(struct _Ellipsoid *, ISTFLOAT mat[3][3]);
	ISTVOID (*GetBias)(struct _Ellipsoid *, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z);
	ISTFLOAT (*GetCovar)(struct _Ellipsoid *);
	ISTVOID (*Enable)(struct _Ellipsoid *);
	ISTVOID (*Disable)(struct _Ellipsoid *);
	IST_READ ISTBOOL IsObject;
} Ellipsoid;

Ellipsoid *IST_Ellipsoid();

IST_EXTERN_C_END

#endif // IST_ELLIPSOID
#endif // IST_ELLIPSOID_H_INCLUDED