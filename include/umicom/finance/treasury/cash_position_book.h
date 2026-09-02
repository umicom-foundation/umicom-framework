/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_position_book.h
 *
 * PURPOSE:
 *   Maintain bounded cash positions with deterministic identifier lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_POSITION_BOOK_H
#define UMICOM_FINANCE_TREASURY_CASH_POSITION_BOOK_H
#include "umicom/finance/treasury/cash_position.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury cash position book data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCashPositionBook {
    UmiTreasuryCashPosition items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryCashPositionBook;
/**
 * Initialise treasury cash position book from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_cash_position_book_init(UmiTreasuryCashPositionBook *registry);
/**
 * Add treasury cash position book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_cash_position_book_add(UmiTreasuryCashPositionBook *registry, const UmiTreasuryCashPosition *item);
/**
 * Find treasury cash position book while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiTreasuryCashPosition *umi_treasury_cash_position_book_find(const UmiTreasuryCashPositionBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
