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
#include "../../base/policy_batch_internal.h"
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_account_daily_pnl_batch, umi_broker_connectivity_evaluate_account_daily_pnl,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
