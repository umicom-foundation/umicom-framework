/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_protective_controls.c
 *
 * PURPOSE:
 *   Verify protective effects, checked reset results and conservative observations at revision capacity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
#include "umicom/trading_ui/action_controller.h"
/* Include the real owner for capacity injection; controllers and the remaining
 * domain owners are supplied by their ordinary libraries. */
#include "../../src/trading/workspace.c"
#define CONTROL_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto done; } } while (0)

typedef struct ControlNotice {
    UmiTradingUiController *controller;
    unsigned count;
    UmiStatus observed;
} ControlNotice;
static void ControlChanged(uint64_t revision, void *data)
{
    ControlNotice *notice = data;
    (void)revision;
    ++notice->count;
    notice->observed = notice->controller->state.last_status;
}
int main(int argc, char **argv)
{
    static const char *known[] = {"health-ordinary", "health-last", "health-exhausted", "kill-ordinary", "kill-last", "kill-exhausted", "disarm-ordinary", "disarm-last", "disarm-exhausted", "arm-ordinary", "arm-last", "arm-exhausted", "reset-ordinary", "reset-last", "reset-exhausted", "legacy-reset-ordinary", "legacy-reset-last", "legacy-reset-exhausted", "controller-reset-ordinary", "controller-reset-last", "controller-reset-exhausted", "health-recovery", "kill-repeat", "arm-invalid-health", "arm-invalid-environment", "null"};
    if (argc != 2) return 2;
    const char *name = argv[1]; int found = 0;
    for (size_t i = 0U; i < sizeof(known)/sizeof(known[0]); ++i)
        if (!strcmp(name, known[i])) found = 1;
    if (!found) return 2;
    int result = 0;
    UmiTradingWorkspace *w = NULL, *before = NULL;
    UmiTradingSessionSource *source = NULL;
    if (!strcmp(name, "null")) {
        CONTROL_CHECK(UmiTradingWorkspaceResetKillSwitch(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CONTROL_CHECK(umi_trading_workspace_set_health(NULL, 0, 0, 0) == UMI_STATUS_INVALID_ARGUMENT);
        CONTROL_CHECK(umi_trading_workspace_set_live_armed(NULL, 0) == UMI_STATUS_INVALID_ARGUMENT);
        CONTROL_CHECK(umi_trading_ui_controller_reset_kill_switch(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        umi_trading_workspace_reset_kill_switch(NULL);
        umi_trading_workspace_engage_kill_switch(NULL, NULL);
        goto done;
    }
    UmiInstrument instrument = test_instrument();
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    config.risk_limit = (UmiRiskLimit){1000.0, 1.0e12, 5000.0, 1.0e12};
    UmiRiskDecision decision;
    CONTROL_CHECK(umi_trading_workspace_create(&config, &w) == UMI_STATUS_OK);
    CONTROL_CHECK(umi_trading_workspace_add_instrument(w, &instrument) == UMI_STATUS_OK);
    CONTROL_CHECK(umi_trading_workspace_set_health(w, 1, 1, 1) == UMI_STATUS_OK);
    CONTROL_CHECK(umi_trading_workspace_set_draft_prices(w, 100.0, 0.0) == UMI_STATUS_OK);
    CONTROL_CHECK(UmiTradingWorkspacePreviewOrderAt(w, 1000, &decision) == UMI_STATUS_OK && decision.allowed);
    int health = !strncmp(name, "health-", 7U), kill = !strncmp(name, "kill-", 5U);
    int disarm = !strncmp(name, "disarm-", 7U), arm = !strncmp(name, "arm-", 4U);
    int legacy = !strncmp(name, "legacy-", 7U), controllerCase = !strncmp(name, "controller-", 11U);
    int reset = !health && !kill && !disarm && !arm;
    int maximum = strstr(name, "-exhausted") != NULL ||
        !strcmp(name, "health-recovery") || !strcmp(name, "kill-repeat");
    int last = strstr(name, "-last") != NULL;
    if (arm || disarm) { w->environment = UMI_TRADING_LIVE; w->draft_order.environment = UMI_TRADING_LIVE; }
    w->live_armed = disarm || kill;
    if (reset || !strcmp(name, "kill-repeat")) umi_kill_switch_engage(&w->oms.kill_switch, "existing protection");
    if (!strcmp(name, "health-recovery")) { w->market_data_ready = 0; w->broker_ready = 0; w->risk_ready = 0; }
    int invalid = !strncmp(name, "arm-invalid-", 12U);
    if (!strcmp(name, "arm-invalid-health")) w->risk_ready = 0;
    if (!strcmp(name, "arm-invalid-environment")) w->environment = UMI_TRADING_SIMULATION;
    w->revision = maximum ? UINT64_MAX : last ? UINT64_MAX - 1U : 42U;
    UmiTradingUiController controller;
    CONTROL_CHECK(umi_trading_ui_controller_init(&controller, w, NULL) == UMI_STATUS_OK);
    ControlNotice notice = {&controller, 0U, UMI_STATUS_OK};
    umi_trading_ui_controller_set_changed_handler(&controller, ControlChanged, &notice);
    source = malloc(sizeof(*source)); before = malloc(sizeof(*before));
    CONTROL_CHECK(source != NULL && before != NULL);
    CONTROL_CHECK(UmiTradingCopySessionSource(w, source) == UMI_STATUS_OK);
    memcpy(before, w, sizeof(*before));
    UmiStatus expected = invalid ? UMI_STATUS_INVALID_STATE :
        maximum && (arm || reset) ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    UmiStatus status = UMI_STATUS_OK;
    if (health) {
        int ready = !strcmp(name, "health-recovery");
        status = umi_trading_workspace_set_health(w, ready, ready, ready);
    } else if (kill) umi_trading_workspace_engage_kill_switch(w, "operator protection");
    else if (disarm || arm) status = umi_trading_workspace_set_live_armed(w, arm);
    else if (controllerCase) status = umi_trading_ui_controller_reset_kill_switch(&controller);
    else if (legacy) umi_trading_workspace_reset_kill_switch(w);
    else status = UmiTradingWorkspaceResetKillSwitch(w);
    if (!legacy) CONTROL_CHECK(status == expected);
    if (expected != UMI_STATUS_OK) {
        CONTROL_CHECK(!memcmp(before, w, sizeof(*w)));
    } else {
        CONTROL_CHECK(w->revision == (maximum ? UINT64_MAX : before->revision + 1U));
        CONTROL_CHECK(!w->has_draft_risk);
        CONTROL_CHECK(!memcmp(&before->oms.orders, &w->oms.orders, sizeof(w->oms.orders)));
        CONTROL_CHECK(!memcmp(&before->positions, &w->positions, sizeof(w->positions)));
        CONTROL_CHECK(!memcmp(&before->executions, &w->executions, sizeof(w->executions)));
        if (health) {
            int ready = !strcmp(name, "health-recovery");
            CONTROL_CHECK(w->market_data_ready == ready && w->broker_ready == ready && w->risk_ready == ready);
        }
        if (kill) CONTROL_CHECK(w->oms.kill_switch.engaged && !w->live_armed &&
            !strcmp(w->oms.kill_switch.reason, "operator protection"));
        if (disarm) CONTROL_CHECK(!w->live_armed);
        if (arm) CONTROL_CHECK(w->live_armed);
        if (reset) CONTROL_CHECK(!w->oms.kill_switch.engaged);
    }
    if (controllerCase) {
        CONTROL_CHECK(notice.count == 1U && notice.observed == expected && controller.state.last_status == expected);
        CONTROL_CHECK(strstr(controller.state.last_message, expected == UMI_STATUS_OK ? "reset through" : "remains engaged") != NULL);
    }
    bool current = true;
    CONTROL_CHECK(UmiTradingSessionSourceCurrent(w, source, &current) == UMI_STATUS_OK);
    CONTROL_CHECK(current == (expected != UMI_STATUS_OK && !maximum));
    UmiTradingWorkspaceSnapshot snapshot;
    CONTROL_CHECK(umi_trading_workspace_snapshot(w, &snapshot) == UMI_STATUS_OK);
    CONTROL_CHECK(snapshot.revision == w->revision);
    if (w->revision == UINT64_MAX) {
        CONTROL_CHECK(!snapshot.can_preview_order && !snapshot.can_submit_order && !snapshot.can_reset_kill_switch);
        CONTROL_CHECK(UmiTradingCopySessionSource(w, source) == UMI_STATUS_OK);
        current = true;
        CONTROL_CHECK(UmiTradingSessionSourceCurrent(w, source, &current) == UMI_STATUS_OK && !current);
    }
done:
    free(source); free(before);
    if (w != NULL) umi_trading_workspace_destroy(w);
    return result;
}
