/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/regulatory_limit.h
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_LIMIT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_LIMIT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the regulatory limit data shared with callers of this public contract.
 */
typedef struct UmiRegulatoryLimit {
    char limit_id[UMI_REG_ID_CAPACITY];
    char metric[UMI_REG_NAME_CAPACITY];
    double threshold;
    int minimum_rule;
} UmiRegulatoryLimit;

/* Initialise one validated define a minimum or maximum regulatory threshold. record. */
UmiStatus umi_reg_regulatory_limit_init(UmiRegulatoryLimit *record, const char *limit_id, const char *metric, double threshold, int minimum_rule);

#ifdef __cplusplus
}
#endif

#endif
