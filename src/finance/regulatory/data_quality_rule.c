/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_quality_rule.c
 *
 * PURPOSE:
 *   Define a governed data-quality rule and acceptance threshold.
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

#include "umicom/finance/regulatory/data_quality_rule.h"

#include <string.h>

UmiStatus umi_reg_data_quality_rule_init(UmiDataQualityRule *record, const char *rule_id, const char *domain_id, const char *dimension, double threshold)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(umi_reg_number_valid(threshold) && threshold >= 0.0 && threshold <= 1.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->rule_id, sizeof record->rule_id, rule_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->domain_id, sizeof record->domain_id, domain_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->dimension, sizeof record->dimension, dimension);
    if (status != UMI_STATUS_OK) return status;
    record->threshold = threshold;
    return UMI_STATUS_OK;
}
