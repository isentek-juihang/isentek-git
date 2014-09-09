#ifndef IST_SMA_H_INCLUDED
#define IST_SMA_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _SMAFilter;
typedef struct _SMAFilter SMAFilter;

struct _SMAFilter {
	SMAFilter *(*New)(ISTVOID);
	ISTVOID (*Delete)(SMAFilter *);
	ISTFLOAT (*Process)(SMAFilter *, ISTFLOAT data);
	ISTVOID (*SetPeriod)(SMAFilter *, ISTINT period);
	IST_READ ISTBOOL IsObject;
};

SMAFilter *IST_SMAFilter(ISTVOID);

IST_EXTERN_C_END

#endif // IST_SMA_H_INCLUDED
