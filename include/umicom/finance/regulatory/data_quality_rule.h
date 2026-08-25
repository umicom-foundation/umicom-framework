/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_quality_rule.h
 *
 * PURPOSE:
 *   Define a governed data-quality rule and acceptance threshold.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_QUALITY_RULE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_QUALITY_RULE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataQualityRule {
    char rule_id[UMI_REG_ID_CAPACITY];
    char domain_id[UMI_REG_ID_CAPACITY];
    char dimension[UMI_REG_NAME_CAPACITY];
    double threshold;
} UmiDataQualityRule;

/* Initialise one validated define a governed data-quality rule and acceptance threshold. record. */
UmiStatus umi_reg_data_quality_rule_init(UmiDataQualityRule *record, const char *rule_id, const char *domain_id, const char *dimension, double threshold);

#ifdef __cplusplus
}
#endif

#endif
