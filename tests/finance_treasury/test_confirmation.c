/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_confirmation.c
 *
 * PURPOSE:
 *   Exercise confirmation validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/confirmation.h"
int main(void) {
    UmiTreasuryConfirmation v;
    if (umi_treasury_confirmation_init(&v, "conf", "trade-1", true, true) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_confirmation_complete(&v))return 2;
    return 0;
}
