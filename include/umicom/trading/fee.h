/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/fee.h
 *
 * PURPOSE:
 *   Calculate simple quantity-based execution fees.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates fee behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_FEE_H
#define INCLUDE_UMICOM_TRADING_FEE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_execution_fee(double quantity, double per_unit_fee, double minimum_fee);
#ifdef __cplusplus
}
#endif
#endif
