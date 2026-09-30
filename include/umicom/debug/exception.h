/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/exception.h
 *
 * PURPOSE:
 *   Define a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_DEBUG_EXCEPTION_H
#define UMICOM_DEBUG_EXCEPTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_EXCEPTION_CAPACITY 2048U
#define UMI_DEBUG_EXCEPTION_API_VERSION 1U

/**
 * Represent the debug exception snapshot data shared with callers of this public contract.
 */
typedef struct UmiDebugExceptionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char type[256];
    char message[1024];
    char break_mode[64];
    int caught;
    uint64_t revision;
} UmiDebugExceptionSnapshot;

/**
 * Represent the debug exception registry data shared with callers of this public contract.
 */
typedef struct UmiDebugExceptionRegistry UmiDebugExceptionRegistry;

/**
 * Initialise debug exception registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_exception_registry_create(UmiDebugExceptionRegistry **out_registry);
/**
 * Release or reset state held by debug exception registry so the same storage can be
 * reused safely.
 */
void umi_debug_exception_registry_destroy(UmiDebugExceptionRegistry *registry);
/**
 * Provide the debug exception registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_exception_registry_upsert(UmiDebugExceptionRegistry *registry, const UmiDebugExceptionSnapshot *item);
/**
 * Remove debug exception registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_exception_registry_remove(UmiDebugExceptionRegistry *registry, const char *id);
/**
 * Find debug exception registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_exception_registry_find(const UmiDebugExceptionRegistry *registry, const char *id, UmiDebugExceptionSnapshot *out_item);
/**
 * Find debug exception registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_exception_registry_at(const UmiDebugExceptionRegistry *registry, size_t index, UmiDebugExceptionSnapshot *out_item);
/**
 * Return the number of records represented by debug exception registry without changing
 * their state.
 */
size_t umi_debug_exception_registry_count(const UmiDebugExceptionRegistry *registry);
/**
 * Provide the debug exception registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_exception_registry_revision(const UmiDebugExceptionRegistry *registry);
/**
 * Release or reset state held by debug exception registry so the same storage can be
 * reused safely.
 */
void umi_debug_exception_registry_clear(UmiDebugExceptionRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_debug_exception_snapshot_validate(const UmiDebugExceptionSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_DEBUG_EXCEPTION_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_debug_exception_registry_upsert_many(UmiDebugExceptionRegistry *registry,
    const UmiDebugExceptionSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
