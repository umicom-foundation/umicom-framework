/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/quote.h
 *
 * PURPOSE:
 *   Calculate spread and midpoint from validated bid/ask quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates quote behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_QUOTE_H
#define INCLUDE_UMICOM_TRADING_QUOTE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_quote_valid(const UmiQuote *quote);
double umi_quote_mid(const UmiQuote *quote);
double umi_quote_spread(const UmiQuote *quote);
#ifdef __cplusplus
}
#endif
#endif
