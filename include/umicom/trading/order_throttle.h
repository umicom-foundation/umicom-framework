/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_throttle.h
 *
 * PURPOSE:
 *   Apply a simple bounded order-rate limit per time window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates order throttle behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ORDER_THROTTLE_H
#define INCLUDE_UMICOM_TRADING_ORDER_THROTTLE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the order throttle data shared with callers of this public contract.
 */
typedef struct UmiOrderThrottle { uint32_t count; uint32_t max_count; int64_t window_start_ms; int64_t window_ms; } UmiOrderThrottle;
/**
 * Initialise order throttle from caller-provided values so later operations receive a
 * known state.
 */
void umi_order_throttle_init(UmiOrderThrottle *throttle, uint32_t max_count, int64_t window_ms);
/**
 * Provide the order throttle accept operation used by this module and its client
 * applications.
 */
int umi_order_throttle_accept(UmiOrderThrottle *throttle, int64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
