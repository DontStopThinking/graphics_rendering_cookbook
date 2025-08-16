#include "base/base_file.h"

#include <cstdio>
#include <cstdlib>

char* ReadFileAndNullTerminateIt(Arena* arena, const char* const filePath)
{
    std::FILE* file = std::fopen(filePath, "rb");
    if (!file)
    {
        return nullptr;
    }
    std::fseek(file, 0, SEEK_END);
    const size_t size = std::ftell(file); // + 1 for the null-terminator
    std::rewind(file);

    char* memory = rcast<char*>(ArenaPush(arena, size));

    std::fread(memory, 1, size, file);
    memory[size] = 0;

    std::fclose(file);
    return memory;
}
