/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_live_opt_in.c
 *
 * PURPOSE:
 *   Verify explicit policy may expose live environment selection without
 *   implicitly arming live execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include <assert.h>
#include <string.h>

/*
 * Exercise test controller live opt in and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_live_opt_in(void)
{
    UmiTradingWorkspaceConfig workspace_config;
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingUiController controller;
    UmiTradingUiControllerConfig config;
    UmiTradingWorkspaceSnapshot snapshot;
    workspace_config = umi_trading_workspace_config_default();
    assert(umi_trading_workspace_create(&workspace_config, &workspace) ==
           UMI_STATUS_OK);
    config = umi_trading_ui_controller_config_default();
    config.allow_live_environment = 1;
    assert(umi_trading_ui_controller_init(&controller, workspace, &config) ==
           UMI_STATUS_OK);
    assert(umi_trading_ui_controller_set_environment(
               &controller, UMI_TRADING_LIVE) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.environment == UMI_TRADING_LIVE);
    assert(!snapshot.live_armed);
    umi_trading_workspace_destroy(workspace);
}
