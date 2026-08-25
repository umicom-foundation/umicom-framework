/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_tenor.c
 *
 * PURPOSE:
 *   Exercise the tenor financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/tenor.h"

int main(void)
{
    UmiTenor t; UmiFinancialDate d; CHECK(umi_tenor_parse("6M",&t)==UMI_STATUS_OK); CHECK(umi_tenor_add((UmiFinancialDate){2026,8U,31U},t,&d)==UMI_STATUS_OK); CHECK(d.month==2U);
    return 0;
}
