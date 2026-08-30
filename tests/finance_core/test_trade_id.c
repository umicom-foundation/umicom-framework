/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_trade_id.c
 *
 * PURPOSE:
 *   Exercise the trade id financial-core contract.
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
#include "umicom/finance/core/trade_id.h"

int main(void)
{
    UmiTradeId x= {0}; CHECK(umi_trade_id_set(&x,"ID")==UMI_STATUS_OK); CHECK(umi_trade_id_is_valid(&x));
    return 0;
}
