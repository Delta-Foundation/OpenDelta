#ifndef XMALLOC_H
#define XMALLOC_H

#include "../ansi.h"

#ifndef PTR_T

    #if defined (__STDC__)
        #define PTR_T	void *
    #else
        #define PTR_T char *
    #endif

#endif

extern PTR_T xmalloc __P((size_t));
extern PTR_T xrealloc __P((void *, size_t));
extern void xfree __P((void *));

#if defined(USING_BASH_MALLOC) && !defined (DISABLE_MALLOC_WRAPPERS)
    extern PTR_T sh_xmalloc __P((size_t, const char *, int));
    extern PTR_T sh_xrealloc __P((void *, size_t, const char *, int));
    extern void sh_xfree __P((void *, const char *, int));

    #define xmalloc(x)	sh_xmalloc((x), __FILE__, __LINE__)
    #define xrealloc(x, n)	sh_xrealloc((x), (n), __FILE__, __LINE__)
    #define xfree(x)	sh_xfree((x), __FILE__, __LINE__)

    #ifdef free
        #undef free
    #endif
    
    #define free(x)		sh_xfree((x), __FILE__, __LINE__)

    #endif
#endif
