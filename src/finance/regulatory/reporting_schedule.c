/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_schedule.c
 *
 * PURPOSE:
 *   Define recurring due-date metadata for a regulatory report family.
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

#include "umicom/finance/regulatory/reporting_schedule.h"

#include <string.h>

UmiStatus umi_reg_reporting_schedule_init(UmiReportingSchedule *record, const char *schedule_id, const char *template_id, uint32_t frequency_days, int32_t next_due_day)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(frequency_days > 0U && next_due_day >= 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->schedule_id, sizeof record->schedule_id, schedule_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->template_id, sizeof record->template_id, template_id);
    if (status != UMI_STATUS_OK) return status;
    record->frequency_days = frequency_days;
    record->next_due_day = next_due_day;
    return UMI_STATUS_OK;
}
