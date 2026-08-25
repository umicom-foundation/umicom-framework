/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_account_hold.c
 *
 * PURPOSE:
 *   Exercise account hold validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/account_hold.h"
int main(void) {
    UmiBankingAccountHold v;
    if(umi_banking_account_hold_init(&v, "hold-1", "dep-1", 500, true)!=UMI_STATUS_OK) return 1;
    if(!umi_banking_account_hold_valid(&v)) return 2;
    return 0;
}
