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

/* Provide the system allocate operation used by this module and its client applications. */
static void *system_allocate(size_t size, void *user_data)
{
    (void)user_data;
    return calloc(1U, size);
}

/* Provide the system reallocate operation used by this module and its client applications. */
static void *system_reallocate(void *memory, size_t size, void *user_data)
{
    (void)user_data;
    return realloc(memory, size);
}

/* Release or reset state held by system so the same storage can be reused safely. */
static void system_release(void *memory, void *user_data)
{
    (void)user_data;
    free(memory);
}

/* Provide the allocator system operation used by this module and its client applications. */
UmiAllocator umi_allocator_system(void)
{
    UmiAllocator allocator;
    allocator.allocate = system_allocate;
    allocator.reallocate = system_reallocate;
    allocator.release = system_release;
    allocator.user_data = 0;
    return allocator;
}

/* Provide the alloc operation used by this module and its client applications. */
void *umi_alloc(const UmiAllocator *allocator, size_t size)
{
    UmiAllocator fallback;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (size == 0U) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (allocator == 0 || allocator->allocate == 0) {
        fallback = umi_allocator_system();
        allocator = &fallback;
    }
    return allocator->allocate(size, allocator->user_data);
}

/* Provide the realloc operation used by this module and its client applications. */
void *umi_realloc(const UmiAllocator *allocator, void *memory, size_t size)
{
    UmiAllocator fallback;
    /* Apply this branch only when its contract condition is satisfied. */
    if (allocator == 0 || allocator->reallocate == 0) {
        fallback = umi_allocator_system();
        allocator = &fallback;
    }
    return allocator->reallocate(memory, size, allocator->user_data);
}

/* Provide the free operation used by this module and its client applications. */
void umi_free(const UmiAllocator *allocator, void *memory)
{
    UmiAllocator fallback;
    /* Apply this branch only when its contract condition is satisfied. */
    if (memory == 0) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (allocator == 0 || allocator->release == 0) {
        fallback = umi_allocator_system();
        allocator = &fallback;
    }
    allocator->release(memory, allocator->user_data);
}
