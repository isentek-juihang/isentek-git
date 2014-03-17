#include "ist_milieu.h"

typedef struct _error_code {
    ErrorCode pub;
} error_code;

typedef struct _msgdb {
    ISTINT code;
    const char *msg;
} msgdb;

static ErrorCode ThisClass = {0};
static ErrorCode *This = (ErrorCode *)NULL;

static ISTVOID SetCode(ISTINT code);
static ISTINT GetCode();
static const char *ToString();

ISTVOID SetCode(ISTINT code)
{
    This->code = code;
}

ISTINT GetCode()
{
    return This->code;
}

const char *ToString()
{
    static msgdb db[] = {
        { IST_S_OK, "[IST:SOK] Done." },
        { IST_E_FAILED, "[IST:ERR] Failed." },
        { IST_E_OUTOFMEMORY, "[IST:ERR] Out of memory." },
        { IST_E_INVALID, "[IST:ERR] Invalid operation." },
    };
    static ISTSHORT max = (ISTSHORT)(sizeof(db)/sizeof(db[0]));
    ISTSHORT i = 0;

    for (i = 0; i < max; ++i) {
        if (db[i].code == This->code)
            return db[i].msg;
    }
    return NULL;
}

ErrorCode *IST_ErrorCode()
{
    if (!This) {
		ThisClass.IsObject = ISTFALSE;
        ThisClass.SetCode = SetCode;
        ThisClass.GetCode = GetCode;
        ThisClass.ToString = ToString;
        This = &ThisClass;
    }
    return (ErrorCode *)This;
}
