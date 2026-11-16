/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/budget_allocator.h
 *
 * PURPOSE:
 *   Bound requested allocation bytes and live owners through the existing allocator callback contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_BASE_BUDGET_ALLOCATOR_H
#define UMICOM_BASE_BUDGET_ALLOCATOR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/memory.h"
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUDGET_ALLOCATOR_MAX_BLOCKS 4096U
typedef struct UmiBudgetAllocator UmiBudgetAllocator;
typedef struct UmiBudgetAllocatorSnapshot {
    size_t byte_limit;
    size_t block_limit;
    size_t live_bytes;
    size_t live_blocks;
    size_t peak_bytes;
    size_t metadata_bytes;
    uint64_t capacity_refusals;
    uint64_t allocation_failures;
    uint64_t invalid_requests;
} UmiBudgetAllocatorSnapshot;

/* Create one private owner and a bounded allocation table. byte_limit and
 * block_limit must be positive; block_limit <= MAX_BLOCKS. out_owner must point
 * to NULL and is unchanged on failure. A NULL parent selects the system
 * allocator; a supplied parent must provide allocate and release. Its callbacks
 * and user_data are copied, and their underlying owner must outlive this one.
 * The parent must return nonoverlapping, suitably aligned storage and allow
 * release of exactly its returned pointers. Its reallocate callback is unused.
 *
 * The byte limit bounds live requested payload, including both buffers during
 * copy-resize. The separately reported fixed metadata allocation is not charged
 * to it. Allocator overhead, external allocations, devices and OS resident
 * memory are not measured or bounded. Allocation can fail below the limit.
 * Callers serialize all operations and parent callbacks must not reenter this
 * owner. This is opt-in; existing system allocation behavior is unchanged. */
UmiStatus umi_budget_allocator_create(const UmiAllocator *parent,size_t byte_limit,
    size_t block_limit,UmiBudgetAllocator **out_owner);

/* Allocate a positive number of bytes with unspecified initial contents.
 * out_memory must point to NULL; failure preserves it. No parent allocation is
 * attempted when either live-byte or live-block admission would fail. */
UmiStatus umi_budget_allocator_allocate(UmiBudgetAllocator *owner,size_t bytes,
    void **out_memory);

/* Allocate a separate replacement, copy min(old,new) bytes, then release the
 * old owner. The transient old+new payload must fit. NULL memory means allocate;
 * bytes==0 is refused and never frees. Same-size requests return the existing
 * pointer without allocation. Use an actual void* variable for out_memory;
 * it must hold NULL or memory and must not itself reside inside a managed block.
 * A refusal preserves the old pointer, contents and out_memory. Newly added
 * bytes are unspecified. Success invalidates the old pointer and all its views.
 * Exact currently-owned starts are accepted; interior and foreign pointers are
 * refused without reading their payload. A stale pointer whose address has been
 * reused cannot be detected: callers must discard released pointers. */
UmiStatus umi_budget_allocator_resize(UmiBudgetAllocator *owner,void *memory,
    size_t bytes,void **out_memory);

/* Release one exact owned allocation. NULL is a successful no-op. A refused
 * foreign/interior pointer is never passed to the parent. The caller must clear
 * all pointer aliases after success; this function cannot find those aliases. */
UmiStatus umi_budget_allocator_release(UmiBudgetAllocator *owner,void *memory);

/* Read copied accounting, not process RSS or a proof of absence of leaks.
 * Peak includes transient successful copy-resize allocation. Diagnostic counters
 * saturate at UINT64_MAX. Failed calls can change counters, not live ownership. */
UmiStatus umi_budget_allocator_snapshot(const UmiBudgetAllocator *owner,
    UmiBudgetAllocatorSnapshot *out_snapshot);

/* Borrow an adapter for existing UmiAllocator consumers. It remains valid only
 * while owner lives. Allocation/resize refusal returns NULL. Its void release
 * callback records invalid pointers in invalid_requests; use the explicit API
 * when the caller must receive the error. Zero-size resize preserves its input.
 * All clients must release allocations through this same owner, not free(). */
UmiStatus umi_budget_allocator_adapter(UmiBudgetAllocator *owner,UmiAllocator *out_adapter);

/* Refuse with BUSY while live allocations remain. This never hides a leak by
 * freeing arbitrary outstanding buffers. On success release metadata and set
 * *owner to NULL. Passing a NULL owner value is an idempotent success. */
UmiStatus umi_budget_allocator_destroy(UmiBudgetAllocator **owner);

#ifdef __cplusplus
}
#endif
#endif
