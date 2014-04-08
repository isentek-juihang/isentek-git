#ifndef IST_HARDIRON_H_INCLUDED
#define IST_HARDIRON_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _Hardiron;
typedef struct _Hardiron Hardiron;

struct _Hardiron {
	Hardiron *(*New)(ISTVOID);
	ISTVOID (*Delete)(Hardiron *);
	ISTBOOL (*Process)(Hardiron *, ISTFLOAT data[3]);
	ISTVOID (*Enable)(Hardiron *);
	ISTVOID (*Disable)(Hardiron *);
	IST_READ ISTBOOL IsObject;
	IST_READ ISTFLOAT Data[3];
	IST_READ ISTFLOAT Bias[3];
	IST_READ ISTFLOAT Radius;
};

Hardiron *IST_Hardiron(ISTVOID);

IST_EXTERN_C_END

#endif // IST_HARDIRON_H_INCLUDED
