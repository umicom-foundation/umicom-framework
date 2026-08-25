/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_freshness.c
 *
 * PURPOSE:
 *   Evaluate market-data staleness against explicit enterprise policy.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/market_data_freshness.h"

UmiStatus umi_enterprise_market_data_freshness_policy_init(UmiEnterpriseMarketDataFreshnessPolicy *policy, int64_t max_age_ms){ if(policy==NULL||max_age_ms<0)return UMI_STATUS_INVALID_ARGUMENT; policy->max_age_ms=max_age_ms; return UMI_STATUS_OK; }
int umi_enterprise_market_data_is_fresh(const UmiEnterpriseMarketDataFreshnessPolicy *policy,int64_t event_ms,int64_t valuation_ms){ if(policy==NULL||event_ms<0||valuation_ms<event_ms)return 0; return (valuation_ms-event_ms)<=policy->max_age_ms ? 1:0; }
