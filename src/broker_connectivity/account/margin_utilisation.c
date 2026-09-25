/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/account/margin_utilisation.c
 *
 * PURPOSE:
 *   Apply Margin Utilisation without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/account/margin_utilisation.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_account_margin_utilisation(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "account.margin-utilisation",
        "Margin Utilisation",
        UMI_BROKER_POLICY_UTILISATION,
        0.000000,
        85.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}
