#pragma once

#include "base/base_types.h"
#include "base/base_arena.h"

// Reads a file and returns a pointer to it. Also null-terminates the file.
char* ReadFileAndNullTerminateIt(Arena* arena, const char* const filePath);
