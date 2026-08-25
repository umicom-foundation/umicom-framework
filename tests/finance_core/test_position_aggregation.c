/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_position_aggregation.c
 *
 * PURPOSE:
 *   Exercise the position aggregation financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/position_aggregation.h"

int main(void)
{
    UmiPositionBook b; double t=0.0; umi_position_book_init(&b); CHECK(umi_position_aggregation_total(&b,&t)==UMI_STATUS_OK&&t==0.0);
    return 0;
}
