#ifndef FS_SHELL_H
#define FS_SHELL_H

#pragma once

#include "../../libc/include/types.h"

boolean fs_shell_execute(
    int argc,
    char ** argv
);

void fs_shell_help(void);

#endif
