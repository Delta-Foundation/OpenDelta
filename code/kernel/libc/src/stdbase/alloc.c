/* Headers from dltlibc */
#include "../../include/stdbase.h"
#include "../../include/types.h"
#include "../../include/string.h"
#include "../../include/ctype.h"
#include "../../include/stdarg.h"

/* Headers from kernel */
#include "../../../fs/headers/fs.h"
#include "../../../kernel-lib/system.h"

static Header base;
static Header * freep = NULL;
static char * data_start = (char *)0x000010000;
static char * data_end = (char *)&data_start;

uint64_t free_mem_addr = 0x000010000;

int _fillbuf(FILE * stream)
{
    if (stream->cnt > 0) {
        return (unsigned char)*stream->ptr++;
    }

    for (int i = 0; i < BUFSIZE; i++) {
        stream->buf[i] = inb(stream->port);
        if (stream->buf[i] == (unsigned char)EOF) {
            stream->cnt = i;
            break;
        }
    }


    if (stream->cnt == 0) {
        return EOF;
    }

    stream->ptr = stream->base = (char *)stream->buf;
    stream->cnt = BUFSIZE - 1;

    return (uint8_t)*stream->ptr++;
}

int _flushbuf(int c, FILE * stream)
{
    if (stream->cnt == (long)NULL) {
        outb(stream->port, *(stream->ptr)++);
        stream->cnt--;
        return c;
    }

    for (int i = 0; i < BUFSIZE; i++) {
        outb(stream->port, stream->buf[i]);
    }

    stream->ptr = stream->base = (char *)stream->buf;
    stream->cnt = BUFSIZE - 1;

    if (c != EOF) {
        outb(stream->port, c);
    }

    return c;
}

char sbrk(int incr)
{
    char * old_end = data_end;
    char * new_end = old_end + incr;

    if (new_end < old_end) {
        return (char) -1;
    }

    data_end = new_end;

    return * old_end;
}

static Header * morecore(unsigned nu)
{
    char * cp = NULL;
    Header * up;

    if (nu < NALLOC) {
        nu = NALLOC;
    }

    *cp = sbrk(nu * sizeof(Header));
    if (cp == (char *) - 1) {
        return NULL;
    }
    up = (Header *) cp;
    up->s.size = nu;
    free((void *) (up + 1));
    return freep;
}

void * free(void * ap)
{
    Header * bp = NULL, * p;
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr) {
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr)) {
            break;
        }
    }

    if (bp + bp->s.size == p->s.ptr) {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else {
        bp->s.ptr = p->s.ptr;
    }

    if (p + p->s.size == bp) {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    }
    else {
        p->s.ptr = bp;
    }

    freep = p;
    return 0;
}

void * malloc(unsigned nbytes)
{
    Header * p, * prevp;
    unsigned nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = freep) == NULL) {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {
            if (p->s.size == nunits) {
                prevp->s.ptr = p->s.ptr;
            }
            else {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p + 1);
        }

        if (p == freep) {
            if ((p = morecore(nunits)) == NULL) {
                return NULL;
            }
        }
    }
}

void * realloc(
    void * ptr,
    size_t size
) {
    if (ptr == NULL) {
        return malloc(size);
    }

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    void * new_ptr = malloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }

    size_t old_size = 0;
    size_t copy_size = (old_size < size) ? old_size : size;
    memcpy(new_ptr, ptr, copy_size);

    free(ptr);

    return new_ptr;
}

void * kmalloc(
    unsigned int size,
    int align, 
    unsigned int * phys_addr
) {
	if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
		free_mem_addr &= 0xFFFFF000;
		free_mem_addr += 0x1000;
	}
	if (phys_addr) {
		*phys_addr = free_mem_addr;
	}

    void *ret = (void *)free_mem_addr;
	free_mem_addr += size;
	return ret;
}
