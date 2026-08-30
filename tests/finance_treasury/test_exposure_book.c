/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_exposure_book.c
 *
 * PURPOSE:
 *   Verify bounded exposure book registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/exposure_book.h"
int main(void) { UmiTreasuryExposureBook r; UmiTreasuryExposure item; umi_treasury_exposure_book_init(&r);
    if (umi_treasury_exposure_init(&item, "exp", 1000, 500) != UMI_STATUS_OK) return 1;
    if (umi_treasury_exposure_book_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_exposure_book_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_exposure_book_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
