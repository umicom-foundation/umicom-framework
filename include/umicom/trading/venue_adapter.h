/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/venue_adapter.h
 *
 * PURPOSE:
 *   Define a provider-neutral venue routing contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates venue adapter behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_VENUE_ADAPTER_H
#define INCLUDE_UMICOM_TRADING_VENUE_ADAPTER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVenueAdapter {
    void *instance;
    const char *venue_name;
    UmiStatus (*route_order)(void *instance, const UmiOrderRequest *request);
} UmiVenueAdapter;
int umi_venue_adapter_valid(const UmiVenueAdapter *adapter);
#ifdef __cplusplus
}
#endif
#endif
