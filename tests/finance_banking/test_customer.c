/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_customer.c
 *
 * PURPOSE:
 *   Exercise customer validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingCustomer v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_customer_init(&v, "cust-1", "le-1", "Customer One", UMI_BANKING_CUSTOMER_ACTIVE)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_banking_customer_operational(&v)) return 2;
    return 0;
}
