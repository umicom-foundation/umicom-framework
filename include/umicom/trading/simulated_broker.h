/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/simulated_broker.h
 *
 * PURPOSE:
 *   Provide a deterministic paper/simulation broker implementing the public broker contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates simulated broker behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_SIMULATED_BROKER_H
#define INCLUDE_UMICOM_TRADING_SIMULATED_BROKER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#include "umicom/trading/broker.h"
#include "umicom/trading/order_store.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the simulated broker storage data shared with callers of this public contract.
 */
typedef struct UmiSimulatedBrokerStorage {
    UmiTradingEnvironment environment;
    UmiOrderStore orders;
    int connected;
} UmiSimulatedBrokerStorage;

/**
 * Initialise simulated broker from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_simulated_broker_create(UmiSimulatedBrokerStorage *storage,
                                      UmiBroker *out_broker);
#ifdef __cplusplus
}
#endif
#endif
