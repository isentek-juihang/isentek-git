#ifndef IST_SOFTIRON_H_INCLUDED
#define IST_SOFTIRON_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _Softiron;
typedef struct _Softiron Softiron;

struct _Softiron {
	Softiron *(*New)(ISTVOID);
	ISTVOID (*Delete)(Softiron *);
	ISTBOOL (*Process)(Softiron *, ISTFLOAT data[3]);
	ISTVOID (*Enable)(Softiron *);
	ISTVOID (*Disable)(Softiron *);
	IST_READ ISTBOOL IsObject;
	IST_READWRITE ISTFLOAT Matrix[3][3];
	IST_READ ISTFLOAT Bias[3];
	IST_READ ISTFLOAT Data[3];
	IST_READ ISTFLOAT Radius;
};

Softiron *IST_Softiron(ISTVOID);

#define IST_SOFTIRON_ONCE_UPDATED (-1)

IST_EXTERN_C_END

#endif // IST_SOFTIRON_H_INCLUDED
