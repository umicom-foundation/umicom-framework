/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_party.c
 *
 * PURPOSE:
 *   Exercise payment party validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_party.h"
int main(void) {
    UmiPaymentsPaymentParty v;
    if(umi_payments_payment_party_init(&v, "party-1", "acct-1", "Debtor", "BANKGB2L")!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_party_valid(&v)) return 2;
    return 0;
}
