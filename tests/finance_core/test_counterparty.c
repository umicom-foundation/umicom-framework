/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_counterparty.c
 *
 * PURPOSE:
 *   Exercise the counterparty financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/counterparty.h"

int main(void)
{
    UmiCounterparty x; CHECK(umi_counterparty_init(&x,"ID","Name","PARENT","CODE")==UMI_STATUS_OK); CHECK(umi_counterparty_is_valid(&x));
    return 0;
}
