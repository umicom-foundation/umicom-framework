/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/value_at_risk.h
 *
 * PURPOSE:
 *   Calculate deterministic historical value-at-risk from loss observations.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUE_AT_RISK_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUE_AT_RISK_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Calculate an empirical historical loss quantile for confidence in (0,1). */
UmiStatus umi_enterprise_value_at_risk_historical(const double *losses,size_t count,double confidence,double *out_var);

#ifdef __cplusplus
}
#endif

#endif
