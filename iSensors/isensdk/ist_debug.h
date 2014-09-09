#ifndef IST_DEBUG_H_INCLUDED
#define IST_DEBUG_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

IST_EXTERN_C_BEGIN

ISTVOID ist_printf(CONST ISTCHAR *fmt, ...);

#ifdef IST_DEBUG
#  define IST_DBG ist_printf
#  if defined(IST_FLOAT_NATIVE)
#    define IST_DBG_F(_var_) ist_printf("%10.14f", (_var_))
#  elif defined(IST_FLOAT_IEEE754)
#    define IST_DBG_F(_var_) ist_printf("0x%08x", (_var_))
#  endif
#else
#  define IST_DBG 1?((ISTVOID)(NULL)):ist_printf
#  define IST_DBG_F(_var_) ((ISTVOID)(_var_))
#endif

#define xIST_DBG 1?((ISTVOID)(NULL)):ist_printf
#define xIST_DBG_F(_var_) IST_UNUSED(_var_)

IST_EXTERN_C_END

#endif // IST_DEBUG_H_INCLUDED
