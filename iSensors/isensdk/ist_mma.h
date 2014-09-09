#ifndef IST_MMA_H_INCLUDED
#define IST_MMA_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _MMAFilter;
typedef struct _MMAFilter MMAFilter;

struct _MMAFilter {
	MMAFilter *(*New)();
	ISTVOID (*Delete)(MMAFilter *);
	ISTFLOAT (*Process)(MMAFilter *, ISTFLOAT data);
	ISTVOID (*SetPeriod)(MMAFilter *, ISTINT period);
	IST_READ ISTBOOL IsObject;
};

MMAFilter *IST_MMAFilter();

IST_EXTERN_C_END

#endif // IST_MMA_H_INCLUDED
