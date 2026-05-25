/* будут использоваться свои библиотеки т.к этот скриптовый язык 
    находится на стадии когда не собрана и не скачана musl libc 
    так что только свои либы
*/
#include "../kernel/lib/stdbase.h"
#include "../kernel/lib/string.h"
#include "../kernel/lib/ctype.h"
#include "../kernel/lib/types.h"

#define MAX_LEN 100

boolean is_delimiter(char chr) {
    return (chr == ' ' || chr == '+' || chr == '-'
            || chr == '*' || chr == '/' || chr == ','
            || chr == ';' || chr == '%' || chr == '>'
            || chr == '<' || chr == '=' || chr == '('
            || chr == ')' || chr == '[' || chr == ']'
            || chr == '{' || chr == '}' 
    );
}

boolean is_operator(char chr) {
    return (chr == '+' || chr == '-' 
            || chr == '*' || chr == '/' 
            || chr == '>' || chr == '<'
            || chr == '='
    );
}

boolean is_valid_identifier(char* str) {
    return (str[0] != '0' && str[0] != '1' && str[0] != '2'
            && str[0] != '3' && str[0] != '4'
            && str[0] != '5' && str[0] != '6'
            && str[0] != '7' && str[0] != '8'
            && str[0] != '9' && !is_delimiter(str[0])
    );
}

boolean is_keyword(char* str)
{
    const char* keywords[]
        = { "func",     "break",    "case",     "let",
            "const",    "continue", "default",  "do",
            "end",      "else",     "enum",     "extern",
            "while",    "for",      "goto",     "if",
            "register", "return",   "sizeof",   "static",
            "struct",   "switch",   "renametype",  "union",
    };
    
    for (int i = 0;
         i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

boolean is_int(char* str) {
    if (str == NULL || *str == '\0') {
        return FALSE;
    }

    int i = 0;
    while (isdigit(str[i])) {
        i++;
    }

    return str[i] == '\0';
}

char* get_substring(char* str, int start, int end) {
    int len = strlen(str);
    int sub_len = end - start + 1;
    char* sub_str
        = (char*)malloc((sub_len + 1) * sizeof(char));
    strncpy(sub_str, str + start, sub_len);
    sub_str[sub_len] = '\0';
    return sub_str;
}

int lexical_analyzer(char* input)
{
    int left = 0, right = 0;
    int len = strlen(input);

    while (right <= len && left <= right) {
        if (!is_delimiter(input[right]))
            right++;

        if (is_delimiter(input[right]) && left == right) {
            if (is_operator(input[right]))
                printf("Token: Operator, Value: %c\n",
                       input[right]);

            right++;
            left = right;
        }
        else if (is_delimiter(input[right]) && left != right
                 || (right == len && left != right)) {
            char* subStr
                = get_substring(input, left, right - 1);

            if (is_keyword(subStr))
                printf("Token: Keyword, Value: %s\n",
                       subStr);

            else if (is_int(subStr))
                printf("Token: Integer, Value: %s\n",
                       subStr);

            else if (is_valid_identifier(subStr)
                     && !is_delimiter(input[right - 1]))
                printf("Token: Identifier, Value: %s\n",
                       subStr);

            else if (!is_valid_identifier(subStr)
                     && !is_delimiter(input[right - 1]))
                printf("Token: Unidentified, Value: %s\n",
                       subStr);
            left = right;
        }
    }
    return 0;
}

int main() 
{
    char lex_input[MAX_LEN] = "let a = b + c";
    printf("For Expression \"%s\":\n", lex_input);
    lexical_analyzer(lex_input);
    printf(" \n");

    char lex_input01[MAX_LEN] = 
        "let x =ab+bc+30+switch+0y ";

     printf("For Expression \"%s\":\n", lex_input01);
     lexical_analyzer(lex_input01);
     return (0);
}
