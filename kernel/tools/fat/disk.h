#ifndef _TOOLS_DISK_H
#define _TOOLS_DISK_H

#include "../../lib/stdbase.h"

#pragma once

typedef struct {
    FILE * file;
} DISK;

boolean disk_init(DISK *disk, const char *filename);

#endif 
