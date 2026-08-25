/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_banking_snapshot.c
 *
 * PURPOSE:
 *   Exercise banking snapshot validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/banking_snapshot.h"
int main(void) {
    UmiBankingBankingSnapshot v;
    if(umi_banking_banking_snapshot_init(&v, "snap-1", 10U, 8U, 3U, 1000000, 600000)!=UMI_STATUS_OK) return 1;
    if(umi_banking_banking_snapshot_net_funding_minor(&v)!=400000) return 2;
    return 0;
}
