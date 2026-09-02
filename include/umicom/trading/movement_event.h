/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/movement_event.h
 *
 * PURPOSE:
 *   Detect price movements that exceed a configured point threshold within a time window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates movement event behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MOVEMENT_EVENT_H
#define INCLUDE_UMICOM_TRADING_MOVEMENT_EVENT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the movement event measure operation used by this module and its client
 * applications.
 */
UmiMovementEvent umi_movement_event_measure(double start_price, double end_price, int64_t start_ms, int64_t end_ms, double minimum_points, int64_t maximum_duration_ms);
#ifdef __cplusplus
}
#endif
#endif
