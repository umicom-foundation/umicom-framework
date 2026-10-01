/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/action_binding.h
 *
 * PURPOSE:
 *   Define visual-designer action bindings to canonical Framework commands.
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
#ifndef UMICOM_DESIGNER_ACTION_BINDING_H
#define UMICOM_DESIGNER_ACTION_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_ACTION_BINDING_CAPACITY 2048U

/**
 * Represent the designer action binding snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerActionBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char node_id[128];
    char action_name[128];
    char command_id[128];
    char state_path[256];
    int enabled;
    uint64_t revision;
} UmiDesignerActionBindingSnapshot;

/**
 * Represent the designer action binding registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerActionBindingRegistry UmiDesignerActionBindingRegistry;

/**
 * Initialise designer action binding registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_designer_action_binding_registry_create(UmiDesignerActionBindingRegistry **out_registry);
/**
 * Release or reset state held by designer action binding registry so the same storage can
 * be reused safely.
 */
void umi_designer_action_binding_registry_destroy(UmiDesignerActionBindingRegistry *registry);
/**
 * Provide the designer action binding registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_designer_action_binding_registry_upsert(UmiDesignerActionBindingRegistry *registry, const UmiDesignerActionBindingSnapshot *item);
/**
 * Remove designer action binding registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_designer_action_binding_registry_remove(UmiDesignerActionBindingRegistry *registry, const char *id);
/**
 * Find designer action binding registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_action_binding_registry_find(const UmiDesignerActionBindingRegistry *registry, const char *id, UmiDesignerActionBindingSnapshot *out_item);
/**
 * Find designer action binding registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_action_binding_registry_at(const UmiDesignerActionBindingRegistry *registry, size_t index, UmiDesignerActionBindingSnapshot *out_item);
/**
 * Return the number of records represented by designer action binding registry without
 * changing their state.
 */
size_t umi_designer_action_binding_registry_count(const UmiDesignerActionBindingRegistry *registry);
/**
 * Provide the designer action binding registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_designer_action_binding_registry_revision(const UmiDesignerActionBindingRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_designer_action_binding_snapshot_validate(const UmiDesignerActionBindingSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_DESIGNER_ACTION_BINDING_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_designer_action_binding_registry_upsert_many(UmiDesignerActionBindingRegistry *registry,
    const UmiDesignerActionBindingSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_designer_action_binding_registry_capture(const UmiDesignerActionBindingRegistry *registry,
    UmiDesignerActionBindingSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_designer_action_binding_registry_replace_if_current(UmiDesignerActionBindingRegistry *registry,
    uint64_t expected_revision, const UmiDesignerActionBindingSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
