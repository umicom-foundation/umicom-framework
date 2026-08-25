/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_nostro_account.c
 *
 * PURPOSE:
 *   Exercise nostro account validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/nostro_account.h"
int main(void) {
    UmiTreasuryNostroAccount v;
    if (umi_treasury_nostro_account_init(&v, "nostro", 1000, 300) != UMI_STATUS_OK) return 1;
    if(umi_treasury_nostro_account_available_minor(&v)!=700)return 2;
    return 0;
}
