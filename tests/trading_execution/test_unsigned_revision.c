/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_unsigned_revision.c
 *
 * PURPOSE:
 *   Verify exact unsigned UI values and trading view revisions at signed and unsigned boundaries.
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
#include "umicom/trading_ui/trading_ui.h"
#include "umicom/ui/value.h"
/* Use the actual workspace owner to inject revision boundaries. No product
 * setter or copied struct layout is added, and no GUI is launched. */
#include "../../src/trading/workspace.c"
#define UNSIGNED_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef UmiStatus (*RevisionFactory)(const char *, UmiTradingWorkspace *, UmiUiViewModel **);
typedef struct UnsignedCase { const char *name; uint64_t value; const char *decimal; } UnsignedCase;

static int CheckValue(const UmiUiValue *value, uint64_t number, const char *decimal)
{
    if (number <= (uint64_t)INT64_MAX) {
        UNSIGNED_CHECK(value->kind == UMI_UI_VALUE_INTEGER);
        UNSIGNED_CHECK(value->integer_value == (int64_t)number);
    } else {
        UNSIGNED_CHECK(value->kind == UMI_UI_VALUE_STRING);
        UNSIGNED_CHECK(!strcmp(value->string_value, decimal));
        UNSIGNED_CHECK(value->integer_value == 0 && value->real_value == 0.0);
    }
    return 0;
}
static int CheckProperty(UmiUiViewModel *view, const char *key,
    uint64_t number, const char *decimal)
{
    UmiUiValue value;
    UNSIGNED_CHECK(umi_ui_view_model_get_property(view, key, &value) == UMI_STATUS_OK);
    return CheckValue(&value, number, decimal);
}
static int CheckViews(uint64_t number, const char *decimal)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiInstrument instrument = test_instrument();
    UNSIGNED_CHECK(umi_trading_workspace_create(NULL, &workspace) == UMI_STATUS_OK);
    UNSIGNED_CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    workspace->revision = number;
    RevisionFactory factories[] = {
        umi_trading_ui_dashboard_view_create,
        umi_trading_ui_watchlist_view_create,
        umi_trading_ui_depth_view_create,
        umi_trading_ui_chart_view_create,
        umi_trading_ui_order_ticket_view_create,
        umi_trading_ui_orders_view_create,
        umi_trading_ui_executions_view_create,
        umi_trading_ui_portfolio_risk_view_create,
        umi_trading_ui_scanner_view_create,
        umi_trading_ui_predictive_lab_view_create,
        umi_trading_ui_news_view_create,
        umi_trading_ui_context_inspector_view_create,
        umi_trading_ui_strategy_view_create,
        umi_trading_ui_replay_view_create,
        umi_trading_ui_research_output_view_create,
        umi_trading_ui_time_and_sales_view_create,
        umi_trading_ui_economic_calendar_view_create,
        umi_trading_ui_fundamentals_view_create,
        umi_trading_ui_alerts_view_create
    };
    for (size_t i = 0U; i < sizeof(factories) / sizeof(factories[0]); ++i) {
        UmiUiViewModel *view = NULL;
        UNSIGNED_CHECK(factories[i]("revision-observation", workspace, &view) == UMI_STATUS_OK);
        UNSIGNED_CHECK(CheckProperty(view, "trading.revision", number, decimal) == 0);
        if (factories[i] == umi_trading_ui_context_inspector_view_create)
            UNSIGNED_CHECK(CheckProperty(view, "context.workspace-revision", number, decimal) == 0);
        if (factories[i] == umi_trading_ui_research_output_view_create)
            UNSIGNED_CHECK(CheckProperty(view, "research.workspace-revision", number, decimal) == 0);
        umi_ui_view_model_destroy(view);
        UNSIGNED_CHECK(workspace->revision == number);
    }
    umi_trading_workspace_destroy(workspace); return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "null")) {
        UNSIGNED_CHECK(UmiUiValueSetUnsigned(NULL, UINT64_MAX) == UMI_STATUS_INVALID_ARGUMENT); return 0;
    }
    if (!strcmp(argv[1], "views-small")) return CheckViews(42U, "42");
    if (!strcmp(argv[1], "views-boundary")) return CheckViews(UINT64_C(9223372036854775808), "9223372036854775808");
    if (!strcmp(argv[1], "views-max")) return CheckViews(UINT64_MAX, "18446744073709551615");
    const UnsignedCase cases[] = {
        {"value-zero", 0U, "0"}, {"value-one", 1U, "1"}, {"value-decimal", 10U, "10"},
        {"value-u32", UINT64_C(4294967295), "4294967295"},
        {"value-real-boundary", UINT64_C(9007199254740992), "9007199254740992"},
        {"value-real-loss", UINT64_C(9007199254740993), "9007199254740993"},
        {"value-signed-max", UINT64_C(9223372036854775807), "9223372036854775807"},
        {"value-unsigned-half", UINT64_C(9223372036854775808), "9223372036854775808"},
        {"value-near-max", UINT64_C(18446744073709551614), "18446744073709551614"},
        {"value-max", UINT64_MAX, "18446744073709551615"}
    };
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(argv[1], cases[i].name)) continue;
        UmiUiValue value, copy;
        memset(&value, 0xa5, sizeof(value));
        UNSIGNED_CHECK(UmiUiValueSetUnsigned(&value, cases[i].value) == UMI_STATUS_OK);
        UNSIGNED_CHECK(CheckValue(&value, cases[i].value, cases[i].decimal) == 0);
        UNSIGNED_CHECK(umi_ui_value_copy(&copy, &value) == UMI_STATUS_OK);
        UNSIGNED_CHECK(umi_ui_value_equal(&copy, &value));
        UNSIGNED_CHECK(UmiUiValueSetUnsigned(&value, 0U) == UMI_STATUS_OK);
        UNSIGNED_CHECK(value.kind == UMI_UI_VALUE_INTEGER && value.integer_value == 0);
        UNSIGNED_CHECK(value.string_value[0] == '\0');
        return 0;
    }
    return 2;
}
