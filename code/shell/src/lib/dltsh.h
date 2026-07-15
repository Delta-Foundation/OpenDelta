#ifndef DLTSH_H
#define DLTSH_H

#include <stdio.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

extern int EOF_Reached;

#define NO_PIPE         -1
#define REDIRECT_BOTH   -2

#define NO_VARIABLE -1

#define EXECUTION_FAILURE 1
#define EXECUTION_SUCCESS 0

#define EX_BAD_USAGE	2 
#define EX_MISC_ERROR   2

#define EX_RETRY_FAIL	124
#define EX_WEXPCOMSUB	125
#define EX_BINARY_FILE	126
#define EX_NO_EXEC	    126
#define EX_NO_INPUT	    126
#define EX_NOT_FOUND	127

#define EX_SHERR_BASE   256

#define EX_BAD_SYNTAX   257
#define EX_USAGE        258
#define EX_REDIR_FAIL   259
#define EX_BAD_ASSIGN   260
#define EX_EXP_FAIL     261

#define MATCH_ANY   0x000
#define MATCH_BEG   0x001
#define MATCH_BED   0x002

#define MATCH_TYPE_MASK 0x003

#define MATCH_GLOB_REP  0x010
#define MATCH_QUOTED    0x020
#define MATCH_ASSIGNRHS 0x040
#define MATCH_STARSUB   0x080

extern char **shell_environment;

extern int debug_mode;
extern int executing, login_shell;
extern int interactive, interactive_shell;
extern int startup_state;
extern int subshell_environment;
extern int shell_compatibility_level;

extern int locale_mb_cur_max;

enum r_instruction {
  r_output_direction, r_input_direction, r_inputa_direction,
  r_appending_to, r_reading_until, r_reading_string,
  r_duplicating_input, r_duplicating_output, r_deblank_reading_until,
  r_close_this, r_err_and_out, r_input_output, r_output_force,
  r_duplicating_input_word, r_duplicating_output_word,
  r_move_input, r_move_output, r_move_input_word, r_move_output_word,
  r_append_err_and_out
};

#define REDIR_VARASSIGN		0x01

#define AMBIGUOUS_REDIRECT  -1
#define NOCLOBBER_REDIRECT  -2
#define RESTRICTED_REDIRECT -3	
#define HEREDOC_REDIRECT    -4 
#define BADVAR_REDIRECT	    -5

#define CLOBBERING_REDIRECT(ri) \
  (ri == r_output_direction || ri == r_err_and_out)

#define OUTPUT_REDIRECT(ri) \
  (ri == r_output_direction || ri == r_input_output || ri == r_err_and_out || ri == r_append_err_and_out)

#define INPUT_REDIRECT(ri) \
  (ri == r_input_direction || ri == r_inputa_direction || ri == r_input_output)

#define WRITE_REDIRECT(ri) \
  (ri == r_output_direction || \
	ri == r_input_output || \
	ri == r_err_and_out || \
	ri == r_appending_to || \
	ri == r_append_err_and_out || \
	ri == r_output_force)

#define TRANSLATE_REDIRECT(ri) \
  (ri == r_duplicating_input_word || ri == r_duplicating_output_word || \
   ri == r_move_input_word || ri == r_move_output_word)

enum command_type { cm_for, cm_case, cm_while, cm_if, cm_simple, cm_select,
		    cm_connection, cm_function_def, cm_until, cm_group,
		    cm_arith, cm_cond, cm_arith_for, cm_subshell, cm_coproc };

struct fd_bitmap {
    int size;
    char *bitmap;
};

struct user_info {
    char *user_name;
    char *shell;
    char *home_dir;
};

typedef struct word_desc {
    char *word;
    int flags;
} WORD_DESC;

typedef struct word_list {
    struct word_list *next;
    WORD_DESC *word;
} WORD_LIST;

extern struct user_info current_user;

#define HEREDOC_MAX 16

typedef struct _sh_parser_state_t {
    int parser_state;
    int *token_state;
    char *token;
    int token_buf_size;
    int input_line_terminator;
    int eof_encountered;
} sh_parser_state_t;

typedef struct _sh_input_line_state_t {
    char *input_line;
    size_t input_line_index;
    size_t input_line_size;
    size_t input_line_len;
} sh_input_line_state_t;

/*---for-language---*/
#define MAX_STRING_TYPE_LENGTH 1024
#define MAX_CHAR_TYPE_LENGTH 512
#define MAX_INT_TYPE_LENGTH 1024
#define MAX_BOOL_TYPE_LENGTH 128

typedef enum {
    MAX_STR_TYPE_LEN = 2048,
    MAX_CHAR_TYPE_LEN = 1048,
    MAX_INT_TYPE_LEN = 8129,
    MAX_BOOL_TYPE_LEN = 128,
} DltshTypes;

typedef struct {
    const char *for_keyword[128];
    const char *while_keyword[128];
    const char *if_keyword[128];
    const char *then_keyword[128];
    const char *else_keyword[128];
    const char *let_keyword[128];
    const char *func_keyword[128];
    const char *type_keyword[128];
    const char *struct_keyword[128];
    const char *import_keyword[128];
    const char *extern_keyword[128];
} Keywords;

typedef struct {
    const char *double_ampersand[3];
    const char *double_vertical_lines[2];
    const char *plus;
    const char *minus;
    const char *star;
    const char *slash;
    const char *dollar;
} SpecificSymbols;

typedef struct {
    const char *double_ampersand_val[128];
    const char *double_vertical_lines_val[128];
    const char *plus_value[128];
    const char *minus_value[128];
    const char *star_values[128];
    const char *slash_values[128];
    const char *dollar_value[128];
} ValueForSymbols;

typedef struct {
    const char *string_type[MAX_STRING_TYPE_LENGTH];
    const char *character_type[MAX_CHAR_TYPE_LENGTH];
    const char *integer_type[MAX_INT_TYPE_LENGTH];
    const char *bool_type[MAX_BOOL_TYPE_LENGTH];
} TypesNames;

void set_keyword();
void set_symbols();
void set_values();
void set_types();

#ifdef __cplusplus
}
#endif

#endif
