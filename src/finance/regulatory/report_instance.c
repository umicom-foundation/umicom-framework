/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/report_instance.c
 *
 * PURPOSE:
 *   Maintain bounded facts and lifecycle state for one regulatory report instance.
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

#include "umicom/finance/regulatory/report_instance.h"

#include <string.h>
/*
 * Initialise reg report instance from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_reg_report_instance_init(UmiReportInstance *report, const char *report_id, const char *template_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(report, 0, sizeof *report);
    status = umi_reg_copy_text(report->report_id, sizeof report->report_id, report_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(report->template_id, sizeof report->template_id, template_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) report->state = UMI_REG_STATE_DRAFT;
    return status;
}
/*
 * Provide the reg report instance record fact operation used by this module and its client
 * applications.
 */
UmiStatus umi_reg_report_instance_record_fact(UmiReportInstance *report, int mandatory)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || (mandatory != 0 && mandatory != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->fact_count >= UMI_REG_MAX_FIELDS) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++report->fact_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (mandatory != 0) ++report->mandatory_fact_count;
    return UMI_STATUS_OK;
}
/*
 * Provide the reg report instance complete operation used by this module and its client
 * applications.
 */
int umi_reg_report_instance_complete(const UmiReportInstance *report, size_t required_mandatory_facts)
{
    return report != NULL && report->mandatory_fact_count >= required_mandatory_facts ? 1 : 0;
}
