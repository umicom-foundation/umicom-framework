/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_trading_workbench.c
 *
 * PURPOSE:
 *   Verify Framework Trader/TMS profile contains canonical trading and operations groups/endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection_provider/trading_workbench.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchSelectionProviderTradingWorkbench profile;
    assert(umi_workbench_selection_provider_trading_workbench_build(
        &profile) == UMI_STATUS_OK);
    assert(profile.trader.group_count == 2U);
    assert(profile.trader.endpoint_count == 7U);
    assert(profile.tms.group_count == 2U);
    assert(profile.tms.endpoint_count == 4U);
    assert(strcmp(profile.trading_group_id, "trading.red") == 0);
    assert(strcmp(profile.operations_group_id, "operations.green") == 0);
    assert(umi_workbench_selection_provider_trading_workbench_validate(
        &profile) == UMI_STATUS_OK);

    return 0;
}
