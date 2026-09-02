/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_job.c
 *
 * PURPOSE:
 *   Represent one scheduled regulatory reporting work item.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/reporting_job.h"

#include <string.h>

/*
 * Initialise reg reporting job from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_reg_reporting_job_init(UmiReportingJob *record, const char *job_id, const char *report_id, uint32_t priority, int64_t due_ms)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(priority > 0U && due_ms >= 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->job_id, sizeof record->job_id, job_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->report_id, sizeof record->report_id, report_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->priority = priority;
    record->due_ms = due_ms;
    return UMI_STATUS_OK;
}
