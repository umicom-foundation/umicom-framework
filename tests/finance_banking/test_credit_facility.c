/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_credit_facility.c
 *
 * PURPOSE:
 *   Exercise credit facility validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/credit_facility.h"
int main(void) {
    UmiBankingCreditFacility v;
    if(umi_banking_credit_facility_init(&v, "credit-1", "cust-1", 50000, 12500)!=UMI_STATUS_OK) return 1;
    if(umi_banking_credit_facility_available_minor(&v)!=37500) return 2;
    return 0;
}
