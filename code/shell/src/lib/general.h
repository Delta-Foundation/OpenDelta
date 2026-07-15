#ifndef DLTSH_GENERAL_H
#define DLTSH_GENERAL_H

#include "types/dltshtypes.h"
#include "types/chartypes.h"
#include "types/xmalloc.h"
#include "dltsh.h"
#include "stdc.h"

#if defined (HAVE_SYS_RESOURCE_H) && defined (RLIMTYPE)
    #if defined (HAVE_SYS_TIME_H)
        #include <sys/time.h>
    #endif
    #include <sys/resource.h>
#endif

#if defined (HAVE_STRING_H)
    #include <string.h>
#else
    #include <strings.h>
#endif /* !HAVE_STRING_H */

#if defined (HAVE_LIMITS_H)
    #include <limits.h>
#endif

#include <stdint.h>

#if !defined (NULL)
    #if defined (__STDC__)
        #define NULL ((void *) 0)
    #else
        #define NULL 0x0
    #endif
#endif

#define pointer_to_int(x)	(int)((char *)x - (char *)0)

#if defined (alpha) && defined (__GNUC__) && !defined (strchr) && !defined (__STDC__)
    extern char *strchr (), *strrchr ();
#endif

#if !defined (strcpy) && (defined (HAVE_DECL_STRCPY) && !HAVE_DECL_STRCPY)
    extern char *strcpy __P((char *, const char *));
#endif

#if !defined (savestring)
    #define savestring(x) (char *)strcpy (xmalloc (1 + strlen (x)), (x))
#endif

#ifndef member
    #define member(c, s) ((c) ? ((char *)mbschr ((s), (c)) != (char *)NULL) : 0)
#endif

#ifndef whitespace
    #define whitespace(c) (((c) == ' ') || ((c) == '\t'))
#endif

#ifndef CHAR_MAX
    #ifdef __CHAR_UNSIGNED__
        #define CHAR_MAX	0xff
    #else
        #define CHAR_MAX	0x7f
    #endif
#endif

#ifndef CHAR_BIT
    #define CHAR_BIT 8
#endif

#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))

#define INT_STRLEN_BOUND(t) \
  ((sizeof (t) * CHAR_BIT - TYPE_SIGNED (t)) * 302 / 1000 \
   + 1 + TYPE_SIGNED (t))

#define legal_variable_starter(c) (ISALPHA(c) || (c == '_'))
#define legal_variable_char(c)	(ISALNUM(c) || c == '_')

#define spctabnl(c)	((c) == ' ' || (c) == '\t' || (c) == '\n')

typedef struct g_list {
    struct g_list *next;
} GENERIC_LIST;

typedef struct {
    char *word;
    int token;
} STRING_INT_ALIST;

#define REVERSE_LIST(list, type) \
  ((list && list->next) ? (type)list_reverse ((GENERIC_LIST *)list) \
			: (type)(list))

#if __GNUC__ > 1
    #define FASTCOPY(s, d, n)  __builtin_memcpy ((d), (s), (n))
#else 
    #if !defined (HAVE_BCOPY)
        #if !defined (HAVE_MEMMOVE)
            #define FASTCOPY(s, d, n)  memcpy ((d), (s), (n))
        #else
            #define FASTCOPY(s, d, n)  memmove ((d), (s), (n))
        #endif
    #else 
        #define FASTCOPY(s, d, n)  bcopy ((s), (d), (n))
    #endif
#endif

#define STREQ(a, b) ((a)[0] == (b)[0] && strcmp(a, b) == 0)
#define STREQN(a, b, n) ((n == 0) ? (1) \
				  : ((a)[0] == (b)[0] && strncmp(a, b, n) == 0))

#define STRLEN(s) (((s) && (s)[0]) ? ((s)[1] ? ((s)[2] ? strlen(s) : 2) : 1) : 0)
#define FREE(s)  do { if (s) free (s); } while (0)
#define MEMBER(c, s) (((c) && c == (s)[0] && !(s)[1]) || (member(c, s)))

#define RESIZE_MALLOCED_BUFFER(str, cind, room, csize, sincr) \
    do { \
        if ((cind) + (room) >= csize) { \
	        while ((cind) + (room) >= csize) \
	            csize += (sincr); \
	        str = xrealloc (str, csize); \
        } \
    } while (0)



#if !defined (_FUNCTION_DEF)
    #define _FUNCTION_DEF
    typedef int Function ();
    typedef void VFunction ();
    typedef char *CPFunction ();
    typedef char **CPPFunction ();
#endif

#ifndef DLTSH_FUNCTION_TYPEDEF
    #define DLTSH_FUNCTION_TYPEDEF
    typedef int dltsh_intfunc_t __P((int));
    typedef int dltsh_ivoidfunc_t __P((void));
    typedef int dltsh_icpfunc_t __P((char *)); 
    typedef int dltsh_icppfunc_t __P((char **));
    typedef int dltsh_iptrfunc_t __P((intptr_t));

    typedef void dltsh_voidfunc_t __P((void));
    typedef void dltsh_vintfunc_t __P((int));
    typedef void dltsh_vcpfunc_t __P((char *));
    typedef void dltsh_vcppfunc_t __P((char **));
    typedef void dltsh_vptrfunc_t __P((intptr_t));

#endif 

#if 0
    extern intptr_t xmalloc __P((size_t));
    extern intptr_t xrealloc __P((void *, size_t));
    extern void xfree __P((void *));
#endif

extern void posix_initialize __P((int));

#if defined (RLIMTYPE)
    extern RLIMTYPE string_to_rlimtype __P((char *));
    extern void print_rlimtype __P((RLIMTYPE, int));
#endif

extern int all_digits __P((const char *));
extern int legal_number __P((const char *, intmax_t *));
extern int legal_identifier __P((const char *));
extern int importable_function_name __P((const char *, size_t));
extern int exportable_function_name __P((const char *));
extern int check_identifier __P((WORD_DESC *, int));
extern int valid_nameref_value __P((const char *, int));
extern int check_selfref __P((const char *, char *, int));
extern int legal_alias_name __P((const char *, int));
extern int assignment __P((const char *, int));

extern int sh_unset_nodelay_mode __P((int));
extern int sh_validfd __P((int));
extern int fd_ispipe __P((int));
extern void check_dev_tty __P((void));
extern int move_to_high_fd __P((int, int, int));
extern int check_binary_file __P((const char *, int));

#ifdef _POSIXSTAT_H_
    extern int same_file __P((const char *, const char *, struct stat *, struct stat *));
#endif

extern int sh_openpipe __P((int *));
extern int sh_closepipe __P((int *));

extern int file_exists __P((const char *));
extern int file_isdir __P((const char  *));
extern int file_iswdir __P((const char  *));
extern int path_dot_or_dotdot __P((const char *));
extern int absolute_pathname __P((const char *));
extern int absolute_program __P((const char *));

extern char *make_absolute __P((const char *, const char *));
extern char *base_pathname __P((char *));
extern char *full_pathname __P((char *));
extern char *polite_directory_format __P((char *));
extern char *trim_pathname __P((char *, int));
extern char *printable_filename __P((char *, int));

extern char *extract_colon_unit __P((char *, int *));

extern void tilde_initialize __P((void));
extern char *bash_tilde_find_word __P((const char *, int, int *));
extern char *bash_tilde_expand __P((const char *, int));

extern int group_member __P((gid_t));
extern char **get_group_list __P((int *));
extern int *get_group_array __P((int *));

extern char *conf_standard_path __P((void));

#endif 
