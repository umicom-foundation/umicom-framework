/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/execution/fill_progress.c
 *
 * PURPOSE:
 *   Apply Execution Fill Progress without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/execution/fill_progress.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_execution_fill_progress(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "execution.fill-progress",
        "Execution Fill Progress",
        UMI_BROKER_POLICY_PROGRESS,
        100.000000,
        80.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}
