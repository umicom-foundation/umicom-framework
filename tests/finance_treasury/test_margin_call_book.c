/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_margin_call_book.c
 *
 * PURPOSE:
 *   Verify bounded margin call book registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_call_book.h"
int main(void) { UmiTreasuryMarginCallBook r; UmiTreasuryMarginCall item; umi_treasury_margin_call_book_init(&r);
    if (umi_treasury_margin_call_init(&item, "mc", 1000, 900, UMI_TREASURY_MARGIN_OPEN) != UMI_STATUS_OK) return 1;
    if (umi_treasury_margin_call_book_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_margin_call_book_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_margin_call_book_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
