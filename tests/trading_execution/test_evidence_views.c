/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_evidence_views.c
 * PURPOSE: Exercise the real controller and shared order-ticket view model.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "../trading/test_trading_common.h"
#include "umicom/trading_ui/trading_ui.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)

/* A real subscriber must receive both an approval and a subsequent denial. */
static void Changed(uint64_t revision, void *userData)
{
    (void)revision;
    (*(size_t *)userData)++;
}

static int Run(const char *name, UmiTradingWorkspace *workspace)
{
    UmiTradingUiController controller;
    UmiInstrument instrument = test_instrument();
    UmiQuote quote = {0};
    UmiRiskDecision decision;
    UmiUiValue value;
    UmiUiViewModel *view = NULL;
    size_t changes = 0U;
    CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    CHECK(umi_trading_ui_controller_init(&controller, workspace, NULL) == UMI_STATUS_OK);
    umi_trading_ui_controller_set_changed_handler(&controller, Changed, &changes);
    CHECK(umi_trading_workspace_set_health(workspace, 1, 0, 1) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_type(workspace, UMI_ORDER_MARKET, UMI_TIF_DAY) == UMI_STATUS_OK);
    quote.instrument = instrument;
    quote.bid = 99; quote.ask = 101; quote.bid_size = 5; quote.ask_size = 5;
    quote.event_time_ms = 1000;
    CHECK(umi_trading_workspace_update_quote(workspace, &quote) == UMI_STATUS_OK);
    if (strcmp(name, "empty") != 0) {
        CHECK(UmiTradingUiControllerPreviewOrderAt(&controller, 1500, &decision) == UMI_STATUS_OK);
        CHECK(decision.allowed && changes == 1U);
    }
    if (strcmp(name, "denial") == 0) {
        CHECK(UmiTradingUiControllerPreviewOrderAt(&controller, 6001, &decision) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(!decision.allowed && changes == 2U);
        CHECK(umi_trading_ui_controller_submit_order(&controller, 6001, &decision) != UMI_STATUS_OK);
    }
    CHECK(umi_trading_ui_order_ticket_view_create("test.ticket", workspace, &view) == UMI_STATUS_OK);
    CHECK(umi_ui_view_model_get_property(view, "risk.has-valuation", &value) == UMI_STATUS_OK);
    CHECK(value.kind == UMI_UI_VALUE_BOOLEAN);
    if (strcmp(name, "approved") == 0) {
        CHECK(value.boolean_value);
        CHECK(umi_ui_view_model_get_property(view, "risk.reference-price", &value) == UMI_STATUS_OK);
        CHECK(value.kind == UMI_UI_VALUE_REAL && value.real_value == 101);
        CHECK(umi_ui_view_model_get_property(view, "risk.notional", &value) == UMI_STATUS_OK && value.real_value == 2020);
        CHECK(umi_ui_view_model_get_property(view, "risk.quote-age-ms", &value) == UMI_STATUS_OK && value.integer_value == 500);
        CHECK(umi_ui_view_model_get_property(view, "risk.price-source", &value) == UMI_STATUS_OK);
        CHECK(strcmp(value.string_value, "quote ask") == 0);
    } else if (strcmp(name, "denial") == 0 || strcmp(name, "empty") == 0) {
        CHECK(!value.boolean_value);
        CHECK(umi_ui_view_model_get_property(view, "risk.reference-price", &value) == UMI_STATUS_OK && value.real_value == 0);
    } else return 2;
    umi_ui_view_model_destroy(view);
    return 0;
}

int main(int argc, char **argv)
{
    UmiTradingWorkspace *workspace = NULL;
    if (argc != 2 || umi_trading_workspace_create(NULL, &workspace) != UMI_STATUS_OK) return 1;
    int status = Run(argv[1], workspace);
    umi_trading_workspace_destroy(workspace);
    return status;
}
