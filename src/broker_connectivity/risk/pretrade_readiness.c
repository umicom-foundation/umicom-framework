/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/risk/pretrade_readiness.c
 *
 * PURPOSE:
 *   Apply Pre-trade Readiness without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/risk/pretrade_readiness.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_risk_pretrade_readiness(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "risk.pretrade-readiness",
        "Pre-trade Readiness",
        UMI_BROKER_POLICY_HEALTH,
        90.000000,
        70.000000,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_risk_pretrade_readiness_batch, umi_broker_connectivity_evaluate_risk_pretrade_readiness,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
