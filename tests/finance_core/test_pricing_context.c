/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_pricing_context.c
 *
 * PURPOSE:
 *   Exercise the pricing context financial-core contract.
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
#include "umicom/finance/core/pricing_context.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiPricingContext x; CHECK(umi_pricing_context_init(&x,"ID","Name","CODE",(UmiFinancialDate){2026,8U,25U})==UMI_STATUS_OK); CHECK(umi_pricing_context_is_valid(&x));
    return 0;
}
