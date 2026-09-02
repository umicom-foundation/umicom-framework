/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/verification.c
 *
 * PURPOSE:
 *   Aggregate named verification checks such as checksum, signature, SBOM and smoke tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Verification collects independent evidence before a release gate decides whether publishing is allowed.
 */

#include "umicom/delivery/verification.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise verification report from caller-provided values so later operations receive a
 * known state.
 */
void umi_verification_report_init(UmiVerificationReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report != NULL) (void)memset(report, 0, sizeof(*report));
}
/* Add verification report only after its inputs and available capacity have been checked. */
UmiStatus umi_verification_report_add(UmiVerificationReport *report,
                                      const char *check_id,
                                      UmiEvidenceStatus status)
{
    UmiVerificationCheck *check;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || check_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->count >= UMI_DELIVERY_MAX_CHECKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    check = &report->checks[report->count++];
    (void)memset(check, 0, sizeof(*check));
    check->status = status;
    return umi_delivery_copy_text(check->check_id, sizeof(check->check_id), check_id);
}
/*
 * Provide the verification report passed operation used by this module and its client
 * applications.
 */
int umi_verification_report_passed(const UmiVerificationReport *report)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || report->count == 0U) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < report->count; ++i) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (report->checks[i].status != UMI_EVIDENCE_PASS) return 0;
    }
    return 1;
}
