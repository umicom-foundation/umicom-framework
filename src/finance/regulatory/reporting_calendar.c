/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_calendar.c
 *
 * PURPOSE:
 *   Define due-date policy for a regulatory reporting cycle.
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

#include "umicom/finance/regulatory/reporting_calendar.h"

#include <string.h>

UmiStatus umi_reg_reporting_calendar_init(UmiReportingCalendar *record, const char *calendar_id, const char *authority_id, uint32_t frequency_days, uint32_t grace_days)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(frequency_days > 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->calendar_id, sizeof record->calendar_id, calendar_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->authority_id, sizeof record->authority_id, authority_id);
    if (status != UMI_STATUS_OK) return status;
    record->frequency_days = frequency_days;
    record->grace_days = grace_days;
    return UMI_STATUS_OK;
}
