/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/regulatory_limit.c
 *
 * PURPOSE:
 *   Define a minimum or maximum regulatory threshold.
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

#include "umicom/finance/regulatory/regulatory_limit.h"

#include <string.h>

UmiStatus umi_reg_regulatory_limit_init(UmiRegulatoryLimit *record, const char *limit_id, const char *metric, double threshold, int minimum_rule)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(umi_reg_number_valid(threshold) && (minimum_rule == 0 || minimum_rule == 1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->limit_id, sizeof record->limit_id, limit_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->metric, sizeof record->metric, metric);
    if (status != UMI_STATUS_OK) return status;
    record->threshold = threshold;
    record->minimum_rule = minimum_rule;
    return UMI_STATUS_OK;
}
