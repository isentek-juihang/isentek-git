#ifndef IST_HARDIRON_H_INCLUDED
#define IST_HARDIRON_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"
#include "ist_qft.h"

IST_EXTERN_C_BEGIN

typedef struct _Hardiron {
	struct _Hardiron *(*New)();
	ISTVOID (*Delete)(struct _Hardiron *);
	ISTBOOL (*Process)(struct _Hardiron *, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z);
	ISTBOOL (*GetBias)(struct _Hardiron *, ISTFLOAT bias[3]);
	ISTFLOAT (*GetRadius)(struct _Hardiron *);
	ISTBOOL (*GetData)(struct _Hardiron *, ISTFLOAT data[3]);
	ISTVOID (*Enable)(struct _Hardiron *);
	ISTVOID (*Disable)(struct _Hardiron *);
	IST_READ ISTBOOL IsObject;
} Hardiron;

Hardiron * IST_Hardiron();

IST_EXTERN_C_END

#endif // IST_HARDIRON_H_INCLUDED