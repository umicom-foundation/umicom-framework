/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_cash_position_book.c
 *
 * PURPOSE:
 *   Verify bounded cash position book registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_position_book.h"
int main(void) { UmiTreasuryCashPositionBook r; UmiTreasuryCashPosition item; umi_treasury_cash_position_book_init(&r);
    if (umi_treasury_cash_position_init(&item, "pos-1", "EUR", 100, 125) != UMI_STATUS_OK) return 1;
    if (umi_treasury_cash_position_book_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_cash_position_book_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_cash_position_book_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
