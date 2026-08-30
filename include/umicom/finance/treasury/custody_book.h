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
typedef struct UmiTreasuryCustodyBook {
    UmiTreasuryCustodyPosition items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryCustodyBook;
void umi_treasury_custody_book_init(UmiTreasuryCustodyBook *registry);
UmiStatus umi_treasury_custody_book_add(UmiTreasuryCustodyBook *registry, const UmiTreasuryCustodyPosition *item);
const UmiTreasuryCustodyPosition *umi_treasury_custody_book_find(const UmiTreasuryCustodyBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
