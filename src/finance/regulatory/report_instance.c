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
UmiStatus umi_reg_report_instance_init(UmiReportInstance *report, const char *report_id, const char *template_id)
{
    UmiStatus status;
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(report, 0, sizeof *report);
    status = umi_reg_copy_text(report->report_id, sizeof report->report_id, report_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(report->template_id, sizeof report->template_id, template_id);
    if (status == UMI_STATUS_OK) report->state = UMI_REG_STATE_DRAFT;
    return status;
}
UmiStatus umi_reg_report_instance_record_fact(UmiReportInstance *report, int mandatory)
{
    if (report == NULL || (mandatory != 0 && mandatory != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    if (report->fact_count >= UMI_REG_MAX_FIELDS) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++report->fact_count;
    if (mandatory != 0) ++report->mandatory_fact_count;
    return UMI_STATUS_OK;
}
int umi_reg_report_instance_complete(const UmiReportInstance *report, size_t required_mandatory_facts)
{
    return report != NULL && report->mandatory_fact_count >= required_mandatory_facts ? 1 : 0;
}
