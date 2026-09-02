/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_customer_registry.c
 *
 * PURPOSE:
 *   Verify bounded customer registry insertion, duplicate prevention and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiBankingCustomerRegistry r; UmiBankingCustomer item; umi_banking_customer_registry_init(&r);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_customer_init(&item, "cust-1", "le-1", "Customer One", UMI_BANKING_CUSTOMER_ACTIVE)!=UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_customer_registry_add(&r,&item)!=UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_customer_registry_add(&r,&item)!=UMI_STATUS_ALREADY_EXISTS) return 3;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(umi_banking_customer_registry_find(&r,item.id.value)==NULL) return 4;
    return 0;
}
