#include "ist_milieu.h"

#ifdef IST_DEBUG_MEM

static size_t memx = 0;
static size_t memc = 0;

size_t __ist_mem_watch(size_t c, int op)
{
	const char *opname;
	size_t pn;
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

void *__ist_calloc(size_t c, size_t s)
{
	size_t cs = c*s;
	void *p = NULL;

	if (cs > 0) {
		p = malloc(cs+sizeof(size_t));
		if (p) {
			__ist_mem_watch(cs, 1);
			*(size_t *)p = cs;
			p = (void *)((size_t *)p + 1);
		}
	}
	return p;
}

void *__ist_realloc(void *p, size_t s)
{
	size_t cs = p ? *((size_t *)p - 1) : 0;
	void *op = p ? ((size_t *)p - 1) : NULL;
	void *np = realloc(op, s+sizeof(size_t));
	if (np) {
		if (cs > s) {
			__ist_mem_watch(cs-s, -1);
		} else if (cs < s) {
			__ist_mem_watch(s-cs, 1);
		}
		*(size_t *)np = s;
		np = (void *)((size_t *)np + 1);
	} else {
		__ist_mem_watch(cs, -1);
	}
	return np;
}

void __ist_free(void *p)
{
	size_t cs = p ? *((size_t *)p - 1) : 0;
	void *op = p ? (size_t *)p - 1 : NULL;
	if (op) {
		free(op);
		__ist_mem_watch(cs, -1);
	}
	return;
}

void __ist_memset(void *p, int v, size_t s)
{
	memset(p, v, s);
}

void *__ist_memcpy(void *d, const void *s, size_t l)
{
	return memcpy(d, s, l);
}

#endif // IST_DEBUG_MEM