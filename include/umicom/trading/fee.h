/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/fee.h
 *
 * PURPOSE:
 *   Calculate simple quantity-based execution fees.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * Provide the execution fee operation used by this module and its client applications.
 */
double umi_execution_fee(double quantity, double per_unit_fee, double minimum_fee);
#ifdef __cplusplus
}
#endif
#endif
