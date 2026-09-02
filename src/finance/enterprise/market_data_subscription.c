/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_subscription.c
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

#include "umicom/finance/enterprise/market_data_subscription.h"

#include <string.h>

/*
 * Initialise enterprise market data subscription from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_enterprise_market_data_subscription_init(UmiEnterpriseMarketDataSubscription *subscription, const char *subscription_id, const char *topic)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (subscription == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(subscription, 0, sizeof *subscription);
    status = umi_quant_copy_text(subscription->subscription_id, sizeof subscription->subscription_id, subscription_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_quant_copy_text(subscription->topic, sizeof subscription->topic, topic);
}

/*
 * Provide the enterprise market data subscription set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_enterprise_market_data_subscription_set_active(UmiEnterpriseMarketDataSubscription *subscription, int active)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (subscription == NULL || (active != 0 && active != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    subscription->active = active;
    return UMI_STATUS_OK;
}
