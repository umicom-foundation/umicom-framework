/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/pnl.h
 *
 * PURPOSE:
 *   Calculate unrealised and total P&L for a position.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates pnl behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_PNL_H
#define INCLUDE_UMICOM_TRADING_PNL_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the position unrealised pnl operation used by this module and its client
 * applications.
 */
double umi_position_unrealised_pnl(const UmiPosition *position, double mark_price);
/**
 * Provide the position total pnl operation used by this module and its client
 * applications.
 */
double umi_position_total_pnl(const UmiPosition *position, double mark_price);
#ifdef __cplusplus
}
#endif
#endif
