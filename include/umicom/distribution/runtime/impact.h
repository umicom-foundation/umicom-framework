/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/impact.h
 * Purpose: Plan affected targets and tests from explicit configured ownership.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_IMPACT_H
#define UMICOM_DISTRIBUTION_RUNTIME_IMPACT_H
#include "umicom/distribution/runtime/inventory.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RELEASE_IMPACT_CHANGE_LIMIT 100000U
#define UMI_RELEASE_IMPACT_EDGE_LIMIT 1000000U
#define UMI_RELEASE_IMPACT_PATH_LIMIT 4096U

typedef struct UmiReleaseImpactPlan UmiReleaseImpactPlan;
/** Reasons are additive. A full selection is an explicit conservative fallback,
 * never proof that execution has taken place or that source metadata is current. */
typedef enum UmiReleaseImpactReason {
    UMI_RELEASE_IMPACT_CHANGED_SOURCE = 1U,
    UMI_RELEASE_IMPACT_DEPENDENT = 2U,
    UMI_RELEASE_IMPACT_TEST_OWNER = 4U,
    UMI_RELEASE_IMPACT_ALWAYS = 8U,
    UMI_RELEASE_IMPACT_FULL = 16U,
    UMI_RELEASE_IMPACT_TEST_PREREQUISITE = 32U,
    /** Related setup, cleanup or ordered tests selected as a conservative group. */
    UMI_RELEASE_IMPACT_TEST_DEPENDENCY = 64U
} UmiReleaseImpactReason;
/** Version-two test metadata can identify related test registrations. Selection
 * includes the entire connected group and every declared executable prerequisite.
 * This is a coverage decision, not a scheduler: CTest still owns ordering, fixture
 * failure semantics and resource locks. Cycles terminate during selection but
 * invalid execution cycles must still be diagnosed by CTest. */

/** Missing knowledge widens coverage rather than returning an empty success. */
typedef enum UmiReleaseImpactFallback {
    UMI_RELEASE_IMPACT_REQUESTED_FULL = 1U,
    UMI_RELEASE_IMPACT_UNMAPPED_PATH = 2U,
    UMI_RELEASE_IMPACT_BUILD_CONFIGURATION = 4U,
    UMI_RELEASE_IMPACT_INCOMPLETE_GRAPH = 8U,
    UMI_RELEASE_IMPACT_UNOWNED_TEST = 16U,
    UMI_RELEASE_IMPACT_UNRESOLVED_SOURCE = 32U
} UmiReleaseImpactFallback;
typedef struct UmiReleaseImpactSummary {
    size_t changedPaths, targets, tests;
    uint32_t fallbackReasons;
    bool noWork, missingTestCoverage;
} UmiReleaseImpactSummary;
typedef struct UmiReleaseImpactItem {
    UmiReleaseInventoryKind kind;
    const char *name;
    uint32_t reasons;
} UmiReleaseImpactItem;

/** Plan from a cmake inventory and repository-relative changed paths. The
 * inventory must cover the same source root as the paths. Include both names
 * of a rename; deleted/untracked paths remain inputs. No files are read.
 * Callers own inventory and paths through this call; the resulting plan copies
 * all returned names and borrows nothing. *outPlan must be NULL and is unchanged
 * on failure. Invalid paths, malformed metadata and capacity exhaustion are
 * errors, never partial plans. Empty changes means no work, not tests passed.
 * forceFull selects all configured targets/tests even without changed paths.
 * Missing ownership, unresolved graph metadata, build configuration changes and
 * unknown files widen to all targets/tests. Cycles terminate. Generator
 * expressions that were not resolved by the producer remain incomplete.
 * This does not detect source edits since inventory capture or Git commits
 * since a previous run. Refresh metadata and record exact source identity in
 * the executor; this service grants no process or publication authority. */
UmiStatus UmiReleaseImpactCreate(const UmiReleaseInventory *inventory,
    const char *const *changedPaths, size_t changedCount, bool forceFull,
    UmiReleaseImpactPlan **outPlan);
/** Destroy an owned immutable plan; NULL is safe. */
void UmiReleaseImpactDestroy(UmiReleaseImpactPlan *plan);
/** Copy the selection summary; invalid arguments leave it unchanged. */
UmiStatus UmiReleaseImpactDescribe(const UmiReleaseImpactPlan *plan,
    UmiReleaseImpactSummary *outSummary);
/** Return the number of selected targets and tests, never the number passed. */
size_t UmiReleaseImpactCount(const UmiReleaseImpactPlan *plan);
/** Borrow an item until plan destruction; out-of-range returns NULL. */
const UmiReleaseImpactItem *UmiReleaseImpactAt(const UmiReleaseImpactPlan *plan,
    size_t index);

/** Encode the exact selected test names as an anchored CTest regular expression.
 * Call with NULL output and zero capacity to measure (including the terminator).
 * outRequired is assigned only on success. Insufficient capacity leaves output
 * and outRequired unchanged. A full selection, no tests, control characters or
 * an expression exceeding 24 KiB returns UNAVAILABLE: never substitute an
 * empty expression, which would accidentally mean all tests. Output is regex
 * data, not shell-quoted command text. The function performs no I/O. */
UmiStatus UmiReleaseImpactTestExpression(const UmiReleaseImpactPlan *plan,
    char *output, size_t capacity, size_t *outRequired);

/** Borrow the canonical inventory fingerprint until plan destruction.
 * NULL returns NULL. This binds metadata, not mutable source file contents. */
const char *UmiReleaseImpactInventoryFingerprint(const UmiReleaseImpactPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
