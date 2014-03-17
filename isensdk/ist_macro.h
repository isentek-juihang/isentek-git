#ifndef IST_MACRO_H_INCLUDED
#define IST_MACRO_H_INCLUDED

#include "ist_prep.h"

//** ------------------------------------------------------------------------
//** Extern C macro
//** ------------------------------------------------------------------------
#ifdef __cplusplus
#  define IST_EXTERN_C_BEGIN extern "C" {
#  define IST_EXTERN_C_END   }
#else
#  define IST_EXTERN_C_BEGIN
#  define IST_EXTERN_C_END
#endif

//** ------------------------------------------------------------------------
//** Compile time assertion
//** ------------------------------------------------------------------------
#define IST_COMPILE_ASSERT(name,x) typedef int _IST_dummy_ ## name[(x)?1:-1]

//** ------------------------------------------------------------------------
//** Disable compiler warning
//** ------------------------------------------------------------------------
#define IST_UNUSED(x)   (ISTVOID)(x)

//** ------------------------------------------------------------------------
//** INLINE macro
//** -----------------------------------------------------------------------
#ifndef INLINE
#  if defined(IST_COMPILER_GCC)
#    define INLINE static inline
#  elif defined(IST_COMPILER_MSVC)
#    define INLINE static __inline
#  else
#    define INLINE static inline
#  endif
#endif

#ifndef STATIC
#  define STATIC static
#endif

#endif // IST_MACRO_H_INCLUDED
