/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/pretrade_guard.h
 *
 * PURPOSE:
 *   Apply provider-neutral quantity, notional, position, daily-loss, buying
 *   power, restricted-instrument and live-approval gates before broker submit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_PRETRADE_GUARD_H
#define UMICOM_BROKER_CONNECTIVITY_PRETRADE_GUARD_H

#include "umicom/base/status.h"
#include "umicom/trading/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBrokerPretradeContext {
    UmiRiskLimit limits;
    double referencePrice;
    double currentPositionQuantity;
    double buyingPower;
    double dailyPnl;
    int restrictedInstrument;
    int killSwitchEngaged;
    int trusted;
    int liveApproved;
} UmiBrokerPretradeContext;

UmiStatus umi_broker_pretrade_guard_evaluate(
    const UmiOrderRequest *request,
    const UmiBrokerPretradeContext *context,
    UmiRiskDecision *outDecision);

#ifdef __cplusplus
}
#endif
#endif
