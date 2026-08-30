/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_confirmation_matching.c
 *
 * PURPOSE:
 *   Exercise confirmation matching validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/confirmation_matching.h"
int main(void) {
    UmiTreasuryConfirmationMatching v;
    if (umi_treasury_confirmation_matching_init(&v, "match", 8U, 8U) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_confirmation_matching_exact(&v))return 2;
    return 0;
}
