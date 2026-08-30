/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_contract_book.c
 *
 * PURPOSE:
 *   Implement the test contract book behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/contract_book.h"

int main(void)
{
    UmiCommodityContractBook catalogue;
    UmiCommodityPhysicalContract item;
    umi_commodity_contract_book_init(&catalogue);
    { UmiFinancialId buyer = {{"BUYER"}}; UmiFinancialId seller = {{"SELLER"}}; UmiCurrency currency = {{'U','S','D','\0'}}; CHECK(umi_commodity_physical_contract_init(&item, "CTR-1", "CMD-WTI", &buyer, &seller, 1000, 0, "BBL", &currency, 7500, 1000, 2000) == UMI_STATUS_OK); }
    CHECK(umi_commodity_contract_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_commodity_contract_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_commodity_contract_book_find(&catalogue, "CTR-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
