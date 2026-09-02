/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_settlement_instruction.c
 *
 * PURPOSE:
 *   Exercise the settlement instruction financial-core contract.
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
#include "umicom/finance/core/settlement_instruction.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSettlementInstruction x; CHECK(umi_settlement_instruction_init(&x,"ID","Name","PARENT",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_settlement_instruction_is_valid(&x));
    return 0;
}
