/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/evidence.h
 * PURPOSE: Capture selected-test results and output without borrowing live registries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_PLATFORM_EVIDENCE_H
#define UMICOM_TEST_PLATFORM_EVIDENCE_H
#include "umicom/test_platform/result.h"
#include "umicom/test_platform/output.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_EVIDENCE_RESULT_LIMIT 32U
#define UMI_TEST_EVIDENCE_OUTPUT_LIMIT 64U
typedef struct UmiTestEvidence UmiTestEvidence;
typedef struct UmiTestEvidenceSummary {
    char item_id[128];
    char session_id[128];
    uint64_t result_revision;
    uint64_t output_revision;
    size_t matching_results;
    size_t retained_results;
    size_t matching_output;
    size_t retained_output;
} UmiTestEvidenceSummary;

/* Capture one exact, nonempty item ID. NULL/empty session_id includes all
 * sessions; otherwise session identity must also match exactly. IDs must fit
 * 128 bytes including the terminator. Newest results are ordered by descending
 * sequence, then revision; output by descending timestamp, then revision.
 * Ties use ascending record ID. Limits retain the newest matching records,
 * never the newest unrelated records. Summary totals disclose omitted records.
 * Access both registries on their owning thread. No process, callback or file
 * operation occurs. The returned immutable copy survives registry changes and
 * destruction. *out_evidence is NULL on failure; callers destroy successful
 * captures. Counts are evidence, not a claim that a test ran or passed. */
UmiStatus UmiTestEvidenceCreate(const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOutputRegistry *output, const char *item_id,
    const char *session_id, UmiTestEvidence **out_evidence);
void UmiTestEvidenceDestroy(UmiTestEvidence *evidence);
UmiStatus UmiTestEvidenceGetSummary(const UmiTestEvidence *evidence,
    UmiTestEvidenceSummary *out_summary);
/* Zero-based newest-first reads. Outputs remain unchanged on failure. */
UmiStatus UmiTestEvidenceResultAt(const UmiTestEvidence *evidence, size_t index,
    UmiTestPlatformResultSnapshot *out_result);
UmiStatus UmiTestEvidenceOutputAt(const UmiTestEvidence *evidence, size_t index,
    UmiTestPlatformOutputSnapshot *out_output);
#ifdef __cplusplus
}
#endif
#endif
