/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/provider/live_mode.h
 *
 * PURPOSE:
 *   Evaluate Live Trading Mode through the provider-neutral broker connectivity layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_PROVIDER_LIVE_MODE_H
#define UMICOM_BROKER_CONNECTIVITY_PROVIDER_LIVE_MODE_H

#include "umicom/broker_connectivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_broker_connectivity_evaluate_provider_live_mode(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
