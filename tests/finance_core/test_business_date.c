/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_business_date.c
 *
 * PURPOSE:
 *   Exercise the business date financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/business_date.h"

int main(void)
{
    UmiFinancialDate d={2026,8U,25U},n={0}; CHECK(umi_business_date_add(d,1,&n)==UMI_STATUS_OK); CHECK(n.day==26U);
    return 0;
}
