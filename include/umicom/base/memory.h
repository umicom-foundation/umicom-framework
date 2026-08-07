#ifndef UMICOM_BASE_MEMORY_H
#define UMICOM_BASE_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*UmiAllocateFn)(size_t size, void *user_data);
typedef void *(*UmiReallocateFn)(void *memory, size_t size, void *user_data);
typedef void (*UmiFreeFn)(void *memory, void *user_data);

typedef struct UmiAllocator {
    UmiAllocateFn allocate;
    UmiReallocateFn reallocate;
    UmiFreeFn release;
    void *user_data;
} UmiAllocator;

UmiAllocator umi_allocator_system(void);
void *umi_alloc(const UmiAllocator *allocator, size_t size);
void *umi_realloc(const UmiAllocator *allocator, void *memory, size_t size);
void umi_free(const UmiAllocator *allocator, void *memory);

#ifdef __cplusplus
}
#endif

#endif
