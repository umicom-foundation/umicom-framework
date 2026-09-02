/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_freshness.h
 *
 * PURPOSE:
 *   Evaluate market-data staleness against explicit enterprise policy.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_FRESHNESS_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_FRESHNESS_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise market data freshness policy data shared with callers of this
 * public contract.
 */
typedef struct UmiEnterpriseMarketDataFreshnessPolicy { int64_t max_age_ms; } UmiEnterpriseMarketDataFreshnessPolicy;
/* Initialise a non-negative maximum acceptable market-data age. */
UmiStatus umi_enterprise_market_data_freshness_policy_init(UmiEnterpriseMarketDataFreshnessPolicy *policy, int64_t max_age_ms);
/* Return one when an event is fresh at the supplied valuation time. */
int umi_enterprise_market_data_is_fresh(const UmiEnterpriseMarketDataFreshnessPolicy *policy, int64_t event_ms, int64_t valuation_ms);

#ifdef __cplusplus
}
#endif

#endif
