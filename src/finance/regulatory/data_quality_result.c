/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_quality_result.c
 *
 * PURPOSE:
 *   Capture observed quality evidence for one governed rule.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/data_quality_result.h"

#include <string.h>

UmiStatus umi_reg_data_quality_result_init(UmiDataQualityResult *record, const char *rule_id, double score, uint64_t checked_records, uint64_t failed_records)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(umi_reg_number_valid(score) && score >= 0.0 && score <= 1.0 && failed_records <= checked_records)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->rule_id, sizeof record->rule_id, rule_id);
    if (status != UMI_STATUS_OK) return status;
    record->score = score;
    record->checked_records = checked_records;
    record->failed_records = failed_records;
    return UMI_STATUS_OK;
}
