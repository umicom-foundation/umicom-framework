/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/navigation_stack.h
 *
 * PURPOSE:
 *   Define an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
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
#ifndef UMICOM_UI_NAVIGATION_STACK_H
#define UMICOM_UI_NAVIGATION_STACK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_NAVIGATION_STACK_CAPACITY 4096U
#define UMI_UI_NAVIGATION_STACK_API_VERSION 1U

/**
 * Represent the ui navigation entry snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiNavigationEntrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char label[256];
    uint32_t line;
    uint32_t column;
    uint64_t visited_at;
    int current;
    uint64_t revision;
} UmiUiNavigationEntrySnapshot;

/**
 * Represent the ui navigation entry registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiNavigationEntryRegistry UmiUiNavigationEntryRegistry;

/**
 * Initialise ui navigation stack registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_navigation_stack_registry_create(UmiUiNavigationEntryRegistry **out_registry);
/**
 * Release or reset state held by ui navigation stack registry so the same storage can be
 * reused safely.
 */
void umi_ui_navigation_stack_registry_destroy(UmiUiNavigationEntryRegistry *registry);
/**
 * Provide the ui navigation stack registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_navigation_stack_registry_upsert(UmiUiNavigationEntryRegistry *registry, const UmiUiNavigationEntrySnapshot *item);
/**
 * Remove ui navigation stack registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_navigation_stack_registry_remove(UmiUiNavigationEntryRegistry *registry, const char *id);
/**
 * Find ui navigation stack registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_navigation_stack_registry_find(const UmiUiNavigationEntryRegistry *registry, const char *id, UmiUiNavigationEntrySnapshot *out_item);
/**
 * Find ui navigation stack registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_navigation_stack_registry_at(const UmiUiNavigationEntryRegistry *registry, size_t index, UmiUiNavigationEntrySnapshot *out_item);
/**
 * Return the number of records represented by ui navigation stack registry without
 * changing their state.
 */
size_t umi_ui_navigation_stack_registry_count(const UmiUiNavigationEntryRegistry *registry);
/**
 * Provide the ui navigation stack registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_navigation_stack_registry_revision(const UmiUiNavigationEntryRegistry *registry);
/**
 * Release or reset state held by ui navigation stack registry so the same storage can be
 * reused safely.
 */
void umi_ui_navigation_stack_registry_clear(UmiUiNavigationEntryRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_ui_navigation_stack_snapshot_validate(const UmiUiNavigationEntrySnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_UI_NAVIGATION_STACK_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_ui_navigation_stack_registry_upsert_many(UmiUiNavigationEntryRegistry *registry,
    const UmiUiNavigationEntrySnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_ui_navigation_stack_registry_capture(const UmiUiNavigationEntryRegistry *registry,
    UmiUiNavigationEntrySnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_ui_navigation_stack_registry_replace_if_current(UmiUiNavigationEntryRegistry *registry,
    uint64_t expected_revision, const UmiUiNavigationEntrySnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
