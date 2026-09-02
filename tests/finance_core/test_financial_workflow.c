/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_financial_workflow.c
 *
 * PURPOSE:
 *   Exercise the financial workflow financial-core contract.
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
#include "umicom/finance/core/financial_workflow.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiFinancialWorkflow x; CHECK(umi_financial_workflow_init(&x,"ID","Name","PARENT",1U)==UMI_STATUS_OK); CHECK(umi_financial_workflow_is_valid(&x));
    return 0;
}
