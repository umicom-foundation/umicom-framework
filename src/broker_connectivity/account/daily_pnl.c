/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/account/daily_pnl.c
 *
 * PURPOSE:
 *   Apply Daily P&L without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/account/daily_pnl.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_account_daily_pnl(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "account.daily-pnl",
        "Daily P&L",
        UMI_BROKER_POLICY_PNL,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}
