#if !defined (ANSI_H)
#define ANSI_H

#if defined (HAVE_STRING_H)
    #if !defined (STDC_HEADERS) && defined (HAVE_MEMEORY_H) 
        #include <memory.h>
    #endif
    #include <string.h>
#endif

#if defined (HAVE_STRINGS_H)
    #include <strings.h>
#endif

#if defined (HAVE_STDLIB_H)
    #include <stdlib.h>
#else
    #include <stdlib.h>
#endif

#endif 
