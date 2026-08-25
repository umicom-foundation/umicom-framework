/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_fee_rule.c
 *
 * PURPOSE:
 *   Exercise payment fee rule validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_fee_rule.h"
int main(void) {
    UmiPaymentsPaymentFeeRule v;
    if(umi_payments_payment_fee_rule_init(&v, "fee-rule", 10, 25U, 1000)!=UMI_STATUS_OK) return 1;
    if(umi_payments_payment_fee_rule_fee_for_10000_minor(&v)!=35) return 2;
    return 0;
}
