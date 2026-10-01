/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/template_palette.h
 *
 * PURPOSE:
 *   Define reusable visual templates and starter compositions for the Studio designer.
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
#ifndef UMICOM_DESIGNER_TEMPLATE_PALETTE_H
#define UMICOM_DESIGNER_TEMPLATE_PALETTE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_TEMPLATE_PALETTE_CAPACITY 1024U

/**
 * Represent the designer template palette snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerTemplatePaletteSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char category[128];
    char description[512];
    char template_id[128];
    char preview_uri[512];
    int32_t order;
    uint64_t revision;
} UmiDesignerTemplatePaletteSnapshot;

/**
 * Represent the designer template palette registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerTemplatePaletteRegistry UmiDesignerTemplatePaletteRegistry;

/**
 * Initialise designer template palette registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_designer_template_palette_registry_create(UmiDesignerTemplatePaletteRegistry **out_registry);
/**
 * Release or reset state held by designer template palette registry so the same storage
 * can be reused safely.
 */
void umi_designer_template_palette_registry_destroy(UmiDesignerTemplatePaletteRegistry *registry);
/**
 * Provide the designer template palette registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_designer_template_palette_registry_upsert(UmiDesignerTemplatePaletteRegistry *registry, const UmiDesignerTemplatePaletteSnapshot *item);
/**
 * Remove designer template palette registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_designer_template_palette_registry_remove(UmiDesignerTemplatePaletteRegistry *registry, const char *id);
/**
 * Find designer template palette registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_template_palette_registry_find(const UmiDesignerTemplatePaletteRegistry *registry, const char *id, UmiDesignerTemplatePaletteSnapshot *out_item);
/**
 * Find designer template palette registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_template_palette_registry_at(const UmiDesignerTemplatePaletteRegistry *registry, size_t index, UmiDesignerTemplatePaletteSnapshot *out_item);
/**
 * Return the number of records represented by designer template palette registry without
 * changing their state.
 */
size_t umi_designer_template_palette_registry_count(const UmiDesignerTemplatePaletteRegistry *registry);
/**
 * Provide the designer template palette registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_designer_template_palette_registry_revision(const UmiDesignerTemplatePaletteRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_designer_template_palette_snapshot_validate(const UmiDesignerTemplatePaletteSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_DESIGNER_TEMPLATE_PALETTE_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_designer_template_palette_registry_upsert_many(UmiDesignerTemplatePaletteRegistry *registry,
    const UmiDesignerTemplatePaletteSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_designer_template_palette_registry_capture(const UmiDesignerTemplatePaletteRegistry *registry,
    UmiDesignerTemplatePaletteSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_designer_template_palette_registry_replace_if_current(UmiDesignerTemplatePaletteRegistry *registry,
    uint64_t expected_revision, const UmiDesignerTemplatePaletteSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiDesignerTemplatePaletteEdit {
    UmiSnapshotEditKind kind;
    UmiDesignerTemplatePaletteSnapshot item;
} UmiDesignerTemplatePaletteEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_DESIGNER_TEMPLATE_PALETTE_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_designer_template_palette_registry_edit_if_current(UmiDesignerTemplatePaletteRegistry *registry,
    uint64_t expected_revision, const UmiDesignerTemplatePaletteEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
