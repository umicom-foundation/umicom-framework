/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/alignment.h
 *
 * PURPOSE:
 *   Define deterministic alignment and distribution operations for visual design surfaces.
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
#ifndef UMICOM_DESIGNER_ALIGNMENT_H
#define UMICOM_DESIGNER_ALIGNMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_ALIGNMENT_CAPACITY 256U

/**
 * Represent the designer alignment snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerAlignmentSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char operation[64];
    char selection_id[128];
    double spacing;
    int horizontal;
    int vertical;
    int distribute;
    uint64_t revision;
} UmiDesignerAlignmentSnapshot;

/**
 * Represent the designer alignment registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerAlignmentRegistry UmiDesignerAlignmentRegistry;

/**
 * Initialise designer alignment registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_alignment_registry_create(UmiDesignerAlignmentRegistry **out_registry);
/**
 * Release or reset state held by designer alignment registry so the same storage can be
 * reused safely.
 */
void umi_designer_alignment_registry_destroy(UmiDesignerAlignmentRegistry *registry);
/**
 * Provide the designer alignment registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_alignment_registry_upsert(UmiDesignerAlignmentRegistry *registry, const UmiDesignerAlignmentSnapshot *item);
/**
 * Remove designer alignment registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_alignment_registry_remove(UmiDesignerAlignmentRegistry *registry, const char *id);
/**
 * Find designer alignment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_alignment_registry_find(const UmiDesignerAlignmentRegistry *registry, const char *id, UmiDesignerAlignmentSnapshot *out_item);
/**
 * Find designer alignment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_alignment_registry_at(const UmiDesignerAlignmentRegistry *registry, size_t index, UmiDesignerAlignmentSnapshot *out_item);
/**
 * Return the number of records represented by designer alignment registry without changing
 * their state.
 */
size_t umi_designer_alignment_registry_count(const UmiDesignerAlignmentRegistry *registry);
/**
 * Provide the designer alignment registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_designer_alignment_registry_revision(const UmiDesignerAlignmentRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_designer_alignment_snapshot_validate(const UmiDesignerAlignmentSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_DESIGNER_ALIGNMENT_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_designer_alignment_registry_upsert_many(UmiDesignerAlignmentRegistry *registry,
    const UmiDesignerAlignmentSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_designer_alignment_registry_capture(const UmiDesignerAlignmentRegistry *registry,
    UmiDesignerAlignmentSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_designer_alignment_registry_replace_if_current(UmiDesignerAlignmentRegistry *registry,
    uint64_t expected_revision, const UmiDesignerAlignmentSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiDesignerAlignmentEdit {
    UmiSnapshotEditKind kind;
    UmiDesignerAlignmentSnapshot item;
} UmiDesignerAlignmentEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_DESIGNER_ALIGNMENT_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_designer_alignment_registry_edit_if_current(UmiDesignerAlignmentRegistry *registry,
    uint64_t expected_revision, const UmiDesignerAlignmentEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

/** Read up to capacity records starting at offset, at expected_revision.
 * Obtain the first revision with umi_designer_alignment_registry_revision and retain it for
 * every page. INVALID_STATE means the registry changed: discard the partial
 * collection and start a new observation. Offsets past the count are invalid;
 * an offset equal to the count returns an empty final page. A zero capacity
 * permits null items and returns metadata only; it does not advance the cursor.
 * All refusals leave items and out_page unchanged. Success copies accepted
 * values in registry order without changing the owner. items must have room
 * for capacity records; out_page is required. Output storage must not overlap
 * the registry, each other, or concurrently used storage. This performs no
 * allocation or I/O. Serialize all calls and mutations on the registry owner. */
UmiStatus umi_designer_alignment_registry_read_page(const UmiDesignerAlignmentRegistry *registry,
    uint64_t expected_revision, size_t offset, UmiDesignerAlignmentSnapshot *items,
    size_t capacity, UmiSnapshotPage *out_page);

#ifdef __cplusplus
}
#endif

#endif
