/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_margin_dispute.c
 *
 * PURPOSE:
 *   Exercise margin dispute validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_dispute.h"
int main(void) {
    UmiTreasuryMarginDispute v;
    if (umi_treasury_margin_dispute_init(&v, "disp", 200, 50) != UMI_STATUS_OK) return 1;
    if(umi_treasury_margin_dispute_outstanding_minor(&v)!=150)return 2;
    return 0;
}
