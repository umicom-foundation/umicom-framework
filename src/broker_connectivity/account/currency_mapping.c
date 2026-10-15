/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/account/currency_mapping.c
 *
 * PURPOSE:
 *   Apply Account Currency Mapping without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/account/currency_mapping.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_account_currency_mapping(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "account.currency-mapping",
        "Account Currency Mapping",
        UMI_BROKER_POLICY_COVERAGE,
        100.000000,
        90.000000,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_account_currency_mapping_batch, umi_broker_connectivity_evaluate_account_currency_mapping,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
