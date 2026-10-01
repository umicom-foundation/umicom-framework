/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/web_style.h
 *
 * PURPOSE:
 *   Define toolkit-neutral style rules for generated web and embedded-browser frontends.
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
#ifndef UMICOM_FRONTEND_WEB_STYLE_H
#define UMICOM_FRONTEND_WEB_STYLE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_WEB_STYLE_CAPACITY 2048U

/**
 * Represent the frontend style snapshot data shared with callers of this public contract.
 */
typedef struct UmiFrontendStyleSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char selector[256];
    char property[128];
    char value[512];
    char media_query[256];
    int32_t order;
    uint64_t revision;
} UmiFrontendStyleSnapshot;

/**
 * Represent the frontend style registry data shared with callers of this public contract.
 */
typedef struct UmiFrontendStyleRegistry UmiFrontendStyleRegistry;

/**
 * Initialise frontend web style registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_web_style_registry_create(UmiFrontendStyleRegistry **out_registry);
/**
 * Release or reset state held by frontend web style registry so the same storage can be
 * reused safely.
 */
void umi_frontend_web_style_registry_destroy(UmiFrontendStyleRegistry *registry);
/**
 * Provide the frontend web style registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_web_style_registry_upsert(UmiFrontendStyleRegistry *registry, const UmiFrontendStyleSnapshot *item);
/**
 * Remove frontend web style registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_web_style_registry_remove(UmiFrontendStyleRegistry *registry, const char *id);
/**
 * Find frontend web style registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_web_style_registry_find(const UmiFrontendStyleRegistry *registry, const char *id, UmiFrontendStyleSnapshot *out_item);
/**
 * Find frontend web style registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_web_style_registry_at(const UmiFrontendStyleRegistry *registry, size_t index, UmiFrontendStyleSnapshot *out_item);
/**
 * Return the number of records represented by frontend web style registry without changing
 * their state.
 */
size_t umi_frontend_web_style_registry_count(const UmiFrontendStyleRegistry *registry);
/**
 * Provide the frontend web style registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_web_style_registry_revision(const UmiFrontendStyleRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_frontend_web_style_snapshot_validate(const UmiFrontendStyleSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_FRONTEND_WEB_STYLE_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_frontend_web_style_registry_upsert_many(UmiFrontendStyleRegistry *registry,
    const UmiFrontendStyleSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_frontend_web_style_registry_capture(const UmiFrontendStyleRegistry *registry,
    UmiFrontendStyleSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_frontend_web_style_registry_replace_if_current(UmiFrontendStyleRegistry *registry,
    uint64_t expected_revision, const UmiFrontendStyleSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiFrontendStyleEdit {
    UmiSnapshotEditKind kind;
    UmiFrontendStyleSnapshot item;
} UmiFrontendStyleEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_FRONTEND_WEB_STYLE_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_frontend_web_style_registry_edit_if_current(UmiFrontendStyleRegistry *registry,
    uint64_t expected_revision, const UmiFrontendStyleEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
