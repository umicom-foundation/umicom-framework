/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/factor_observation.h
 *
 * PURPOSE:
 *   Calculate the change of an observed market factor around a movement event.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates factor observation behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_FACTOR_OBSERVATION_H
#define INCLUDE_UMICOM_TRADING_FACTOR_OBSERVATION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_factor_change(const UmiMarketFactor *factor);
#ifdef __cplusplus
}
#endif
#endif
