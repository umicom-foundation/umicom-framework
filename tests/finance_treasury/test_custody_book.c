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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTreasuryCustodyBook r; UmiTreasuryCustodyPosition item; umi_treasury_custody_book_init(&r);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_custody_position_init(&item, "pos", 100, 10, 5) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_custody_book_add(&r,&item) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_treasury_custody_book_find(&r,item.id) == NULL) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_custody_book_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
