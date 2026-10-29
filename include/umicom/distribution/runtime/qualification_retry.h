/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification_retry.h
 *
 * PURPOSE:
 *   Plan source changes and outstanding checks from an identified earlier attempt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_RETRY_H
#define UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_RETRY_H
#include "umicom/distribution/runtime/qualification.h"
#include "umicom/distribution/runtime/observed_impact.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiQualificationRetryHistoryReason {
    UMI_QUALIFICATION_RETRY_NO_HISTORY = 1U,
    UMI_QUALIFICATION_RETRY_OPEN_HISTORY = 2U,
    UMI_QUALIFICATION_RETRY_BASELINE_MISMATCH = 4U,
    UMI_QUALIFICATION_RETRY_INVENTORY_MISMATCH = 8U,
    UMI_QUALIFICATION_RETRY_TOOLCHAIN_MISMATCH = 16U,
    UMI_QUALIFICATION_RETRY_UNSTABLE_ATTEMPT = 32U,
    UMI_QUALIFICATION_RETRY_BUILD_UNQUALIFIED = 64U,
    UMI_QUALIFICATION_RETRY_RUNNER_UNQUALIFIED = 128U,
    UMI_QUALIFICATION_RETRY_INTERRUPTED = 256U
} UmiQualificationRetryHistoryReason;

typedef struct UmiQualificationRetrySummary {
    UmiObservedImpactSummary observation;
    size_t configuredTests;
    size_t requiredTests; /* Includes outstanding names absent from current metadata. */
    size_t historicalPasses; /* Considered, not executed or necessarily excluded. */
    size_t missingResults;
    size_t nonpassingResults;
    size_t removedOutstandingTests;
    uint32_t historyReasons;
} UmiQualificationRetrySummary;

/** Derive the next selection without a manually assembled failed-test list.
 * baseline is the source observation used by previous; current is freshly
 * captured against inventory. previous may be NULL. Current toolchain is a
 * lowercase SHA-256 of compiler/options/commands/relevant environment; NULL
 * means unknown, never unchanged. Malformed non-NULL digests are errors.
 *
 * Prior individual passes are considered only for a closed, uninterrupted,
 * stable attempt with a passed build, compatible inventory and baseline, and
 * matching known toolchain. A recorded unsuccessful aggregate test runner
 * invalidates every fresh pass from that attempt. Without aggregate evidence,
 * only recorded individual passes with known successful exit are considered.
 * Unusable history widens to the full configured target/test graph, including
 * targets with no registered tests. The observation summary retains its own
 * changed-file count; a forced full plan need not enumerate changed paths.
 * Missing, skipped, disabled, cancelled, timed-out and failed checks remain due.
 * Tests absent from that earlier scoped attempt remain due too.
 *
 * Source-driven work and required checks use the same dependency/fixture graph.
 * An outstanding old name absent from the current inventory widens selection,
 * so deleting a registration does not silently erase an unresolved failure.
 * Summary historicalPasses is not a reuse count: source changes can select those
 * tests again. A nonempty inventory with zero registered tests is UNAVAILABLE.
 * Configured test count and accumulated retry requests are bounded to the
 * impact change limit; exhaustion returns an error, never a truncated plan.
 *
 * Inputs remain immutable and are borrowed only during the call. The caller
 * must serialize access to a mutable previous attempt. *outPlan must be NULL.
 * Every failure preserves both outputs; a successful plan owns its names.
 * No files, database, subprocesses, network or Git state are read or changed.
 *
 * This is planning, not execution, authenticated provenance or permission to
 * skip tests. Observations cover declared input bytes, not atomic snapshots.
 * Hosts must capture complete inputs, identify current toolchains/environments,
 * schedule work, retain logs and apply release policy separately. An empty
 * selection is not a fresh passing qualification or publication authority.
 */
UmiStatus UmiReleaseQualificationPlanNext(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    const UmiReleaseQualification *previous, const char *toolchain,
    UmiReleaseImpactPlan **outPlan, UmiQualificationRetrySummary *outSummary);

/** Plan using bounded canonical report bytes retained from an earlier attempt.
 * expectedReportSha256 is mandatory and must be a lowercase SHA-256 retained
 * separately by the host. A mismatch is INVALID_STATE. A matching digest binds
 * bytes to that receipt; it does not authenticate the reporter or prove that
 * its commands ran. The host remains responsible for trusted source evidence.
 *
 * The report is strictly parsed, including all counters and the plan identity,
 * before any selection is published. Its record is temporary and never exposed
 * as an executable or resumed job. Open or incompatible reports widen work under
 * PlanNext rules. Corrupt, unsupported or oversized reports return an error;
 * they never turn into an empty success. The baseline observation is still
 * required to compare individual inputs; NULL widens to the full graph.
 *
 * Length excludes a string terminator. Inputs are borrowed for the call.
 * No file paths from the report are read, no commands run, and both outputs
 * remain unchanged on failure. Destroy the returned plan with ImpactDestroy.
 */
UmiStatus UmiReleaseQualificationPlanFromReport(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    const char *report, size_t length, const char *expectedReportSha256,
    const char *toolchain, UmiReleaseImpactPlan **outPlan,
    UmiQualificationRetrySummary *outSummary);
#ifdef __cplusplus
}
#endif
#endif
