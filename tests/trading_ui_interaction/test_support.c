/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_support.c
 *
 * PURPOSE:
 *   Build and destroy simulation-only workspaces for interaction tests.
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
 * Exercise changed and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void changed(uint64_t revision, void *user_data)
{
    UmiTradingUiTestFixture *fixture = user_data;
    assert(fixture != NULL);
    fixture->changed_count += 1U;
    fixture->last_revision = revision;
}

/*
 * Initialise trading ui test fixture from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_ui_test_fixture_init(UmiTradingUiTestFixture *fixture,
                                      int seed_market)
{
    UmiTradingWorkspaceConfig config;
    UmiTradingUiControllerConfig controller_config;
    assert(fixture != NULL);
    (void)memset(fixture, 0, sizeof(*fixture));
    config = umi_trading_workspace_config_default();
    assert(umi_trading_workspace_create(&config, &fixture->workspace) ==
           UMI_STATUS_OK);
    controller_config = umi_trading_ui_controller_config_default();
    assert(umi_trading_ui_controller_init(
               &fixture->controller, fixture->workspace,
               &controller_config) == UMI_STATUS_OK);
    umi_trading_ui_controller_set_changed_handler(
        &fixture->controller, changed, fixture);
    assert(umi_trading_simulation_market_init(
               &fixture->simulation, fixture->workspace) == UMI_STATUS_OK);
    /* Apply this branch only when its contract condition is satisfied. */
    if (seed_market)
        assert(umi_trading_simulation_market_seed_default(
                   &fixture->simulation, 1000000) == UMI_STATUS_OK);
}

/*
 * Release or reset state held by trading ui test fixture so the same storage can be reused
 * safely.
 */
void umi_trading_ui_test_fixture_destroy(UmiTradingUiTestFixture *fixture)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fixture == NULL) return;
    umi_trading_ui_controller_set_changed_handler(
        &fixture->controller, NULL, NULL);
    umi_trading_workspace_destroy(fixture->workspace);
    fixture->workspace = NULL;
}
