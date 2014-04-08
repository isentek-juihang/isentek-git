#ifndef IST_MILIEU_H_INCLUDED
#define IST_MILIEU_H_INCLUDED

#include "ist_prep.h"
#include "ist_macro.h"
#include "ist_types.h"

#if defined(IST_OS_WIN32) \
	|| defined(IST_OS_ANDROID) \
	|| defined(IST_OS_LINUX) \
	|| defined(IST_OS_LIBC)
#  include <stdlib.h>
#  include <string.h>
#  ifdef IST_DEBUG_MEM
ISTUINT __ist_mem_watch(ISTUINT c, ISTINT op);
ISTVOID * __ist_calloc(ISTUINT c, ISTUINT s);
ISTVOID * __ist_realloc(ISTVOID *p, ISTUINT s);
ISTVOID __ist_free(ISTVOID *p);
ISTVOID __ist_memset(ISTVOID *p, ISTINT v, ISTUINT s);
ISTVOID * __ist_memcpy(ISTVOID *d, CONST ISTVOID *s, ISTUINT l);
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

#include "ist_errno.h"
#include "ist_debug.h"
#include "ist_math.h"

#ifdef IST_FIFO
#  include "ist_fifo.h"
#endif // IST_FIFO

#ifdef IST_ALMA
#  include "ist_alma.h"
#endif // IST_ALMA

#ifdef IST_SMA
#  include "ist_sma.h"
#endif // IST_SMA

#ifdef IST_RMA
#  include "ist_rma.h"
#endif // IST_RMA

#ifdef IST_MMA
#  include "ist_mma.h"
#endif // IST_MMA

#ifdef IST_WMA
#  include "ist_wma.h"
#endif // IST_WMA

#ifdef IST_BIQUAD_LPF
#  include "ist_bqdlpf.h"
#endif // IST_BIQUAD_LPF

#ifdef IST_SPHERE
#  include "ist_sphere.h"
#endif // IST_SPHERE

#ifdef IST_ELLIPSOID
#  include "ist_ellipsoid.h"
#endif // IST_ELLIPSOID

#ifdef IST_HIRON
#  include "ist_hardiron.h"
#endif // IST_HIRON

#ifdef IST_SIRON
#  include "ist_softiron.h"
#endif // IST_SIRON

#ifdef IST_MAGNET
#  include "ist_magnet.h"
#endif // IST_MAGNET

#ifdef IST_ACCEL
#  include "ist_accel.h"
#endif // IST_ACCEL

#ifdef IST_ROTATION
#  include "ist_rotation.h"
#endif // IST_ROTATION

#endif // IST_MILIEU_H_INCLUDED
