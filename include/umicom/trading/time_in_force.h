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

/* BEGINNER NOTE:
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
int umi_time_in_force_valid(UmiTimeInForce tif);
#ifdef __cplusplus
}
#endif
#endif
