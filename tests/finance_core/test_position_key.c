/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_position_key.c
 *
 * PURPOSE:
 *   Exercise the position key financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/position_key.h"

int main(void)
{
    UmiPositionKey a,b; CHECK(umi_position_key_init(&a,"B","P","X")==UMI_STATUS_OK); CHECK(umi_position_key_init(&b,"B","P","X")==UMI_STATUS_OK); CHECK(umi_position_key_equal(&a,&b));
    return 0;
}
