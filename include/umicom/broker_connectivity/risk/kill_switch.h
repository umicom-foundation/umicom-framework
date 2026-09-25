/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/risk/kill_switch.h
 *
 * PURPOSE:
 *   Evaluate Kill Switch Gate through the provider-neutral broker connectivity layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_RISK_KILL_SWITCH_H
#define UMICOM_BROKER_CONNECTIVITY_RISK_KILL_SWITCH_H

#include "umicom/broker_connectivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_broker_connectivity_evaluate_risk_kill_switch(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
