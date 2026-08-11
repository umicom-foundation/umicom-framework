/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/instrument.h
 *
 * PURPOSE:
 *   Validate canonical tradeable instrument records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
int umi_instrument_valid(const UmiInstrument *instrument);
int umi_instrument_same(const UmiInstrument *left, const UmiInstrument *right);
#ifdef __cplusplus
}
#endif
#endif
