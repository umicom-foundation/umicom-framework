/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/coverage.h
 *
 * PURPOSE:
 *   Define a reusable test-explorer and test-run record independent of any single test framework.
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
#ifndef UMICOM_TEST_PLATFORM_COVERAGE_H
#define UMICOM_TEST_PLATFORM_COVERAGE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_COVERAGE_CAPACITY 4096U
#define UMI_TEST_PLATFORM_COVERAGE_API_VERSION 1U

/**
 * Represent the test platform coverage snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformCoverageSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char uri[1024];
    uint64_t lines_total;
    uint64_t lines_covered;
    uint64_t branches_total;
    uint64_t branches_covered;
    uint64_t revision;
} UmiTestPlatformCoverageSnapshot;

/**
 * Represent the test platform coverage registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformCoverageRegistry UmiTestPlatformCoverageRegistry;

/**
 * Initialise test platform coverage registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_coverage_registry_create(UmiTestPlatformCoverageRegistry **out_registry);
/**
 * Release or reset state held by test platform coverage registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_coverage_registry_destroy(UmiTestPlatformCoverageRegistry *registry);
/**
 * Provide the test platform coverage registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_coverage_registry_upsert(UmiTestPlatformCoverageRegistry *registry, const UmiTestPlatformCoverageSnapshot *item);
/**
 * Remove test platform coverage registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_coverage_registry_remove(UmiTestPlatformCoverageRegistry *registry, const char *id);
/**
 * Find test platform coverage registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_coverage_registry_find(const UmiTestPlatformCoverageRegistry *registry, const char *id, UmiTestPlatformCoverageSnapshot *out_item);
/**
 * Find test platform coverage registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_coverage_registry_at(const UmiTestPlatformCoverageRegistry *registry, size_t index, UmiTestPlatformCoverageSnapshot *out_item);
/**
 * Return the number of records represented by test platform coverage registry without
 * changing their state.
 */
size_t umi_test_platform_coverage_registry_count(const UmiTestPlatformCoverageRegistry *registry);
/**
 * Provide the test platform coverage registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_coverage_registry_revision(const UmiTestPlatformCoverageRegistry *registry);
/**
 * Release or reset state held by test platform coverage registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_coverage_registry_clear(UmiTestPlatformCoverageRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_test_platform_coverage_snapshot_validate(const UmiTestPlatformCoverageSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_TEST_PLATFORM_COVERAGE_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_test_platform_coverage_registry_upsert_many(UmiTestPlatformCoverageRegistry *registry,
    const UmiTestPlatformCoverageSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_test_platform_coverage_registry_capture(const UmiTestPlatformCoverageRegistry *registry,
    UmiTestPlatformCoverageSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_test_platform_coverage_registry_replace_if_current(UmiTestPlatformCoverageRegistry *registry,
    uint64_t expected_revision, const UmiTestPlatformCoverageSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiTestPlatformCoverageEdit {
    UmiSnapshotEditKind kind;
    UmiTestPlatformCoverageSnapshot item;
} UmiTestPlatformCoverageEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_TEST_PLATFORM_COVERAGE_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_test_platform_coverage_registry_edit_if_current(UmiTestPlatformCoverageRegistry *registry,
    uint64_t expected_revision, const UmiTestPlatformCoverageEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

/** Read up to capacity records starting at offset, at expected_revision.
 * Obtain the first revision with umi_test_platform_coverage_registry_revision and retain it for
 * every page. INVALID_STATE means the registry changed: discard the partial
 * collection and start a new observation. Offsets past the count are invalid;
 * an offset equal to the count returns an empty final page. A zero capacity
 * permits null items and returns metadata only; it does not advance the cursor.
 * All refusals leave items and out_page unchanged. Success copies accepted
 * values in registry order without changing the owner. items must have room
 * for capacity records; out_page is required. Output storage must not overlap
 * the registry, each other, or concurrently used storage. This performs no
 * allocation or I/O. Serialize all calls and mutations on the registry owner. */
UmiStatus umi_test_platform_coverage_registry_read_page(const UmiTestPlatformCoverageRegistry *registry,
    uint64_t expected_revision, size_t offset, UmiTestPlatformCoverageSnapshot *items,
    size_t capacity, UmiSnapshotPage *out_page);

#ifdef __cplusplus
}
#endif

#endif
