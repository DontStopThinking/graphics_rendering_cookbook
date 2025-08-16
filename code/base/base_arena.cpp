#include "base/base_arena.h"

#include <cstddef>
#include <cstdlib>
#include <cassert>

#include "base/base_utils.h"

static constexpr size_t ALIGN = alignof(std::max_align_t);

Arena* ArenaCreate(size_t bytes)
{
    Arena* result = new Arena;

    // TODO(sbalse): Use the Windows API to allocate memory so we can use virtual memory.
    result->m_Heap = rcast<u8*>(std::calloc(1, bytes));
    result->m_UsedSize = 0;
    result->m_Capacity = bytes;

    return result;
}

void* ArenaPush(Arena* arena, size_t bytes)
{
    size_t paddedSize = AlignToNextPow2(bytes, ALIGN);

    // TODO(sbalse): Custom assert and log macro
    assert((arena->m_UsedSize + paddedSize) < arena->m_Capacity);

    u8* ptr = &arena->m_Heap[arena->m_UsedSize];

    arena->m_UsedSize += paddedSize;

    return ptr;
}

void ArenaClear(Arena* arena)
{
    arena->m_UsedSize = 0;
}

void ArenaDestroy(Arena* arena)
{
    std::free(arena->m_Heap);
    delete arena;
}
