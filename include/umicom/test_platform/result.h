/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/result.h
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
#ifndef UMICOM_TEST_PLATFORM_RESULT_H
#define UMICOM_TEST_PLATFORM_RESULT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_RESULT_CAPACITY 4096U
#define UMI_TEST_PLATFORM_RESULT_API_VERSION 2U

/**
 * List the named test platform outcome values accepted by this public contract.
 */
typedef enum UmiTestPlatformOutcome {
    UMI_TEST_PLATFORM_OUTCOME_NOT_RUN = 0,
    UMI_TEST_PLATFORM_OUTCOME_PASSED = 1,
    UMI_TEST_PLATFORM_OUTCOME_FAILED = 2,
    UMI_TEST_PLATFORM_OUTCOME_SKIPPED = 3,
    UMI_TEST_PLATFORM_OUTCOME_CANCELLED = 4,
    UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT = 5
} UmiTestPlatformOutcome;

/**
 * Represent the test platform result snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformResultSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char item_id[128];
    char message[1024];
    char failure_details[2048];
    char attachment_id[128];
    double duration_ms;
    int outcome;
    int exit_code;
    int flaky;
    uint64_t sequence;
    uint64_t revision;
} UmiTestPlatformResultSnapshot;

/**
 * Provide the test platform outcome text operation used by this module and its client
 * applications.
 */
const char *umi_test_platform_outcome_text(UmiTestPlatformOutcome outcome);

/**
 * Represent the test platform result registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformResultRegistry UmiTestPlatformResultRegistry;

/**
 * Initialise test platform result registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_platform_result_registry_create(UmiTestPlatformResultRegistry **out_registry);
/**
 * Release or reset state held by test platform result registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_result_registry_destroy(UmiTestPlatformResultRegistry *registry);
/**
 * Provide the test platform result registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_result_registry_upsert(UmiTestPlatformResultRegistry *registry, const UmiTestPlatformResultSnapshot *item);
/**
 * Remove test platform result registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_test_platform_result_registry_remove(UmiTestPlatformResultRegistry *registry, const char *id);
/**
 * Find test platform result registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_result_registry_find(const UmiTestPlatformResultRegistry *registry, const char *id, UmiTestPlatformResultSnapshot *out_item);
/**
 * Find test platform result registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_result_registry_at(const UmiTestPlatformResultRegistry *registry, size_t index, UmiTestPlatformResultSnapshot *out_item);
/**
 * Return the number of records represented by test platform result registry without
 * changing their state.
 */
size_t umi_test_platform_result_registry_count(const UmiTestPlatformResultRegistry *registry);
/**
 * Provide the test platform result registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_test_platform_result_registry_revision(const UmiTestPlatformResultRegistry *registry);
/**
 * Release or reset state held by test platform result registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_result_registry_clear(UmiTestPlatformResultRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_test_platform_result_snapshot_validate(const UmiTestPlatformResultSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_TEST_PLATFORM_RESULT_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_test_platform_result_registry_upsert_many(UmiTestPlatformResultRegistry *registry,
    const UmiTestPlatformResultSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
