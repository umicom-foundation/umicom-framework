/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/position_book.h
 *
 * PURPOSE:
 *   Maintain a bounded collection of instrument positions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates position book behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_POSITION_BOOK_H
#define INCLUDE_UMICOM_TRADING_POSITION_BOOK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#include "umicom/trading/instrument.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the position book data shared with callers of this public contract.
 */
typedef struct UmiPositionBook { UmiPosition positions[UMI_TRADING_MAX_POSITIONS]; size_t count; } UmiPositionBook;
/**
 * Initialise position book from caller-provided values so later operations receive a known
 * state.
 */
void umi_position_book_init(UmiPositionBook *book);
/**
 * Provide the position book get operation used by this module and its client applications.
 */
UmiStatus umi_position_book_get(UmiPositionBook *book, const UmiInstrument *instrument, int create_if_missing, UmiPosition **out_position);
#ifdef __cplusplus
}
#endif
#endif
