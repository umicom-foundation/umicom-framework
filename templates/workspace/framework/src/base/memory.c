/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/base/memory.c
 *
 * PURPOSE:
 *   Implement the memory behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/memory.h"

#include <stdlib.h>

static void *system_allocate(size_t size, void *user_data)
{
    (void)user_data;
    return calloc(1U, size);
}

static void *system_reallocate(void *memory, size_t size, void *user_data)
{
    (void)user_data;
    return realloc(memory, size);
}

static void system_release(void *memory, void *user_data)
{
    (void)user_data;
    free(memory);
}

UmiAllocator umi_allocator_system(void)
{
    UmiAllocator allocator;
    allocator.allocate = system_allocate;
    allocator.reallocate = system_reallocate;
    allocator.release = system_release;
    allocator.user_data = 0;
    return allocator;
}

void *umi_alloc(const UmiAllocator *allocator, size_t size)
{
    UmiAllocator fallback;
    if (size == 0U) return 0;
    if (allocator == 0 || allocator->allocate == 0) {
        fallback = umi_allocator_system();
        allocator = &fallback;
    }
    return allocator->allocate(size, allocator->user_data);
}

void *umi_realloc(const UmiAllocator *allocator, void *memory, size_t size)
{
    UmiAllocator fallback;
    if (allocator == 0 || allocator->reallocate == 0) {
        fallback = umi_allocator_system();
        allocator = &fallback;
    }
    return allocator->reallocate(memory, size, allocator->user_data);
}

void umi_free(const UmiAllocator *allocator, void *memory)
{
    UmiAllocator fallback;
    if (memory == 0) return;
    if (allocator == 0 || allocator->release == 0) {
        fallback = umi_allocator_system();
        allocator = &fallback;
    }
    allocator->release(memory, allocator->user_data);
}
