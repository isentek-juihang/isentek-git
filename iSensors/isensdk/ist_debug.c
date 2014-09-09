#include "ist_milieu.h"

#ifdef IST_OS_WIN32
#  include <windows.h>
#  include <stdio.h>
#  include <stdarg.h>
#endif

ISTVOID ist_printf(CONST ISTCHAR *fmt, ...)
{
#ifdef IST_DEBUG
#  ifdef IST_OS_WIN32
	ISTCHAR buf[IST_DBG_BUF_SIZE];
	va_list ap;

	va_start(ap, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, ap);
#ifdef IST_DBG_CONSOLE
	fprintf(stdout, "%s", buf);
#endif
	OutputDebugStringA(buf);

	va_end(ap);
#  elif defined(IST_OS_ANDROID)
#    error "Need to implement android debug routines."
#  else
	IST_UNUSED(fmt);
#  endif
#else
    IST_UNUSED(fmt);
#endif
	return;
}
