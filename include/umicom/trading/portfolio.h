/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/portfolio.h
 *
 * PURPOSE:
 *   Aggregate absolute gross position quantity across a position book.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates portfolio behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_PORTFOLIO_H
#define INCLUDE_UMICOM_TRADING_PORTFOLIO_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#include "umicom/trading/position_book.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_portfolio_gross_quantity(const UmiPositionBook *book);
#ifdef __cplusplus
}
#endif
#endif
