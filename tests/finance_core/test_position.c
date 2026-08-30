/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_position.c
 *
 * PURPOSE:
 *   Exercise the position financial-core contract.
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
#include "umicom/finance/core/position.h"

int main(void)
{
    UmiPositionKey k; UmiPosition p; CHECK(umi_position_key_init(&k,"B","P","X")==UMI_STATUS_OK); CHECK(umi_position_init(&p,&k,(UmiQuantity){1.0},(UmiFinancialDate){2026,8U,25U})==UMI_STATUS_OK); CHECK(umi_position_add(&p,(UmiQuantity){2.0})==UMI_STATUS_OK);
    return 0;
}
