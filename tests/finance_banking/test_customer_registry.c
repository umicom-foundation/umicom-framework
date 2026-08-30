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
int main(void) { UmiBankingCustomerRegistry r; UmiBankingCustomer item; umi_banking_customer_registry_init(&r);
    if(umi_banking_customer_init(&item, "cust-1", "le-1", "Customer One", UMI_BANKING_CUSTOMER_ACTIVE)!=UMI_STATUS_OK) return 1;
    if(umi_banking_customer_registry_add(&r,&item)!=UMI_STATUS_OK) return 2;
    if(umi_banking_customer_registry_add(&r,&item)!=UMI_STATUS_ALREADY_EXISTS) return 3;
    if(umi_banking_customer_registry_find(&r,item.id.value)==NULL) return 4;
    return 0;
}
