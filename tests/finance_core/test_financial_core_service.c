/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_financial_core_service.c
 *
 * PURPOSE:
 *   Exercise the financial core service financial-core contract.
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
#include "umicom/finance/core/financial_core_service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiFinancialCoreService s; umi_financial_core_service_init(&s); CHECK(umi_financial_core_service_start(&s)==UMI_STATUS_OK); CHECK(s.ready);
    return 0;
}
