/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/smoke_test.c
 *
 * PURPOSE:
 *   Record post-install smoke checks used to prove a staged or installed application starts correctly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Smoke tests are deliberately small checks such as launching an executable or asking a health endpoint for a response.
 */

#include "umicom/delivery/smoke_test.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise smoke report from caller-provided values so later operations receive a known
 * state.
 */
void umi_smoke_report_init(UmiSmokeReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report != NULL) (void)memset(report, 0, sizeof(*report));
}

/* Add smoke report only after its inputs and available capacity have been checked. */
UmiStatus umi_smoke_report_add(UmiSmokeReport *report,
                               const char *check_id,
                               UmiEvidenceStatus status,
                               const char *message)
{
    UmiSmokeCheck *check;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || check_id == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->count >= UMI_DELIVERY_MAX_CHECKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    check = &report->checks[report->count];
    (void)memset(check, 0, sizeof(*check));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(check->check_id, sizeof(check->check_id), check_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(check->message, sizeof(check->message), message) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    check->status = status;
    ++report->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the smoke report passed operation used by this module and its client
 * applications.
 */
int umi_smoke_report_passed(const UmiSmokeReport *report)
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
