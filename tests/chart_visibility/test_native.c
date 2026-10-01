/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_visibility/test_native.c
 * PURPOSE: Exercise real chart visibility controls, stale rows and retained widget lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x) == UMI_STATUS_OK)

/* Locate public automation identities in the actual shared chart widget. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}

/* Drive one registered scenario through the public feature owners. */
int main(int argc, char **argv)
{
    CHECK(argc == 2); if (!gtk_init_check()) return 77;
    const char *name = argv[1]; ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingWorkspaceSnapshot before, after; OK(umi_trading_workspace_snapshot(f.workspace, &before));
    const char *id = before.selected_instrument_id;
    OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "range", (UmiChartPoint){60000, 100}, (UmiChartPoint){120000, 105}));
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
    UmiChartDrawingSnapshot drawing, actual; OK(umi_chart_drawing_registry_at(registry, 0, &drawing));
    UmiGtk4TradingPanelContext context = {f.workspace, &f.controller, 0};
    GtkWidget *root = UmiGtk4TradingInteractiveChartCreate(&context); CHECK(root != NULL); g_object_ref_sink(root);
    GtkWidget *toggle = Find(root, "trading.chart.hide-drawing"), *hide = Find(root, "trading.chart.hide-all");
    GtkWidget *show = Find(root, "trading.chart.show-all"), *objects = Find(root, "trading.chart.drawings");
    CHECK(GTK_IS_BUTTON(toggle) && GTK_IS_BUTTON(hide) && GTK_IS_BUTTON(show) && GTK_IS_DROP_DOWN(objects));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(objects), 0U);
    if (strcmp(name, "single") == 0 || strcmp(name, "hidden-move") == 0 || strcmp(name, "locked") == 0) {
        if (strcmp(name, "locked") == 0) {
            g_signal_emit_by_name(Find(root, "trading.chart.lock-drawing"), "clicked");
        }
        g_signal_emit_by_name(toggle, "clicked"); OK(umi_chart_drawing_registry_at(registry, 0, &actual));
        CHECK(actual.visibility_flags == UMI_CHART_DRAWING_VISIBILITY_HIDDEN);
        CHECK(actual.time1 == drawing.time1 && actual.value2 == drawing.value2);
        GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(objects));
        CHECK(g_list_model_get_n_items(model) == 1U);
        GObject *item = g_list_model_get_item(model, 0);
        CHECK(GTK_IS_STRING_OBJECT(item) && strstr(gtk_string_object_get_string(GTK_STRING_OBJECT(item)), "(hidden)") != NULL);
        g_object_unref(item);
        if (strcmp(name, "hidden-move") == 0) {
            g_signal_emit_by_name(Find(root, "trading.chart.move-drawing"), "clicked");
            CHECK(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Find(root, "trading.chart.cursor"))));
            uint64_t revision = actual.revision;
            OK(umi_chart_drawing_registry_at(registry, 0, &actual)); CHECK(actual.revision == revision);
        } else {
            g_signal_emit_by_name(toggle, "clicked"); OK(umi_chart_drawing_registry_at(registry, 0, &actual));
            CHECK(actual.visibility_flags == 0U);
            CHECK(actual.locked == (strcmp(name, "locked") == 0));
        }
    } else if (strcmp(name, "pane") == 0) {
        OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "ray", (UmiChartPoint){60000, 100}, (UmiChartPoint){120000, 105}));
        UmiGtk4TradingInteractiveChartRefresh(root);
        g_signal_emit_by_name(hide, "clicked");
        for (size_t i = 0; i < 2U; ++i) { OK(umi_chart_drawing_registry_at(registry, i, &actual)); CHECK(actual.visibility_flags == 1U); }
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(objects))) == 2U);
        g_signal_emit_by_name(show, "clicked");
        for (size_t i = 0; i < 2U; ++i) { OK(umi_chart_drawing_registry_at(registry, i, &actual)); CHECK(actual.visibility_flags == 0U); }
    } else if (strcmp(name, "stale-row") == 0 || strcmp(name, "stale-pane") == 0) {
        if (strcmp(name, "stale-row") == 0) {
            drawing.locked = 1; OK(umi_chart_drawing_registry_upsert(registry, &drawing));
        } else OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "ray", (UmiChartPoint){60000, 100}, (UmiChartPoint){120000, 105}));
        uint64_t revision = umi_chart_drawing_registry_revision(registry);
        g_signal_emit_by_name(strcmp(name, "stale-row") == 0 ? toggle : hide, "clicked");
        CHECK(umi_chart_drawing_registry_revision(registry) == revision);
        for (size_t i = 0; i < umi_chart_drawing_registry_count(registry); ++i) {
            OK(umi_chart_drawing_registry_at(registry, i, &actual)); CHECK(actual.visibility_flags == 0U);
        }
    } else if (strcmp(name, "instrument-change") == 0) {
        UmiInstrument other = test_instrument();
        OK(umi_trading_workspace_select_instrument(f.workspace, other.instrument_id.value));
        OK(umi_trading_workspace_snapshot(f.workspace, &before));
        g_signal_emit_by_name(hide, "clicked");
        OK(umi_chart_drawing_registry_at(registry, 0, &actual)); CHECK(actual.visibility_flags == 0U && actual.revision == drawing.revision);
    } else if (strcmp(name, "retained") == 0) {
        g_object_ref(toggle); g_object_ref(hide); g_object_ref(show);
        UmiGtk4TradingInteractiveChartDetach(root);
        g_signal_emit_by_name(toggle, "clicked"); g_signal_emit_by_name(hide, "clicked"); g_signal_emit_by_name(show, "clicked");
        g_object_unref(root); root = NULL;
        g_signal_emit_by_name(toggle, "clicked"); g_signal_emit_by_name(hide, "clicked"); g_signal_emit_by_name(show, "clicked");
        OK(umi_chart_drawing_registry_at(registry, 0, &actual)); CHECK(actual.visibility_flags == 0U && actual.revision == drawing.revision);
        g_object_unref(toggle); g_object_unref(hide); g_object_unref(show);
    } else return 2;
    OK(umi_trading_workspace_snapshot(f.workspace, &after));
    CHECK(after.order_count == before.order_count && after.live_armed == before.live_armed && after.environment == before.environment);
    CHECK(memcmp(&after.draft_order, &before.draft_order, sizeof before.draft_order) == 0);
    if (root != NULL) { UmiGtk4TradingInteractiveChartDetach(root); g_object_unref(root); }
    umi_trading_workspace_destroy(f.workspace); return 0;
}
