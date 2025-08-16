#pragma once

#include "base/base_types.h"

struct Arena
{
    u8* m_Heap;
    size_t m_UsedSize;
    size_t m_Capacity;
};

Arena* ArenaCreate(size_t bytes);
void* ArenaPush(Arena* arena, size_t bytes);
void ArenaClear(Arena* arena);
void ArenaDestroy(Arena* arena);
