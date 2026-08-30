/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/factor_attribution.h
 *
 * PURPOSE:
 *   Calculate simple signed factor contribution from change and weight.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates factor attribution behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_FACTOR_ATTRIBUTION_H
#define INCLUDE_UMICOM_TRADING_FACTOR_ATTRIBUTION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_factor_contribution(double factor_change, double weight);
#ifdef __cplusplus
}
#endif
#endif
