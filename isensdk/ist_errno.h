#ifndef IST_ERRNO_H_INCLUDED
#define IST_ERRNO_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

//** ------------------------------------------------------------------------
//** Error code
//** ------------------------------------------------------------------------
#define IST_S_OK 0                      // SUCCESS
#define IST_E_FAILED (-1)               // Normal failure
#define IST_E_OUTOFMEMORY (-2)          // Out of memory
#define IST_E_INVALID (-3)              // Invalid operation

//** ------------------------------------------------------------------------
//** Error Class
//** ------------------------------------------------------------------------
typedef struct _ErrorCode {
	// Methods
    ISTVOID (*SetCode)(ISTINT code);
    ISTINT (*GetCode)();
    const char *(*ToString)();
	// Properties
	IST_READ ISTBOOL IsObject;
	IST_READ ISTINT code;
} ErrorCode;

ErrorCode *IST_ErrorCode();

IST_EXTERN_C_END

#endif // IST_ERRNO_H_INCLUDED
