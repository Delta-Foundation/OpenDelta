#ifndef DLTSH_VARS_H
#define DLTSH_VARS_H

#include "dltsh.h"
#include "general.h"

#define VC_HASLOCAL	    0x01
#define VC_HASTMPVAR	0x02
#define VC_FUNCENV	    0x04
#define VC_BLTNENV	    0x08
#define VC_TEMPENV	    0x10

#define VC_TEMPFLAGS	    (VC_FUNCENV|VC_BLTNENV|VC_TEMPENV)

#define vc_isfuncenv(vc)	(((vc)->flags & VC_FUNCENV) != 0)
#define vc_isbltnenv(vc)	(((vc)->flags & VC_BLTNENV) != 0)
#define vc_istempenv(vc)	(((vc)->flags & (VC_TEMPFLAGS)) == VC_TEMPENV)

#define vc_istempscope(vc)	(((vc)->flags & (VC_TEMPENV|VC_BLTNENV)) != 0)

#define vc_haslocals(vc)	(((vc)->flags & VC_HASLOCAL) != 0)
#define vc_hastmpvars(vc)	(((vc)->flags & VC_HASTMPVAR) != 0)

typedef struct var *sh_var_value_func_t __P((struct variable *));

typedef struct var_context {
    char *name;
    int scope; 
    struct var_context *up;
    struct var_context *down;
} VAR_CONTEXT;

typedef struct var {
    char *name;
    char *val;
    char *exportstr;
    sh_var_value_func_t *dynamic_val;
    int attributes;
    int context;
} SHELL_VAR;

#endif 
