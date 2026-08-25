/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_financial_core_service.c
 *
 * PURPOSE:
 *   Exercise the financial core service financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/financial_core_service.h"

int main(void)
{
    UmiFinancialCoreService s; umi_financial_core_service_init(&s); CHECK(umi_financial_core_service_start(&s)==UMI_STATUS_OK); CHECK(s.ready);
    return 0;
}
