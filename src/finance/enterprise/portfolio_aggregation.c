/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/portfolio_aggregation.c
 *
 * PURPOSE:
 *   Aggregate net and gross market value from portfolio snapshots.
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

#include "umicom/finance/enterprise/portfolio_aggregation.h"

#include <math.h>
UmiStatus umi_enterprise_portfolio_aggregate(const UmiEnterprisePortfolioSnapshot *s,UmiEnterprisePortfolioAggregation *out){ size_t i; if(s==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT; out->net_market_value=0.0;out->gross_market_value=0.0;out->position_count=s->count; for(i=0U;i<s->count;++i){out->net_market_value+=s->positions[i].market_value;out->gross_market_value+=fabs(s->positions[i].market_value);} return UMI_STATUS_OK; }
