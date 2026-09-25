/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/account/net_liquidation.h
 *
 * PURPOSE:
 *   Evaluate Net Liquidation through the provider-neutral broker connectivity layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_ACCOUNT_NET_LIQUIDATION_H
#define UMICOM_BROKER_CONNECTIVITY_ACCOUNT_NET_LIQUIDATION_H

#include "umicom/broker_connectivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_broker_connectivity_evaluate_account_net_liquidation(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
