/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/expected_shortfall.h
 *
 * PURPOSE:
 *   Calculate deterministic historical expected shortfall beyond empirical VaR.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_EXPECTED_SHORTFALL_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_EXPECTED_SHORTFALL_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Calculate average historical loss at or beyond the requested confidence quantile. */
UmiStatus umi_enterprise_expected_shortfall_historical(const double *losses,size_t count,double confidence,double *out_es);

#ifdef __cplusplus
}
#endif

#endif
