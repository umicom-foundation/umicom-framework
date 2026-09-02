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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingBankRelationship v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_bank_relationship_init(&v, "rel-1", "cust-1", "Manager One", true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_bank_relationship_valid(&v)) return 2;
    return 0;
}
