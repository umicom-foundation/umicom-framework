/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/order/working_notional.c
 *
 * PURPOSE:
 *   Apply Working Order Notional without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/order/working_notional.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_order_working_notional(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "order.working-notional",
        "Working Order Notional",
        UMI_BROKER_POLICY_VALUE,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_order_working_notional_batch, umi_broker_connectivity_evaluate_order_working_notional,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
