#ifndef IST_RMA_H_INCLUDED
#define IST_RMA_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _RMAFilter;
typedef struct _RMAFilter RMAFilter;

struct _RMAFilter {
	RMAFilter *(*New)(ISTVOID);
	ISTVOID (*Delete)(RMAFilter *);
	ISTFLOAT (*Process)(RMAFilter *, ISTFLOAT data);
	ISTVOID (*SetPeriod)(RMAFilter *, ISTINT period);
	ISTVOID (*SetWeight)(RMAFilter *, ISTFLOAT weight);
	IST_READ ISTBOOL IsObject;
};

RMAFilter *IST_RMAFilter(ISTVOID);

IST_EXTERN_C_END

#endif // IST_RMA_H_INCLUDED
