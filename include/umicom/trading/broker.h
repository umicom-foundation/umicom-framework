/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/broker.h
 *
 * PURPOSE:
 *   Define the stable C ABI contract used by simulated and vendor broker adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates broker behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_BROKER_H
#define INCLUDE_UMICOM_TRADING_BROKER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the broker data shared with callers of this public contract.
 */
typedef struct UmiBroker {
    void *instance;
    const char *provider_name;
    UmiStatus (*connect)(void *instance, UmiTradingEnvironment environment);
    UmiStatus (*submit_order)(void *instance, const UmiOrderRequest *request);
    UmiStatus (*cancel_order)(void *instance, const UmiFinancialId *client_order_id);
    void (*destroy)(void *instance);
} UmiBroker;
/**
 * Check that broker contract satisfies its contract before another service relies on it.
 */
int umi_broker_contract_valid(const UmiBroker *broker);
#ifdef __cplusplus
}
#endif
#endif
