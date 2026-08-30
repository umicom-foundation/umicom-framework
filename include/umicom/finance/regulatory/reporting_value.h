/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_value.h
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_VALUE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_VALUE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportingValue {
    char field_id[UMI_REG_ID_CAPACITY];
    char source_id[UMI_REG_ID_CAPACITY];
    double value;
    int64_t as_of_ms;
} UmiReportingValue;

/* Initialise one validated represent one typed numeric fact with source evidence. record. */
UmiStatus umi_reg_reporting_value_init(UmiReportingValue *record, const char *field_id, const char *source_id, double value, int64_t as_of_ms);

#ifdef __cplusplus
}
#endif

#endif
