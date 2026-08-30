/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_sensitivity_request.c
 *
 * PURPOSE:
 *   Implement the test sensitivity request behavior for
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

#include "umicom/finance/quant/sensitivity_request.h"

int main(void)
{
    UmiQuantSensitivityRequest value;
    CHECK(umi_quant_sensitivity_request_init(&value, 100.0, 1.0, 1) == UMI_STATUS_OK);
    CHECK(umi_quant_sensitivity_request_up_value(&value) == 101.0);
    return 0;
}
