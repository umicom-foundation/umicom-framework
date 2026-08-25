/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_banking_service.c
 *
 * PURPOSE:
 *   Exercise banking service validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/banking_service.h"
int main(void) {
    UmiBankingBankingService v;
    if(umi_banking_banking_service_init(&v, "banking-service", true, 10U, 12U, 3U, 0U)!=UMI_STATUS_OK) return 1;
    if(!umi_banking_banking_service_ready(&v)) return 2;
    return 0;
}
