/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_pnl_explain.c
 *
 * PURPOSE:
 *   Implement the test pnl explain behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/pnl_explain.h"

int main(void)
{
    UmiQuantPnlExplain value;
    CHECK(umi_quant_pnl_explain_init(&value, 80.0, 10.0, 100.0) == UMI_STATUS_OK);
    CHECK(umi_quant_pnl_explain_unexplained(&value) > 9.9 && umi_quant_pnl_explain_unexplained(&value) < 10.1);
    return 0;
}
