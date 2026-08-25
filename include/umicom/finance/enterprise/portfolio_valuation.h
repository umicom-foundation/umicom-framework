/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/portfolio_valuation.h
 *
 * PURPOSE:
 *   Store portfolio-level PV, previous PV and derived P&L for one valuation cut.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_VALUATION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_VALUATION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterprisePortfolioValuation { char portfolio_id[UMI_ENTERPRISE_ID_CAPACITY]; int64_t as_of_ms; double present_value; double previous_present_value; double pnl; } UmiEnterprisePortfolioValuation;
/* Initialise portfolio valuation evidence and derive period P&L. */
UmiStatus umi_enterprise_portfolio_valuation_init(UmiEnterprisePortfolioValuation *valuation,const char *portfolio_id,int64_t as_of_ms,double present_value,double previous_present_value);

#ifdef __cplusplus
}
#endif

#endif
