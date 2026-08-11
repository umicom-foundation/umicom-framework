/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/risk_context.h
 *
 * PURPOSE:
 *   Calculate post-order projected position quantity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates risk context behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_RISK_CONTEXT_H
#define INCLUDE_UMICOM_TRADING_RISK_CONTEXT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_risk_projected_position(double current_position, const UmiOrderRequest *request);
#ifdef __cplusplus
}
#endif
#endif
