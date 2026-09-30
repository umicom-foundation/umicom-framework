/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/variable.h
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
#ifndef UMICOM_DEBUG_VARIABLE_H
#define UMICOM_DEBUG_VARIABLE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_VARIABLE_CAPACITY 2048U
#define UMI_DEBUG_VARIABLE_API_VERSION 1U

/**
 * Represent the debug variable snapshot data shared with callers of this public contract.
 */
typedef struct UmiDebugVariableSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char scope_id[128];
    char name[256];
    char value[1024];
    char type[256];
    char evaluate_name[512];
    uint64_t variables_reference;
    int changed;
    uint64_t revision;
} UmiDebugVariableSnapshot;

/**
 * Represent the debug variable registry data shared with callers of this public contract.
 */
typedef struct UmiDebugVariableRegistry UmiDebugVariableRegistry;

/**
 * Initialise debug variable registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_variable_registry_create(UmiDebugVariableRegistry **out_registry);
/**
 * Release or reset state held by debug variable registry so the same storage can be reused
 * safely.
 */
void umi_debug_variable_registry_destroy(UmiDebugVariableRegistry *registry);
/**
 * Provide the debug variable registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_variable_registry_upsert(UmiDebugVariableRegistry *registry, const UmiDebugVariableSnapshot *item);
/**
 * Remove debug variable registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_variable_registry_remove(UmiDebugVariableRegistry *registry, const char *id);
/**
 * Find debug variable registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_variable_registry_find(const UmiDebugVariableRegistry *registry, const char *id, UmiDebugVariableSnapshot *out_item);
/**
 * Find debug variable registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_variable_registry_at(const UmiDebugVariableRegistry *registry, size_t index, UmiDebugVariableSnapshot *out_item);
/**
 * Return the number of records represented by debug variable registry without changing
 * their state.
 */
size_t umi_debug_variable_registry_count(const UmiDebugVariableRegistry *registry);
/**
 * Provide the debug variable registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_variable_registry_revision(const UmiDebugVariableRegistry *registry);
/**
 * Release or reset state held by debug variable registry so the same storage can be reused
 * safely.
 */
void umi_debug_variable_registry_clear(UmiDebugVariableRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_debug_variable_snapshot_validate(const UmiDebugVariableSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_DEBUG_VARIABLE_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_debug_variable_registry_upsert_many(UmiDebugVariableRegistry *registry,
    const UmiDebugVariableSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
