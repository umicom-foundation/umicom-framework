/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/run_session.h
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
#ifndef UMICOM_TEST_PLATFORM_RUN_SESSION_H
#define UMICOM_TEST_PLATFORM_RUN_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_RUN_SESSION_CAPACITY 4096U
#define UMI_TEST_PLATFORM_RUN_SESSION_API_VERSION 1U

/**
 * Represent the test platform run session snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformRunSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char profile_id[128];
    char suite_id[128];
    uint64_t started_at;
    uint64_t finished_at;
    size_t total;
    size_t passed;
    size_t failed;
    size_t skipped;
    int state;
    uint64_t revision;
} UmiTestPlatformRunSessionSnapshot;

/**
 * Represent the test platform run session registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformRunSessionRegistry UmiTestPlatformRunSessionRegistry;

/**
 * Initialise test platform run session registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_run_session_registry_create(UmiTestPlatformRunSessionRegistry **out_registry);
/**
 * Release or reset state held by test platform run session registry so the same storage
 * can be reused safely.
 */
void umi_test_platform_run_session_registry_destroy(UmiTestPlatformRunSessionRegistry *registry);
/**
 * Provide the test platform run session registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_run_session_registry_upsert(UmiTestPlatformRunSessionRegistry *registry, const UmiTestPlatformRunSessionSnapshot *item);
/**
 * Remove test platform run session registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_run_session_registry_remove(UmiTestPlatformRunSessionRegistry *registry, const char *id);
/**
 * Find test platform run session registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_run_session_registry_find(const UmiTestPlatformRunSessionRegistry *registry, const char *id, UmiTestPlatformRunSessionSnapshot *out_item);
/**
 * Find test platform run session registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_run_session_registry_at(const UmiTestPlatformRunSessionRegistry *registry, size_t index, UmiTestPlatformRunSessionSnapshot *out_item);
/**
 * Return the number of records represented by test platform run session registry without
 * changing their state.
 */
size_t umi_test_platform_run_session_registry_count(const UmiTestPlatformRunSessionRegistry *registry);
/**
 * Provide the test platform run session registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_test_platform_run_session_registry_revision(const UmiTestPlatformRunSessionRegistry *registry);
/**
 * Release or reset state held by test platform run session registry so the same storage
 * can be reused safely.
 */
void umi_test_platform_run_session_registry_clear(UmiTestPlatformRunSessionRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_test_platform_run_session_snapshot_validate(const UmiTestPlatformRunSessionSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_TEST_PLATFORM_RUN_SESSION_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_test_platform_run_session_registry_upsert_many(UmiTestPlatformRunSessionRegistry *registry,
    const UmiTestPlatformRunSessionSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_test_platform_run_session_registry_capture(const UmiTestPlatformRunSessionRegistry *registry,
    UmiTestPlatformRunSessionSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_test_platform_run_session_registry_replace_if_current(UmiTestPlatformRunSessionRegistry *registry,
    uint64_t expected_revision, const UmiTestPlatformRunSessionSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiTestPlatformRunSessionEdit {
    UmiSnapshotEditKind kind;
    UmiTestPlatformRunSessionSnapshot item;
} UmiTestPlatformRunSessionEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_TEST_PLATFORM_RUN_SESSION_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_test_platform_run_session_registry_edit_if_current(UmiTestPlatformRunSessionRegistry *registry,
    uint64_t expected_revision, const UmiTestPlatformRunSessionEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
