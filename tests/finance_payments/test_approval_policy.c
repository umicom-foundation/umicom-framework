/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_approval_policy.c
 *
 * PURPOSE:
 *   Exercise approval policy validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/approval_policy.h"
int main(void) {
    UmiPaymentsApprovalPolicy v;
    if(umi_payments_approval_policy_init(&v, "approval-1", 1U, 1U, 1U, 1U, true)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_approval_policy_satisfied(&v)) return 2;
    return 0;
}
