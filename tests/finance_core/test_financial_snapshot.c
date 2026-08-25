/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_financial_snapshot.c
 *
 * PURPOSE:
 *   Exercise the financial snapshot financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/financial_snapshot.h"

int main(void)
{
    UmiFinancialSnapshot x; CHECK(umi_financial_snapshot_init(&x,"ID","Name","CODE",1U)==UMI_STATUS_OK); CHECK(umi_financial_snapshot_is_valid(&x));
    return 0;
}
