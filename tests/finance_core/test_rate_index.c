/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_rate_index.c
 *
 * PURPOSE:
 *   Exercise the rate index financial-core contract.
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
#include "umicom/finance/core/rate_index.h"

int main(void)
{
    UmiRateIndex x; CHECK(umi_rate_index_init(&x,"ID","Name","CODE",1U)==UMI_STATUS_OK); CHECK(umi_rate_index_is_valid(&x));
    return 0;
}
