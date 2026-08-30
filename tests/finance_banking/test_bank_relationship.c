/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_bank_relationship.c
 *
 * PURPOSE:
 *   Exercise bank relationship validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_relationship.h"
int main(void) {
    UmiBankingBankRelationship v;
    if(umi_banking_bank_relationship_init(&v, "rel-1", "cust-1", "Manager One", true)!=UMI_STATUS_OK) return 1;
    if(!umi_banking_bank_relationship_valid(&v)) return 2;
    return 0;
}
