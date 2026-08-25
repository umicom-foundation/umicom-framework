/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/portfolio_aggregation.h
 *
 * PURPOSE:
 *   Aggregate net and gross market value from portfolio snapshots.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_AGGREGATION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_AGGREGATION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/portfolio_snapshot.h"
typedef struct UmiEnterprisePortfolioAggregation { double net_market_value; double gross_market_value; size_t position_count; } UmiEnterprisePortfolioAggregation;
/* Aggregate finite portfolio market values into net and gross totals. */
UmiStatus umi_enterprise_portfolio_aggregate(const UmiEnterprisePortfolioSnapshot *snapshot,UmiEnterprisePortfolioAggregation *out_aggregation);

#ifdef __cplusplus
}
#endif

#endif
