/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_settlement_event.c
 *
 * PURPOSE:
 *   Exercise the settlement event financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/settlement_event.h"

int main(void)
{
    UmiSettlementEvent x; CHECK(umi_settlement_event_init(&x,"ID","Name","PARENT",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_settlement_event_is_valid(&x));
    return 0;
}
