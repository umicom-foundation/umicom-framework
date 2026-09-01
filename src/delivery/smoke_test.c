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

void umi_smoke_report_init(UmiSmokeReport *report)
{
    if (report != NULL) (void)memset(report, 0, sizeof(*report));
}

UmiStatus umi_smoke_report_add(UmiSmokeReport *report,
                               const char *check_id,
                               UmiEvidenceStatus status,
                               const char *message)
{
    UmiSmokeCheck *check;
    if (report == NULL || check_id == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (report->count >= UMI_DELIVERY_MAX_CHECKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    check = &report->checks[report->count];
    (void)memset(check, 0, sizeof(*check));
    if (umi_delivery_copy_text(check->check_id, sizeof(check->check_id), check_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(check->message, sizeof(check->message), message) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    check->status = status;
    ++report->count;
    return UMI_STATUS_OK;
}

int umi_smoke_report_passed(const UmiSmokeReport *report)
{
    size_t i;
    if (report == NULL || report->count == 0U) return 0;
    for (i = 0U; i < report->count; ++i) {
        if (report->checks[i].status != UMI_EVIDENCE_PASS) return 0;
    }
    return 1;
}
