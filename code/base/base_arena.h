#pragma once

#include "base/base_types.h"

template <typename T>
struct Range
{
    T* m_Data;
    size_t m_Count;
};

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

template <typename T>
T* PushStruct(Arena* arena)
{
    return scast<T*>(ArenaPush(arena, sizeof(T)));
}

template <typename T>
Range<T> PushArray(Arena* arena, size_t count)
{
    Range<T> result = {};
    result.m_Data = scast<T*>(ArenaPush(arena, sizeof(T) * count));
    result.m_Count = count;
    return result;
}
