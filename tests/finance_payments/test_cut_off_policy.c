/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_cut_off_policy.c
 *
 * PURPOSE:
 *   Exercise cut off policy validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/cut_off_policy.h"
int main(void) {
    UmiPaymentsCutOffPolicy v;
    if(umi_payments_cut_off_policy_init(&v, "cutoff-1", "rail-1", 1020U, true)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_cut_off_policy_accepts_minute_600(&v)) return 2;
    return 0;
}
