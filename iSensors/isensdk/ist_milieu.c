#include "ist_milieu.h"

#ifdef IST_DEBUG_MEM

STATIC ISTUINT memx = 0;
STATIC ISTUINT memc = 0;

ISTUINT __ist_mem_watch(ISTUINT c, ISTINT op)
{
	CONST ISTCHAR *opname;
	ISTUINT pn;
	if (op > 0) {
		memc += c;
		opname = "ADD";
		pn = c;
	} else if (op < 0) {
		memc -= c;
		opname = "SUB";
		pn = c;
	} else {
		opname = "WATCH";
		pn = memc;
	}
	if (memx < memc) {
		memx = memc;
		IST_DBG("memx=%u\n", memx);
	}
	IST_DBG("%s %u\n", opname, pn);
	return memc;
}

ISTVOID *__ist_calloc(ISTUINT c, ISTUINT s)
{
	ISTUINT cs = c*s;
	ISTVOID *p = NULL;

	if (cs > 0) {
		p = malloc(cs+sizeof(ISTUINT));
		if (p) {
			__ist_mem_watch(cs, 1);
			*(ISTUINT *)p = cs;
			p = (ISTVOID *)((ISTUINT *)p + 1);
		}
	}
	return p;
}

ISTVOID *__ist_realloc(ISTVOID *p, ISTUINT s)
{
	ISTUINT cs = p ? *((ISTUINT *)p - 1) : 0;
	ISTVOID *op = p ? ((ISTUINT *)p - 1) : NULL;
	ISTVOID *np = realloc(op, s+sizeof(ISTUINT));
	if (np) {
		if (cs > s) {
			__ist_mem_watch(cs-s, -1);
		} else if (cs < s) {
			__ist_mem_watch(s-cs, 1);
		}
		*(ISTUINT *)np = s;
		np = (ISTVOID *)((ISTUINT *)np + 1);
	} else {
		__ist_mem_watch(cs, -1);
	}
	return np;
}

ISTVOID __ist_free(ISTVOID *p)
{
	ISTUINT cs = p ? *((ISTUINT *)p - 1) : 0;
	ISTVOID *op = p ? (ISTUINT *)p - 1 : NULL;
	if (op) {
		free(op);
		__ist_mem_watch(cs, -1);
	}
	return;
}

ISTVOID __ist_memset(ISTVOID *p, ISTINT v, ISTUINT s)
{
	memset(p, v, s);
}

ISTVOID *__ist_memcpy(ISTVOID *d, CONST ISTVOID *s, ISTUINT l)
{
	return memcpy(d, s, l);
}

#endif // IST_DEBUG_MEM