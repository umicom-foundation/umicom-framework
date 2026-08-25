/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_custody_account.c
 *
 * PURPOSE:
 *   Exercise custody account validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_account.h"
int main(void) {
    UmiTreasuryCustodyAccount v;
    if (umi_treasury_custody_account_init(&v, "cust", "custodian", true) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_custody_account_is_segregated(&v))return 2;
    return 0;
}
