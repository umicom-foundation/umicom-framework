/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_period.c
 *
 * PURPOSE:
 *   Represent the inclusive reporting period for one submission.
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

#include "umicom/finance/regulatory/reporting_period.h"

#include <string.h>

UmiStatus umi_reg_reporting_period_init(UmiReportingPeriod *record, const char *period_id, int32_t start_day, int32_t end_day, int year_end)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(start_day >= 0 && end_day >= start_day && (year_end == 0 || year_end == 1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->period_id, sizeof record->period_id, period_id);
    if (status != UMI_STATUS_OK) return status;
    record->start_day = start_day;
    record->end_day = end_day;
    record->year_end = year_end;
    return UMI_STATUS_OK;
}
