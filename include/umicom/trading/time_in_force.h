/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/time_in_force.h
 *
 * PURPOSE:
 *   Validate time-in-force values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates time in force behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_TIME_IN_FORCE_H
#define INCLUDE_UMICOM_TRADING_TIME_IN_FORCE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Check that time in force satisfies its contract before another service relies on it.
 */
int umi_time_in_force_valid(UmiTimeInForce tif);
/* Capability of the local retained-order model, separate from enum validity.
 * DAY/GTC may be retained; the caller still owns session-end cancellation.
 * No automatic calendar expiry or venue matching is supplied. IOC/FOK return
 * NOT_IMPLEMENTED because their immediate execution guarantees are absent.
 * Unknown values return INVALID_ARGUMENT. This function has no side effects
 * and makes no claim about an external broker's supported policies. */
UmiStatus UmiTimeInForceCheckLocalRetention(UmiTimeInForce tif);
#ifdef __cplusplus
}
#endif
#endif
