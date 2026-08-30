/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_value.c
 *
 * PURPOSE:
 *   Represent one typed numeric fact with source evidence.
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

#include "umicom/finance/regulatory/reporting_value.h"

#include <string.h>

UmiStatus umi_reg_reporting_value_init(UmiReportingValue *record, const char *field_id, const char *source_id, double value, int64_t as_of_ms)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(umi_reg_number_valid(value) && as_of_ms >= 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->field_id, sizeof record->field_id, field_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->source_id, sizeof record->source_id, source_id);
    if (status != UMI_STATUS_OK) return status;
    record->value = value;
    record->as_of_ms = as_of_ms;
    return UMI_STATUS_OK;
}
