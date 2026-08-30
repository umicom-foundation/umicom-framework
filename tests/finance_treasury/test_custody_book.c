/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_custody_book.c
 *
 * PURPOSE:
 *   Verify bounded custody book registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_book.h"
int main(void) { UmiTreasuryCustodyBook r; UmiTreasuryCustodyPosition item; umi_treasury_custody_book_init(&r);
    if (umi_treasury_custody_position_init(&item, "pos", 100, 10, 5) != UMI_STATUS_OK) return 1;
    if (umi_treasury_custody_book_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_custody_book_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_custody_book_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
