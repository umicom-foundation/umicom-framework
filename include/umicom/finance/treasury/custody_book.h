/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/custody_book.h
 *
 * PURPOSE:
 *   Maintain custody positions by security/account identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CUSTODY_BOOK_H
#define UMICOM_FINANCE_TREASURY_CUSTODY_BOOK_H
#include "umicom/finance/treasury/custody_position.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury custody book data shared with callers of this public contract.
 */
typedef struct UmiTreasuryCustodyBook {
    UmiTreasuryCustodyPosition items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryCustodyBook;
/**
 * Initialise treasury custody book from caller-provided values so later operations receive
 * a known state.
 */
void umi_treasury_custody_book_init(UmiTreasuryCustodyBook *registry);
/**
 * Add treasury custody book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_custody_book_add(UmiTreasuryCustodyBook *registry, const UmiTreasuryCustodyPosition *item);
/**
 * Find treasury custody book while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiTreasuryCustodyPosition *umi_treasury_custody_book_find(const UmiTreasuryCustodyBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
