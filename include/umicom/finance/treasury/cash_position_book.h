/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_position_book.h
 *
 * PURPOSE:
 *   Maintain bounded cash positions with deterministic identifier lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_POSITION_BOOK_H
#define UMICOM_FINANCE_TREASURY_CASH_POSITION_BOOK_H
#include "umicom/finance/treasury/cash_position.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCashPositionBook {
    UmiTreasuryCashPosition items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryCashPositionBook;
void umi_treasury_cash_position_book_init(UmiTreasuryCashPositionBook *registry);
UmiStatus umi_treasury_cash_position_book_add(UmiTreasuryCashPositionBook *registry, const UmiTreasuryCashPosition *item);
const UmiTreasuryCashPosition *umi_treasury_cash_position_book_find(const UmiTreasuryCashPositionBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
