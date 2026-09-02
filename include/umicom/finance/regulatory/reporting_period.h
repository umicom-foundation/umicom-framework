/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_period.h
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_PERIOD_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_PERIOD_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the reporting period data shared with callers of this public contract.
 */
typedef struct UmiReportingPeriod {
    char period_id[UMI_REG_ID_CAPACITY];
    int32_t start_day;
    int32_t end_day;
    int year_end;
} UmiReportingPeriod;

/* Initialise one validated represent the inclusive reporting period for one submission. record. */
UmiStatus umi_reg_reporting_period_init(UmiReportingPeriod *record, const char *period_id, int32_t start_day, int32_t end_day, int year_end);

#ifdef __cplusplus
}
#endif

#endif
