/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_counterparty_registry.c
 *
 * PURPOSE:
 *   Exercise the counterparty registry financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/counterparty_registry.h"

int main(void)
{
    UmiCounterpartyRegistry r; UmiCounterparty x; umi_counterparty_registry_init(&r); CHECK(umi_counterparty_init(&x,"CP","Counterparty","LE","BROKER")==UMI_STATUS_OK); CHECK(umi_counterparty_registry_add(&r,&x)==UMI_STATUS_OK);
    return 0;
}
