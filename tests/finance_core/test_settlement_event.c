/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_settlement_event.c
 *
 * PURPOSE:
 *   Exercise the settlement event financial-core contract.
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
#include "umicom/finance/core/settlement_event.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSettlementEvent x; CHECK(umi_settlement_event_init(&x,"ID","Name","PARENT",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_settlement_event_is_valid(&x));
    return 0;
}
