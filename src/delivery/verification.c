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

/* BEGINNER NOTE:
 * Verification collects independent evidence before a release gate decides whether publishing is allowed.
 */

#include "umicom/delivery/verification.h"
#include "delivery_internal.h"
#include <string.h>
void umi_verification_report_init(UmiVerificationReport *report)
{
    if (report != NULL) (void)memset(report, 0, sizeof(*report));
}
UmiStatus umi_verification_report_add(UmiVerificationReport *report,
                                      const char *check_id,
                                      UmiEvidenceStatus status)
{
    UmiVerificationCheck *check;
    if (report == NULL || check_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (report->count >= UMI_DELIVERY_MAX_CHECKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    check = &report->checks[report->count++];
    (void)memset(check, 0, sizeof(*check));
    check->status = status;
    return umi_delivery_copy_text(check->check_id, sizeof(check->check_id), check_id);
}
int umi_verification_report_passed(const UmiVerificationReport *report)
{
    size_t i;
    if (report == NULL || report->count == 0U) return 0;
    for (i = 0U; i < report->count; ++i) {
        if (report->checks[i].status != UMI_EVIDENCE_PASS) return 0;
    }
    return 1;
}
