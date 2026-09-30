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

#ifdef __cplusplus
}
#endif

#endif
