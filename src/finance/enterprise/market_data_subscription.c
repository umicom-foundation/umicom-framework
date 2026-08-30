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

UmiStatus umi_enterprise_market_data_subscription_init(UmiEnterpriseMarketDataSubscription *subscription, const char *subscription_id, const char *topic)
{
    UmiStatus status;
    if (subscription == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(subscription, 0, sizeof *subscription);
    status = umi_quant_copy_text(subscription->subscription_id, sizeof subscription->subscription_id, subscription_id);
    if (status != UMI_STATUS_OK) return status;
    return umi_quant_copy_text(subscription->topic, sizeof subscription->topic, topic);
}

UmiStatus umi_enterprise_market_data_subscription_set_active(UmiEnterpriseMarketDataSubscription *subscription, int active)
{
    if (subscription == NULL || (active != 0 && active != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    subscription->active = active;
    return UMI_STATUS_OK;
}
