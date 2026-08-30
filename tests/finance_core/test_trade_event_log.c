/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_trade_event_log.c
 *
 * PURPOSE:
 *   Exercise the trade event log financial-core contract.
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
#include "umicom/finance/core/trade_event_log.h"

int main(void)
{
    UmiTradeEventLog l; UmiTradeEvent e; umi_trade_event_log_init(&l); CHECK(umi_trade_event_init(&e,"E","Event","T",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_trade_event_log_add(&l,&e)==UMI_STATUS_OK);
    return 0;
}
