/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_physical_contract.c
 *
 * PURPOSE:
 *   Implement the test physical contract behavior for
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

#include "umicom/finance/commodity/physical_contract.h"

int main(void)
{
    UmiCommodityPhysicalContract value;
    UmiFinancialId buyer = {{"BUYER"}};
    UmiFinancialId seller = {{"SELLER"}};
    UmiCurrency currency = {{'U','S','D','\0'}};
    CHECK(umi_commodity_physical_contract_init(&value, "CTR-1", "CMD-WTI", &buyer, &seller, 1000, 0, "BBL", &currency, 7500, 1000, 2000) == UMI_STATUS_OK);
    CHECK(umi_commodity_physical_contract_valid(&value));
    return 0;
}
