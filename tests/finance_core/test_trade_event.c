/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_trade_event.c
 *
 * PURPOSE:
 *   Exercise the trade event financial-core contract.
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
#include "umicom/finance/core/trade_event.h"

int main(void)
{
    UmiTradeEvent x; CHECK(umi_trade_event_init(&x,"ID","Name","PARENT",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_trade_event_is_valid(&x));
    return 0;
}
