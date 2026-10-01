/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/notification_item.h
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
#ifndef UMICOM_UI_NOTIFICATION_ITEM_H
#define UMICOM_UI_NOTIFICATION_ITEM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_NOTIFICATION_ITEM_CAPACITY 4096U
#define UMI_UI_NOTIFICATION_ITEM_API_VERSION 1U

/**
 * Represent the ui notification item snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiNotificationItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char title[256];
    char message[1024];
    char source[128];
    char action_id[128];
    uint64_t timestamp;
    int severity;
    int read;
    int sticky;
    uint64_t revision;
} UmiUiNotificationItemSnapshot;

/**
 * Represent the ui notification item registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiNotificationItemRegistry UmiUiNotificationItemRegistry;

/**
 * Initialise ui notification item registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_notification_item_registry_create(UmiUiNotificationItemRegistry **out_registry);
/**
 * Release or reset state held by ui notification item registry so the same storage can be
 * reused safely.
 */
void umi_ui_notification_item_registry_destroy(UmiUiNotificationItemRegistry *registry);
/**
 * Provide the ui notification item registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_notification_item_registry_upsert(UmiUiNotificationItemRegistry *registry, const UmiUiNotificationItemSnapshot *item);
/**
 * Remove ui notification item registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_notification_item_registry_remove(UmiUiNotificationItemRegistry *registry, const char *id);
/**
 * Find ui notification item registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_notification_item_registry_find(const UmiUiNotificationItemRegistry *registry, const char *id, UmiUiNotificationItemSnapshot *out_item);
/**
 * Find ui notification item registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_notification_item_registry_at(const UmiUiNotificationItemRegistry *registry, size_t index, UmiUiNotificationItemSnapshot *out_item);
/**
 * Return the number of records represented by ui notification item registry without
 * changing their state.
 */
size_t umi_ui_notification_item_registry_count(const UmiUiNotificationItemRegistry *registry);
/**
 * Provide the ui notification item registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_notification_item_registry_revision(const UmiUiNotificationItemRegistry *registry);
/**
 * Release or reset state held by ui notification item registry so the same storage can be
 * reused safely.
 */
void umi_ui_notification_item_registry_clear(UmiUiNotificationItemRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_ui_notification_item_snapshot_validate(const UmiUiNotificationItemSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_UI_NOTIFICATION_ITEM_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_ui_notification_item_registry_upsert_many(UmiUiNotificationItemRegistry *registry,
    const UmiUiNotificationItemSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_ui_notification_item_registry_capture(const UmiUiNotificationItemRegistry *registry,
    UmiUiNotificationItemSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_ui_notification_item_registry_replace_if_current(UmiUiNotificationItemRegistry *registry,
    uint64_t expected_revision, const UmiUiNotificationItemSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiUiNotificationItemEdit {
    UmiSnapshotEditKind kind;
    UmiUiNotificationItemSnapshot item;
} UmiUiNotificationItemEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_UI_NOTIFICATION_ITEM_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_ui_notification_item_registry_edit_if_current(UmiUiNotificationItemRegistry *registry,
    uint64_t expected_revision, const UmiUiNotificationItemEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

/** Read up to capacity records starting at offset, at expected_revision.
 * Obtain the first revision with umi_ui_notification_item_registry_revision and retain it for
 * every page. INVALID_STATE means the registry changed: discard the partial
 * collection and start a new observation. Offsets past the count are invalid;
 * an offset equal to the count returns an empty final page. A zero capacity
 * permits null items and returns metadata only; it does not advance the cursor.
 * All refusals leave items and out_page unchanged. Success copies accepted
 * values in registry order without changing the owner. items must have room
 * for capacity records; out_page is required. Output storage must not overlap
 * the registry, each other, or concurrently used storage. This performs no
 * allocation or I/O. Serialize all calls and mutations on the registry owner. */
UmiStatus umi_ui_notification_item_registry_read_page(const UmiUiNotificationItemRegistry *registry,
    uint64_t expected_revision, size_t offset, UmiUiNotificationItemSnapshot *items,
    size_t capacity, UmiSnapshotPage *out_page);

#ifdef __cplusplus
}
#endif

#endif
