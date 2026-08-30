/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_funding_source_catalogue.c
 *
 * PURPOSE:
 *   Verify bounded funding source catalogue registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_source_catalogue.h"
int main(void) { UmiTreasuryFundingSourceCatalogue r; UmiTreasuryFundingSource item; umi_treasury_funding_source_catalogue_init(&r);
    if (umi_treasury_funding_source_init(&item, "facility", 1000, 100, 80) != UMI_STATUS_OK) return 1;
    if (umi_treasury_funding_source_catalogue_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_funding_source_catalogue_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_funding_source_catalogue_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
