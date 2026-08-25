/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_notional.c
 *
 * PURPOSE:
 *   Exercise the notional financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/notional.h"

int main(void)
{
    UmiNotional n; UmiMoney m={1,2U,{{'U','S','D','\0'}}}; CHECK(umi_notional_init(&n,m)==UMI_STATUS_OK);
    return 0;
}
