/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_lifecycle_event.c
 *
 * PURPOSE:
 *   Exercise the lifecycle event financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/lifecycle_event.h"

int main(void)
{
    UmiLifecycleEvent x; CHECK(umi_lifecycle_event_init(&x,"ID","Name","PARENT",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_lifecycle_event_is_valid(&x));
    return 0;
}
