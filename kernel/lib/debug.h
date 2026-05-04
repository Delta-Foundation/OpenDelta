#ifndef _OPEN_DELTA_DEBUG_H
#define _OPEN_DELTA_DEBUG_H

#include "./stdbase.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_LOG_LEVEL LVL_DEBUG

typedef enum {
    LVL_DEBUG = 0,
    LVL_INFO = 1,
    LVL_WARN = 2,
    LVL_ERROR = 3,
    LVL_CRITICAL = 4
} debug_level;

void logf(const char *module, debug_level level, const char *fmt, ...);
#define log_debug(modele, ...) logf(module, LVL_DEBUG, __VA_ARGS__)
#define log_info(module, ...) logf(module, LVL_INFO, __VA_ARGS__)
#define log_warn(module, ...) logf(module, LVL_WARN, __VA_ARGS__)
#define log_err(module, ...) logf(module, LVL_ERROR, __VA_ARGS__)
#define log_crit(module, ...) logf(module, LVL_CRITICAL, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif 
