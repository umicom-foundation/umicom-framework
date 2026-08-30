/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_day_count.c
 *
 * PURPOSE:
 *   Exercise the day count financial-core contract.
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
#include "umicom/finance/core/day_count.h"

int main(void)
{
    double f=0.0; CHECK(umi_day_count_fraction((UmiFinancialDate){2026,1U,1U},(UmiFinancialDate){2027,1U,1U},UMI_DAY_COUNT_ACT_365F,&f)==UMI_STATUS_OK); CHECK(f>0.99&&f<1.01);
    return 0;
}
