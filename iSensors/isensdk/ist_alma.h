#ifndef IST_ALMA_H_INCLUDED
#define IST_ALMA_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _ALMAFilter;
typedef struct _ALMAFilter ALMAFilter;

struct _ALMAFilter {
	ALMAFilter *(*New)(ISTVOID);
	ISTVOID (*Delete)(ALMAFilter *);
	ISTFLOAT (*Process)(ALMAFilter *, ISTFLOAT data);
	ISTVOID (*SetPeriod)(ALMAFilter *, ISTINT period);
	ISTVOID (*SetSigma)(ALMAFilter *, ISTINT sigma);
	ISTVOID (*SetOffset)(ALMAFilter *, ISTFLOAT offset);
	IST_READ ISTBOOL IsObject;
};

ALMAFilter *IST_ALMAFilter(ISTVOID);

IST_EXTERN_C_END

#endif // IST_ALMA_H_INCLUDED
