#include "../debug.h"
#include "../stdarg.h"
#include "../stdbase.h"

static const char* const log_colors[] = {
    [LVL_DEBUG]        = "\033[2;37m",
    [LVL_INFO]         = "\033[37m",
    [LVL_WARN]         = "\033[1;33m",
    [LVL_ERROR]        = "\033[1;31m",
    [LVL_CRITICAL]     = "\033[1;37;41m",
};

static const char* const reset_color = "\033[0m";

void logf(const char* module, debug_level level, const char* fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    if (level < MIN_LOG_LEVEL) {
        return;
    }

    fputs(log_colors[level], (FILE *)debug);
    fprintf((FILE *)debug, "[%s]", module);
    vfprintf((FILE *)debug, fmt, args);
    fputs(reset_color, (FILE *)debug);
    fputc('\n', (FILE *)debug);

    va_end(args);
}

void debugc(char c) {
    fputc(c, (FILE *)debug);
}

void debugs(const char* str) {
    fputs(str, (FILE *)debug);
}

void debugf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf((FILE *)debug, fmt, args);
    va_end(args);
}

void debug_buffer(const char* msg, const void* buf, uint32_t count) {
    fprintf_buffer((FILE* )debug, msg, buf, count);
}
