/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
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
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/types.h"

int main(void)
{
    char text[16];
    double ratio = 0.0;
    CHECK(umi_reg_copy_text(text, sizeof text, "COREP") == UMI_STATUS_OK);
    CHECK(strcmp(text, "COREP") == 0);
    CHECK(umi_reg_hash_text(text) != 0U);
    CHECK(umi_reg_ratio(8.0, 10.0, &ratio) == UMI_STATUS_OK);
    CHECK(ratio > 0.79 && ratio < 0.81);
    return 0;
}
