/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/action_recovery/test_feedback.c
 * PURPOSE: Check trading UI rejection feedback without submitting real orders.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_ui/trading_ui.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct FeedbackFixture {
    UmiTradingWorkspace *workspace;
    UmiTradingUiController controller;
    size_t calls;
    UmiTradingUiControllerSnapshot observed;
} FeedbackFixture;

/* The observer sees fully published feedback, including the failure status. */
static void ObserveFeedback(uint64_t revision, void *userData)
{
    FeedbackFixture *fixture = userData;
    fixture->calls += 1U;
    fixture->observed = fixture->controller.state;
    if (fixture->observed.revision != revision) fixture->observed.last_status = UMI_STATUS_INTERNAL_ERROR;
}

int main(int argc, char **argv)
{
    FeedbackFixture fixture = {0};
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    UmiTradingSimulationMarket simulation;
    UmiTradingWorkspaceSnapshot before, after;
    UmiRiskDecision decision = {0};
    UmiStatus status;
    const char *mode = argc > 1 ? argv[1] : "live-denied";
    REQUIRE(umi_trading_workspace_create(&config, &fixture.workspace) == UMI_STATUS_OK);
    REQUIRE(umi_trading_ui_controller_init(&fixture.controller, fixture.workspace, NULL) == UMI_STATUS_OK);
    REQUIRE(umi_trading_simulation_market_init(&simulation, fixture.workspace) == UMI_STATUS_OK);
    REQUIRE(umi_trading_simulation_market_seed_default(&simulation, 1000000) == UMI_STATUS_OK);
    /* A finite draft can still exceed the configured risk limit. Arrange an
     * explicit rejection instead of assuming seeded market defaults fail. */
    if (strcmp(mode, "preview-once") == 0)
        REQUIRE(umi_trading_workspace_set_draft_quantity(fixture.workspace, 1e100) == UMI_STATUS_OK);
    umi_trading_ui_controller_set_changed_handler(&fixture.controller, ObserveFeedback, &fixture);
    REQUIRE(umi_trading_workspace_snapshot(fixture.workspace, &before) == UMI_STATUS_OK);
    const uint64_t revision = fixture.controller.state.revision;
    if (strcmp(mode, "live-denied") == 0) {
        status = umi_trading_ui_controller_set_environment(&fixture.controller, UMI_TRADING_LIVE);
        REQUIRE(status == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(mode, "submit-denied") == 0) {
        status = umi_trading_ui_controller_submit_order(&fixture.controller, 1001000, &decision);
        REQUIRE(status == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "selection-missing") == 0) {
        status = umi_trading_ui_controller_select_instrument(&fixture.controller, "SIM.MISSING");
        REQUIRE(status == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(mode, "cancel-missing") == 0) {
        status = umi_trading_ui_controller_cancel_selected_order(&fixture.controller);
        REQUIRE(status != UMI_STATUS_OK);
    } else if (strcmp(mode, "quantity-invalid") == 0) {
        status = umi_trading_ui_controller_set_draft_quantity(&fixture.controller, INFINITY);
        REQUIRE(status != UMI_STATUS_OK);
    } else if (strcmp(mode, "preview-once") == 0) {
        status = UmiTradingUiControllerPreviewOrderAt(&fixture.controller, 1001000, &decision);
        REQUIRE(status != UMI_STATUS_OK);
    } else if (strcmp(mode, "valid-selection") == 0) {
        status = umi_trading_ui_controller_select_instrument(&fixture.controller, "SIM.EURUSD");
        REQUIRE(status == UMI_STATUS_OK);
    } else if (strcmp(mode, "no-handler") == 0) {
        umi_trading_ui_controller_set_changed_handler(&fixture.controller, NULL, NULL);
        status = umi_trading_ui_controller_set_environment(&fixture.controller, UMI_TRADING_LIVE);
        REQUIRE(status == UMI_STATUS_PERMISSION_DENIED);
    } else { fprintf(stderr, "Unknown test case\n"); return 2; }
    REQUIRE(fixture.controller.state.revision == revision + 1U);
    REQUIRE(fixture.controller.state.last_status == status);
    REQUIRE(fixture.controller.state.last_message[0] != '\0');
    if (strcmp(mode, "no-handler") == 0) REQUIRE(fixture.calls == 0U);
    else {
        REQUIRE(fixture.calls == 1U);
        REQUIRE(fixture.observed.last_status == status);
        REQUIRE(fixture.observed.revision == fixture.controller.state.revision);
    }
    REQUIRE(umi_trading_workspace_snapshot(fixture.workspace, &after) == UMI_STATUS_OK);
    REQUIRE(after.environment == UMI_TRADING_SIMULATION);
    REQUIRE(!after.live_armed);
    REQUIRE(after.order_count == before.order_count && after.execution_count == before.execution_count);
    if (strcmp(mode, "valid-selection") != 0 && strcmp(mode, "preview-once") != 0) {
        REQUIRE(after.draft_order.quantity == before.draft_order.quantity);
        REQUIRE(strcmp(after.selected_instrument_id, before.selected_instrument_id) == 0);
    }
    umi_trading_ui_controller_set_changed_handler(&fixture.controller, NULL, NULL);
    umi_trading_workspace_destroy(fixture.workspace);
    puts(mode);
    return 0;
}
