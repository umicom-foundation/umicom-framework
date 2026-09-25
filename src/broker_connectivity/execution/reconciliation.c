/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/execution/reconciliation.c
 *
 * PURPOSE:
 *   Apply Execution Reconciliation without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/execution/reconciliation.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_execution_reconciliation(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "execution.reconciliation",
        "Execution Reconciliation",
        UMI_BROKER_POLICY_RECONCILIATION,
        100.000000,
        95.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}
