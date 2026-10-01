/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/console_entry.h
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
#ifndef UMICOM_DEBUG_CONSOLE_ENTRY_H
#define UMICOM_DEBUG_CONSOLE_ENTRY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_CONSOLE_ENTRY_CAPACITY 2048U
#define UMI_DEBUG_CONSOLE_ENTRY_API_VERSION 1U

/**
 * Represent the debug console entry snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugConsoleEntrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char category[64];
    char text[2048];
    uint64_t timestamp;
    int severity;
    uint64_t revision;
} UmiDebugConsoleEntrySnapshot;

/**
 * Represent the debug console entry registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugConsoleEntryRegistry UmiDebugConsoleEntryRegistry;

/**
 * Initialise debug console entry registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_console_entry_registry_create(UmiDebugConsoleEntryRegistry **out_registry);
/**
 * Release or reset state held by debug console entry registry so the same storage can be
 * reused safely.
 */
void umi_debug_console_entry_registry_destroy(UmiDebugConsoleEntryRegistry *registry);
/**
 * Provide the debug console entry registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_console_entry_registry_upsert(UmiDebugConsoleEntryRegistry *registry, const UmiDebugConsoleEntrySnapshot *item);
/**
 * Remove debug console entry registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_console_entry_registry_remove(UmiDebugConsoleEntryRegistry *registry, const char *id);
/**
 * Find debug console entry registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_console_entry_registry_find(const UmiDebugConsoleEntryRegistry *registry, const char *id, UmiDebugConsoleEntrySnapshot *out_item);
/**
 * Find debug console entry registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_console_entry_registry_at(const UmiDebugConsoleEntryRegistry *registry, size_t index, UmiDebugConsoleEntrySnapshot *out_item);
/**
 * Return the number of records represented by debug console entry registry without
 * changing their state.
 */
size_t umi_debug_console_entry_registry_count(const UmiDebugConsoleEntryRegistry *registry);
/**
 * Provide the debug console entry registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_console_entry_registry_revision(const UmiDebugConsoleEntryRegistry *registry);
/**
 * Release or reset state held by debug console entry registry so the same storage can be
 * reused safely.
 */
void umi_debug_console_entry_registry_clear(UmiDebugConsoleEntryRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_debug_console_entry_snapshot_validate(const UmiDebugConsoleEntrySnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_DEBUG_CONSOLE_ENTRY_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_debug_console_entry_registry_upsert_many(UmiDebugConsoleEntryRegistry *registry,
    const UmiDebugConsoleEntrySnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_debug_console_entry_registry_capture(const UmiDebugConsoleEntryRegistry *registry,
    UmiDebugConsoleEntrySnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

/** Replace the complete collection only while expected_revision still matches
 * this registry. Use a revision from capture, not a different registry. A stale
 * proposal returns INVALID_STATE without changing records. IDs must be unique;
 * an empty proposal clears the collection, except an already-empty collection
 * needs no change. Successful publication advances the revision once and gives
 * every stored row that revision. Revision exhaustion returns CAPACITY_EXCEEDED.
 * Existing upsert normalisation remains in effect. Any validation/allocation
 * failure retains the complete previous collection. Inputs and optional result
 * must not overlap each other or registry storage; serialize owner access.
 * This is an in-memory publication, not a thread lock or a durable disk save. */
UmiStatus umi_debug_console_entry_registry_replace_if_current(UmiDebugConsoleEntryRegistry *registry,
    uint64_t expected_revision, const UmiDebugConsoleEntrySnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
