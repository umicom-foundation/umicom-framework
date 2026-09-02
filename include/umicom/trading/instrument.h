/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/instrument.h
 *
 * PURPOSE:
 *   Validate canonical tradeable instrument records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates instrument behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_INSTRUMENT_H
#define INCLUDE_UMICOM_TRADING_INSTRUMENT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Check that instrument satisfies its contract before another service relies on it.
 */
int umi_instrument_valid(const UmiInstrument *instrument);
/**
 * Provide the instrument same operation used by this module and its client applications.
 */
int umi_instrument_same(const UmiInstrument *left, const UmiInstrument *right);
#ifdef __cplusplus
}
#endif
#endif
