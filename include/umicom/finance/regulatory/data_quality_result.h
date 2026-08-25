/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_quality_result.h
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_QUALITY_RESULT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_QUALITY_RESULT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataQualityResult {
    char rule_id[UMI_REG_ID_CAPACITY];
    double score;
    uint64_t checked_records;
    uint64_t failed_records;
} UmiDataQualityResult;

/* Initialise one validated capture observed quality evidence for one governed rule. record. */
UmiStatus umi_reg_data_quality_result_init(UmiDataQualityResult *record, const char *rule_id, double score, uint64_t checked_records, uint64_t failed_records);

#ifdef __cplusplus
}
#endif

#endif
