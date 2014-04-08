#ifndef IST_MAGNET_H_INCLUDED
#define IST_MAGNET_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _Magnet;
typedef struct _Magnet Magnet;

struct _Magnet {
	Magnet *(*New)(ISTVOID);
	ISTVOID (*Delete)(Magnet *);
	ISTBOOL (*Process)(Magnet *, ISTFLOAT data[3], ISTFLOAT dT);
	IST_READ ISTBOOL IsObject;
	IST_READ ISTFLOAT Data[3];
	IST_READ ISTFLOAT Bias[3];
	IST_READ ISTFLOAT Matrix[3][3];
	IST_READ ISTFLOAT Radius;
	IST_READ ISTFLOAT Velocity[3];
};

Magnet *IST_Magnet(ISTVOID);

IST_EXTERN_C_END

#endif // IST_MAGNET_H_INCLUDED
