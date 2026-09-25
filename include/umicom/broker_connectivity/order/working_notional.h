/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/order/working_notional.h
 *
 * PURPOSE:
 *   Evaluate Working Order Notional through the provider-neutral broker connectivity layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_ORDER_WORKING_NOTIONAL_H
#define UMICOM_BROKER_CONNECTIVITY_ORDER_WORKING_NOTIONAL_H

#include "umicom/broker_connectivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_broker_connectivity_evaluate_order_working_notional(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
