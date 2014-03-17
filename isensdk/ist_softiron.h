#ifndef IST_SOFTIRON_H_INCLUDED
#define IST_SOFTIRON_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"
#include "ist_qft.h"

IST_EXTERN_C_BEGIN

#define IST_SOFTIRON_ONCE_UPDATED (-1)

typedef struct _Softiron {
	struct _Softiron *(*New)();
	ISTVOID (*Delete)(struct _Softiron *);
#ifdef IST_SIRON_REALTIME
	ISTVOID (*SetActive)(struct _Softiron *, ISTINT min, ISTINT max);
	ISTVOID (*SetFreq)(struct _Softiron *, ISTINT hz);
	// IST_SOFTIRON_ONCE_UPDATED means never update again after first calibration
	ISTVOID (*SetUpdateFreq)(struct _Softiron *, ISTINT update);
#else
	ISTVOID (*SetOfflineMatrix)(struct _Softiron *, const ISTFLOAT mat[3][3]);
#endif
	ISTBOOL (*Process)(struct _Softiron *, ISTFLOAT x, ISTFLOAT y, ISTFLOAT z);
	ISTBOOL (*GetBias)(struct _Softiron *, ISTFLOAT bias[3]);
	ISTBOOL (*GetMatrix)(struct _Softiron *, ISTFLOAT mat[3][3]);
	ISTBOOL (*GetData)(struct _Softiron *, ISTFLOAT data[3]);
	ISTFLOAT (*GetRadius)(struct _Softiron *);
	ISTVOID (*Enable)(struct _Softiron *);
	ISTVOID (*Disable)(struct _Softiron *);
	IST_READ ISTBOOL IsObject;
} Softiron;

Softiron * IST_Softiron();

IST_EXTERN_C_END

#endif // IST_SOFTIRON_H_INCLUDED