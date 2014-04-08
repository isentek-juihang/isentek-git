#ifndef IST_WMA_H_INCLUDED
#define IST_WMA_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _WMAFilter;
typedef struct _WMAFilter WMAFilter;

struct _WMAFilter {
	WMAFilter *(*New)(ISTVOID);
	ISTVOID (*Delete)(WMAFilter *);
	ISTFLOAT (*Process)(WMAFilter *, ISTFLOAT data);
	ISTVOID (*SetPeriod)(WMAFilter *, ISTINT period);
	IST_READ ISTBOOL IsObject;
};

WMAFilter *IST_WMAFilter(ISTVOID);

IST_EXTERN_C_END

#endif // IST_WMA_H_INCLUDED
