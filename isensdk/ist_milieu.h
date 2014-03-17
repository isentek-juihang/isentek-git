#ifndef IST_MILIEU_H_INCLUDED
#define IST_MILIEU_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"
#include "ist_errno.h"
#include "ist_debug.h"
#include "ist_math.h"
#include "ist_qft.h"
#include "ist_ellipsoid.h"
#include "ist_sphere.h"
#include "ist_hardiron.h"
#include "ist_softiron.h"

#if defined(IST_OS_WIN32) \
	|| defined(IST_OS_ANDROID) \
	|| defined(IST_OS_LINUX) \
	|| defined(IST_OS_LIBC)
#  include <stdlib.h>
#  include <string.h>
#  ifdef IST_DEBUG_MEM
     size_t __ist_mem_watch(size_t c, int op);
     void * __ist_calloc(size_t c, size_t s);
	 void * __ist_realloc(void *p, size_t s);
	 void __ist_free(void *p);
	 void __ist_memset(void *p, int v, size_t s);
	 void * __ist_memcpy(void *d, const void *s, size_t l);
#  else
#    define __ist_mem_watch(c,op) 0
#    define __ist_calloc(c,s) calloc((c),(s))
#    define __ist_realloc(p,s) realloc((p),(s))
#    define __ist_free(p) free((p))
#    define __ist_memset(p,v,s) memset((p),(v),(s))
#    define __ist_memcpy(d,s,l) memcpy((d),(s),(l))
#  endif
#  define ist_calloc __ist_calloc
#  define ist_realloc __ist_realloc
#  define ist_free __ist_free
#  define ist_memset __ist_memset
#  define ist_memcpy __ist_memcpy
#else
#  error "Unknown development environment."
#endif

#endif // IST_MILIEU_H_INCLUDED
