/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_limit_book.c
 *
 * PURPOSE:
 *   Verify bounded risk limit book registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_limit_book.h"
int main(void) { UmiTreasuryRiskLimitBook r; UmiTreasuryRiskLimit item; umi_treasury_risk_limit_book_init(&r);
    if (umi_treasury_risk_limit_init(&item, "limit", 1000, 800) != UMI_STATUS_OK) return 1;
    if (umi_treasury_risk_limit_book_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_risk_limit_book_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_risk_limit_book_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
