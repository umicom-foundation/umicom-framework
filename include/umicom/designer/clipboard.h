/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/clipboard.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral designer clipboard for copy, cut, paste and duplication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_DESIGNER_CLIPBOARD_H
#define UMICOM_DESIGNER_CLIPBOARD_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_CLIPBOARD_CAPACITY 256U

/**
 * Represent the designer clipboard item snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerClipboardItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char source_node_id[128];
    char component_type[128];
    char serialized[2048];
    uint64_t copied_at;
    uint64_t revision;
} UmiDesignerClipboardItemSnapshot;

/**
 * Represent the designer clipboard item registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerClipboardItemRegistry UmiDesignerClipboardItemRegistry;

/**
 * Initialise designer clipboard registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_clipboard_registry_create(UmiDesignerClipboardItemRegistry **out_registry);
/**
 * Release or reset state held by designer clipboard registry so the same storage can be
 * reused safely.
 */
void umi_designer_clipboard_registry_destroy(UmiDesignerClipboardItemRegistry *registry);
/**
 * Provide the designer clipboard registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_clipboard_registry_upsert(UmiDesignerClipboardItemRegistry *registry, const UmiDesignerClipboardItemSnapshot *item);
/**
 * Remove designer clipboard registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_clipboard_registry_remove(UmiDesignerClipboardItemRegistry *registry, const char *id);
/**
 * Find designer clipboard registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_clipboard_registry_find(const UmiDesignerClipboardItemRegistry *registry, const char *id, UmiDesignerClipboardItemSnapshot *out_item);
/**
 * Find designer clipboard registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_clipboard_registry_at(const UmiDesignerClipboardItemRegistry *registry, size_t index, UmiDesignerClipboardItemSnapshot *out_item);
/**
 * Return the number of records represented by designer clipboard registry without changing
 * their state.
 */
size_t umi_designer_clipboard_registry_count(const UmiDesignerClipboardItemRegistry *registry);
/**
 * Provide the designer clipboard registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_designer_clipboard_registry_revision(const UmiDesignerClipboardItemRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_designer_clipboard_snapshot_validate(const UmiDesignerClipboardItemSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_DESIGNER_CLIPBOARD_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_designer_clipboard_registry_upsert_many(UmiDesignerClipboardItemRegistry *registry,
    const UmiDesignerClipboardItemSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_designer_clipboard_registry_capture(const UmiDesignerClipboardItemRegistry *registry,
    UmiDesignerClipboardItemSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_designer_clipboard_registry_replace_if_current(UmiDesignerClipboardItemRegistry *registry,
    uint64_t expected_revision, const UmiDesignerClipboardItemSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
