/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/evidence_runs.h
 * PURPOSE: Discover exact run identities before presenting selected-test evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_PLATFORM_EVIDENCE_RUNS_H
#define UMICOM_TEST_PLATFORM_EVIDENCE_RUNS_H
#include "umicom/test_platform/evidence.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestEvidenceRuns UmiTestEvidenceRuns;
typedef struct UmiTestEvidenceRun {
    char session_id[128];
    size_t results;
    size_t output;
} UmiTestEvidenceRun;
typedef struct UmiTestEvidenceRunsSummary {
    char item_id[128];
    uint64_t result_revision, output_revision;
    size_t runs, matching_results, matching_output;
    size_t unspecified_results, unspecified_output;
} UmiTestEvidenceRunsSummary;

/* Discover named sessions from every retained registry record for this exact
 * item. Do not derive choices from a limited evidence capture: an older run
 * must remain selectable even when newer records fill that capture.
 * Rows are ordered by bytewise session ID, not by presumed execution time.
 * Records without a session contribute to the unspecified counts and remain
 * available through the existing all-runs capture. They are never assigned
 * to a named run. Counts describe records, not tests or successful executions.
 *
 * Call on the owning thread with both registries alive. The bounded registry
 * capacities limit allocation. No file, callback or process operation occurs.
 * On failure *out_runs is NULL. Immutable reads remain valid after registry
 * destruction; validation and capture require the original live registries.
 */
UmiStatus UmiTestEvidenceRunsCreate(const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOutputRegistry *output, const char *item_id,
    UmiTestEvidenceRuns **out_runs);
void UmiTestEvidenceRunsDestroy(UmiTestEvidenceRuns *runs);
UmiStatus UmiTestEvidenceRunsRead(const UmiTestEvidenceRuns *runs,
    UmiTestEvidenceRunsSummary *out_summary);
UmiStatus UmiTestEvidenceRunsAt(const UmiTestEvidenceRuns *runs, size_t index,
    UmiTestEvidenceRun *out_run);
/* Reads leave output unchanged on failure. Validation checks identity as well
 * as revision, and does not lock either registry or authorize a test run. */
UmiStatus UmiTestEvidenceRunsValidate(const UmiTestEvidenceRuns *runs,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOutputRegistry *output);
/* Capture only while the catalogue is current. NULL/empty session selects all
 * runs, preserving UmiTestEvidenceCreate semantics. A nonempty session must
 * appear in this catalogue; unknown IDs return NOT_FOUND, never all records.
 * Named selection includes result-only and output-only runs. The original
 * evidence limits, outcomes, ordering and omission counts remain unchanged.
 * *out_evidence is NULL on failure. */
UmiStatus UmiTestEvidenceRunsCapture(const UmiTestEvidenceRuns *runs,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOutputRegistry *output, const char *session_id,
    UmiTestEvidence **out_evidence);

#ifdef __cplusplus
}
#endif
#endif
