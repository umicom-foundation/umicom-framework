/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/execution/audit.c
 *
 * PURPOSE:
 *   Apply Execution Audit Coverage without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/execution/audit.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_execution_audit(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "execution.audit",
        "Execution Audit Coverage",
        UMI_BROKER_POLICY_COVERAGE,
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
