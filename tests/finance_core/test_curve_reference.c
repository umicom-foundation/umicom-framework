/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_curve_reference.c
 *
 * PURPOSE:
 *   Exercise the curve reference financial-core contract.
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
#include "umicom/finance/core/curve_reference.h"

int main(void)
{
    UmiCurveReference x; CHECK(umi_curve_reference_init(&x,"ID","Name","CODE")==UMI_STATUS_OK); CHECK(umi_curve_reference_is_valid(&x));
    return 0;
}
