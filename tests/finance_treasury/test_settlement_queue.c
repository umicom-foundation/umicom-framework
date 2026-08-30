/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_queue.c
 *
 * PURPOSE:
 *   Verify bounded settlement queue registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_queue.h"
int main(void) { UmiTreasurySettlementQueue r; UmiTreasurySettlementObligation item; umi_treasury_settlement_queue_init(&r);
    if (umi_treasury_settlement_obligation_init(&item, "settle-1", 100, 5, UMI_TREASURY_SETTLEMENT_NEW) != UMI_STATUS_OK) return 1;
    if (umi_treasury_settlement_queue_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_settlement_queue_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_settlement_queue_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
