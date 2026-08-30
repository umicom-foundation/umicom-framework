/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_fixing.c
 *
 * PURPOSE:
 *   Exercise the fixing financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/fixing.h"

int main(void)
{
    UmiMarketDataKey k; UmiFixing f; CHECK(umi_market_data_key_init(&k,"K","Key","SRC",1U)==UMI_STATUS_OK); CHECK(umi_fixing_init(&f,&k,(UmiFinancialDate){2026,8U,25U},1.0,1)==UMI_STATUS_OK);
    return 0;
}
