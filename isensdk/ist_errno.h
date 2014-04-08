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

struct _ErrorCode;
typedef struct _ErrorCode ErrorCode;

struct _ErrorCode {
    ISTVOID (*SetCode)(ISTINT code);
    ISTINT (*GetCode)(ISTVOID);
    CONST ISTCHAR *(*ToString)(ISTVOID);
	IST_READ ISTBOOL IsObject;
	IST_READ ISTINT code;
};

ErrorCode *IST_ErrorCode(ISTVOID);

IST_EXTERN_C_END

#endif // IST_ERRNO_H_INCLUDED
