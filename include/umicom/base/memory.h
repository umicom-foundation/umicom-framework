/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/memory.h
 *
 * PURPOSE:
 *   Publish the public memory contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_MEMORY_H
#define UMICOM_BASE_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*UmiAllocateFn)(size_t size, void *user_data);
typedef void *(*UmiReallocateFn)(void *memory, size_t size, void *user_data);
typedef void (*UmiFreeFn)(void *memory, void *user_data);

/**
 * Represent the allocator data shared with callers of this public contract.
 */
typedef struct UmiAllocator {
    UmiAllocateFn allocate;
    UmiReallocateFn reallocate;
    UmiFreeFn release;
    void *user_data;
} UmiAllocator;

/**
 * Provide the allocator system operation used by this module and its client applications.
 */
UmiAllocator umi_allocator_system(void);
/**
 * Provide the alloc operation used by this module and its client applications.
 */
void *umi_alloc(const UmiAllocator *allocator, size_t size);
/**
 * Provide the realloc operation used by this module and its client applications.
 */
void *umi_realloc(const UmiAllocator *allocator, void *memory, size_t size);
/**
 * Provide the free operation used by this module and its client applications.
 */
void umi_free(const UmiAllocator *allocator, void *memory);

#ifdef __cplusplus
}
#endif

#endif
