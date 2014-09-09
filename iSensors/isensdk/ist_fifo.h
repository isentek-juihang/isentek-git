#ifndef IST_FIFO_H_INCLUDED
#define IST_FIFO_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

struct _FIFOBuffer;
typedef struct _FIFOBuffer FIFOBuffer;

enum _FIFOType;
typedef enum _FIFOType FIFOType;

union _FIFOData;
typedef union _FIFOData FIFOData;

struct _FIFOParcel;
typedef struct _FIFOParcel FIFOParcel;

enum _FIFOType {
	E_FIFO_EMPTY = 0,
	E_FIFO_PVOID,		// void pointer
	E_FIFO_FLOAT,		// float value

	E_FIFO_MAX,
};

union _FIFOData {
	ISTVOID *void_ptr;
	ISTFLOAT float_val;
};

struct _FIFOParcel {
	FIFOType type;
	FIFOData data;
};

struct _FIFOBuffer {
	FIFOBuffer *(*New)(ISTVOID);
	ISTVOID (*Delete)(FIFOBuffer *);
	ISTBOOL (*Put)(FIFOBuffer *, CONST FIFOParcel par);
	FIFOParcel (*Get)(FIFOBuffer *);
	ISTINT (*GetSize)(FIFOBuffer *);
	IST_READ ISTBOOL IsObject;
};

FIFOBuffer *IST_FIFOBuffer(ISTVOID);

IST_EXTERN_C_END

#endif // IST_FIFO_H_INCLUDED
