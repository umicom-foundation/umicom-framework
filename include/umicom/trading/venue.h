/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/venue.h
 *
 * PURPOSE:
 *   Validate venue identifiers used by routing and market-data services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates venue behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_VENUE_H
#define INCLUDE_UMICOM_TRADING_VENUE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_trading_venue_valid(const char *venue);
#ifdef __cplusplus
}
#endif
#endif
