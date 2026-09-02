/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_notional.c
 *
 * PURPOSE:
 *   Exercise the notional financial-core contract.
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
#include "umicom/finance/core/notional.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNotional n; UmiMoney m={1,2U,{{'U','S','D','\0'}}}; CHECK(umi_notional_init(&n,m)==UMI_STATUS_OK);
    return 0;
}
