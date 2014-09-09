#include "ist_milieu.h"

struct msgdb {
    ISTINT code;
    CONST char *msg;
};

STATIC ErrorCode ThisClass = {0};
STATIC ErrorCode *This = (ErrorCode *)NULL;

STATIC ISTVOID SetCode(ISTINT code);
STATIC ISTINT GetCode(ISTVOID);
STATIC CONST ISTCHAR *ToString(ISTVOID);

ISTVOID SetCode(ISTINT code)
{
    This->code = code;
}

ISTINT GetCode(ISTVOID)
{
    return This->code;
}

CONST ISTCHAR *ToString(ISTVOID)
{
    STATIC struct msgdb db[] = {
        { IST_S_OK, "[IST:SOK] Done." },
        { IST_E_FAILED, "[IST:ERR] Failed." },
        { IST_E_OUTOFMEMORY, "[IST:ERR] Out of memory." },
        { IST_E_INVALID, "[IST:ERR] Invalid operation." },
    };
    STATIC ISTSHORT max = (ISTSHORT)(sizeof(db)/sizeof(db[0]));
    ISTSHORT i = 0;

    for (i = 0; i < max; ++i) {
        if (db[i].code == This->code) {
            return db[i].msg;
		}
    }
    return NULL;
}

ErrorCode *IST_ErrorCode(ISTVOID)
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
