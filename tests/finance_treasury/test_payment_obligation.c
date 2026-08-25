/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_payment_obligation.c
 *
 * PURPOSE:
 *   Exercise payment obligation validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/payment_obligation.h"
int main(void) {
    UmiTreasuryPaymentObligation v;
    if (umi_treasury_payment_obligation_init(&v, "pay-1", 1000, 900, 100) != UMI_STATUS_OK) return 1;
    if(umi_treasury_payment_obligation_outstanding_minor(&v)!=800)return 2;
    return 0;
}
