/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_book.h
 *
 * PURPOSE:
 *   Calculate top-of-book liquidity and imbalance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates order book behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ORDER_BOOK_H
#define INCLUDE_UMICOM_TRADING_ORDER_BOOK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_order_book_top_liquidity(const UmiMarketDepth *depth);
double umi_order_book_imbalance(const UmiMarketDepth *depth);
#ifdef __cplusplus
}
#endif
#endif
