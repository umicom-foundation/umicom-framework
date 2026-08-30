/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_overdraft_facility.c
 *
 * PURPOSE:
 *   Exercise overdraft facility validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/overdraft_facility.h"
int main(void) {
    UmiBankingOverdraftFacility v;
    if(umi_banking_overdraft_facility_init(&v, "od-1", "dep-1", 10000, 2500)!=UMI_STATUS_OK) return 1;
    if(umi_banking_overdraft_facility_available_minor(&v)!=7500) return 2;
    return 0;
}
