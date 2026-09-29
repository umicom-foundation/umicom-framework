/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/inventory.h
 * Purpose: Inspect configured release inventories without certifying a release.
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_INVENTORY_H
#define UMICOM_DISTRIBUTION_RUNTIME_INVENTORY_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RELEASE_INVENTORY_TEXT_LIMIT (64U * 1024U * 1024U)
#define UMI_RELEASE_INVENTORY_RECORD_LIMIT 200000U
#define UMI_RELEASE_INVENTORY_FIELD_LIMIT (256U * 1024U)

typedef struct UmiReleaseInventory UmiReleaseInventory;
typedef enum UmiReleaseInventoryKind {
    UMI_RELEASE_INVENTORY_HEADER,
    UMI_RELEASE_INVENTORY_TARGET,
    UMI_RELEASE_INVENTORY_SOURCE,
    UMI_RELEASE_INVENTORY_TEST
} UmiReleaseInventoryKind;

/* Strings are owned by the immutable inventory and remain valid until destroy.
 * They are byte-preserved UTF-8 from CMake, not shell commands. Escape control
 * characters before displaying them. Header owners are explicit build-target
 * declarations, not inferred API/symbol ownership or proof of ABI correctness.
 * Source details preserve raw CMake expressions when they cannot be resolved.
 * CTest details describe registration, never a test execution result. */
typedef struct UmiReleaseInventoryRecord {
    UmiReleaseInventoryKind kind;
    const char *identity;
    const char *owner;
    const char *state;
    const char *detail;
} UmiReleaseInventoryRecord;

typedef struct UmiReleaseInventorySummary {
    size_t headers, unassignedHeaders, targets, sources, unresolvedSources;
    size_t tests, disabledTests, missingCommands;
} UmiReleaseInventorySummary;

typedef enum UmiReleaseInventoryDifference {
    UMI_RELEASE_INVENTORY_TEST_MISSING,
    UMI_RELEASE_INVENTORY_TEST_ADDED
} UmiReleaseInventoryDifference;
typedef void (*UmiReleaseInventoryDifferenceFn)(void *context,
    UmiReleaseInventoryDifference difference, const char *testName);
typedef struct UmiReleaseInventoryComparison {
    size_t expectedTests, observedTests, missingTests, addedTests;
    size_t disabledTests, missingCommands;
    bool namesMatch;
} UmiReleaseInventoryComparison;

/* Strict version-1 TSV with hexadecimal string fields. No file access, process
 * execution, Git queries or network calls. No fixed 1,024-entry truncation.
 * Duplicate test/header/target identities are rejected; source identities may
 * appear once per owning target. Embedded NUL, malformed hex, unknown states,
 * missing context and exceeded bounds are errors. Empty input is not a pass.
 * *outInventory must be NULL. Failure preserves it. The input is copied and
 * may be released after success. Results are sorted by kind/identity/owner. */
UmiStatus UmiReleaseInventoryParse(const char *text, size_t length,
    UmiReleaseInventory **outInventory);
void UmiReleaseInventoryDestroy(UmiReleaseInventory *inventory);
size_t UmiReleaseInventoryCount(const UmiReleaseInventory *inventory);
const UmiReleaseInventoryRecord *UmiReleaseInventoryAt(
    const UmiReleaseInventory *inventory, size_t index);
const char *UmiReleaseInventoryProducer(const UmiReleaseInventory *inventory);
const char *UmiReleaseInventoryGeneration(const UmiReleaseInventory *inventory);
const char *UmiReleaseInventorySourceRoot(const UmiReleaseInventory *inventory);
const char *UmiReleaseInventoryBuildRoot(const UmiReleaseInventory *inventory);
const char *UmiReleaseInventoryConfiguration(const UmiReleaseInventory *inventory);
UmiStatus UmiReleaseInventorySummarise(const UmiReleaseInventory *inventory,
    UmiReleaseInventorySummary *outSummary);

/* Compare exact, case-sensitive test-name sets, not merely totals. Inputs must
 * be cmake then ctest from the same generation, source/build roots and config.
 * Context mismatch is INVALID_STATE and leaves *outComparison untouched.
 * Zero registered tests is UNAVAILABLE, not a successful release inventory.
 * On a valid comparison, optional callbacks report each missing/added name;
 * callback strings are borrowed and callbacks must not destroy either input.
 * A matching set says nothing about execution, skips, ownership acceptance,
 * current source bytes, signed provenance, clean Git state or release approval.
 * Disabled tests and absent executable commands remain explicit report fields.
 * Immutable inputs permit concurrent readers; destruction must not race reads. */
UmiStatus UmiReleaseInventoryCompareTests(const UmiReleaseInventory *expected,
    const UmiReleaseInventory *observed,
    UmiReleaseInventoryDifferenceFn difference, void *context,
    UmiReleaseInventoryComparison *outComparison);
#ifdef __cplusplus
}
#endif
#endif
