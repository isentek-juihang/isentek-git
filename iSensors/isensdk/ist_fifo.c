#include "ist_milieu.h"

#ifdef IST_FIFO

struct fifo_node {
	struct fifo_node *next;
	struct fifo_node *prev;
	FIFOParcel par;
};

struct fifo_buffer {
	FIFOBuffer pub;
	struct fifo_node head;
	ISTINT size;
	ISTINT need_destroy;
};

STATIC FIFOBuffer *New(ISTVOID);
STATIC ISTVOID Delete(FIFOBuffer *self);
STATIC ISTBOOL Put(FIFOBuffer *self, CONST FIFOParcel par);
STATIC FIFOParcel Get(FIFOBuffer *self);
STATIC ISTINT GetSize(FIFOBuffer *self);

STATIC FIFOParcel ThisEmptyParcel;
STATIC FIFOBuffer ThisClass = {0};
STATIC FIFOBuffer *This = NULL;

FIFOBuffer *New(ISTVOID)
{
	struct fifo_buffer *s;

	s = (struct fifo_buffer *)ist_calloc(1, sizeof(struct fifo_buffer));
	if (!s) {
		goto EXIT;
	}
	memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->head.next = s->head.prev = &s->head;
	s->size = 0;
	s->need_destroy = 0;
	return (FIFOBuffer *)s;

EXIT:
	return (FIFOBuffer *)NULL;
}

ISTVOID Delete(FIFOBuffer *self)
{
	struct fifo_buffer *s;
	struct fifo_node *next;
	struct fifo_node *curr;
	struct fifo_node *head;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct fifo_buffer *)self;
	if (s->need_destroy > 0) {
		IST_DBG("FIFO: Resources are not released totally.\n");
#ifdef IST_FIFO_DESTROY
		IST_DBG("FIFO: Force to free resources.\n");
#endif // IST_FIFO_DESTROY
	}
	head = &s->head;
	curr = head->next;
	while (curr && (curr != head)) {
		next = curr->next;
		next->prev = head;
		head->next = next;
#ifdef IST_FIFO_DESTROY
		switch (curr->par.type)
		{
		case E_FIFO_PVOID:
			ist_free(curr->par.data.void_ptr);
			curr->par.data.void_ptr = NULL;
			break;
		default:
			break;
		}
#endif // IST_FIFO_DESTROY
		ist_free(curr);
		curr = head->next;
	}
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTBOOL Put(FIFOBuffer *self, CONST FIFOParcel par)
{
	struct fifo_buffer *s;
	struct fifo_node *add;
	struct fifo_node *head;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct fifo_buffer *)self;
	add = (struct fifo_node *)ist_calloc(1, sizeof(struct fifo_node));
	if (!add) {
		goto EXIT;
	}
	switch (par.type)
	{
	case E_FIFO_PVOID:
		s->need_destroy++;
		break;
	default:
		break;
	}
	s->size++;
	head = &s->head;
	add->par = par;
	add->next = head->next;
	add->prev = head;
	head->next->prev = add;
	head->next = add;
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

FIFOParcel Get(FIFOBuffer *self)
{
	struct fifo_buffer *s;
	struct fifo_node *del;
	struct fifo_node *head;
	FIFOParcel par;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct fifo_buffer *)self;
	head = &s->head;
	del = head->prev;
	if (!del || (del == head)) {
		goto EXIT;
	}
	if (s->size > 0) {
		s->size--;
	}
	del->prev->next = head;
	head->prev = del->prev;
	par = del->par;
	ist_free(del);
	del = NULL;
	return par;

EXIT:
	return ThisEmptyParcel;
}

ISTINT GetSize(FIFOBuffer *self)
{
	struct fifo_buffer *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct fifo_buffer *)self;
	return s->size;

EXIT:
	return 0;
}

FIFOBuffer *IST_FIFOBuffer(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisEmptyParcel.type = E_FIFO_EMPTY;
		ThisEmptyParcel.data.void_ptr = NULL;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Put = Put;
		ThisClass.Get = Get;
		ThisClass.GetSize = GetSize;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_FIFO
