/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_subscription.h
 *
 * PURPOSE:
 *   Represent lifecycle state for reusable market-data subscriptions.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_SUBSCRIPTION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_SUBSCRIPTION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise market data subscription data shared with callers of this
 * public contract.
 */
typedef struct UmiEnterpriseMarketDataSubscription {
    char subscription_id[UMI_ENTERPRISE_ID_CAPACITY];
    char topic[UMI_ENTERPRISE_ID_CAPACITY];
    int active;
} UmiEnterpriseMarketDataSubscription;

/* Initialise a dormant subscription for one canonical market-data topic. */
UmiStatus umi_enterprise_market_data_subscription_init(UmiEnterpriseMarketDataSubscription *subscription, const char *subscription_id, const char *topic);
/* Toggle delivery state without changing subscription identity. */
UmiStatus umi_enterprise_market_data_subscription_set_active(UmiEnterpriseMarketDataSubscription *subscription, int active);

#ifdef __cplusplus
}
#endif

#endif
