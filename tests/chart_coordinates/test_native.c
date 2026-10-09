/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_coordinates/test_native.c
 * PURPOSE: Drive precise drawing edits through real controls and assert order isolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Drawing undo/redo and its snapshot are declared by the shared history
 * contract; do not depend on another chart header including it incidentally. */
#include "umicom/ui/gtk4/automation.h"
#include "umicom/trading/chart_history.h"
#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#include "umicom/chart/drawing_appearance.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x) == UMI_STATUS_OK)
/* The rendered-child walk omitted controls owned by collapsed expanders. The shared bounded logical-tree lookup replaces it; retain the earlier traversal for review. */
#if 0
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
#endif
/* Use the Framework logical tree so a collapsed panel can be inspected
 * without changing the user's layout or overlooking an ambiguous identifier. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(root, id);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    const char *cases[] = {"apply", "undo", "no-op", "precision", "support", "resistance", "invalid-time", "overflow-time", "invalid-price", "comma-price", "overflow-price", "underflow-price", "empty", "zero-area", "stale", "selection", "instrument", "locked", "no-load", "draft-refresh", "retained", "level-no-op"};
    int known = 0;
    for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index) if (strcmp(name, cases[index]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingWorkspaceSnapshot before, after; OK(umi_trading_workspace_snapshot(f.workspace, &before));
    const char *pane = before.selected_instrument_id;
    int level = strcmp(name, "support") == 0 || strcmp(name, "resistance") == 0 || strcmp(name, "level-no-op") == 0;
    OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, pane, level ? (strcmp(name, "resistance") == 0 ? "resistance" : "support") : "range",
        (UmiChartPoint){60000,100}, (UmiChartPoint){120000,105}));
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
    UmiChartDrawingSnapshot drawing, current; OK(umi_chart_drawing_registry_at(registry, 0U, &drawing));
    if (strcmp(name, "locked") == 0) {
        drawing.locked = 1; OK(umi_chart_drawing_registry_upsert(registry, &drawing));
        OK(umi_chart_drawing_registry_at(registry, 0U, &drawing));
    }
    UmiChartDrawingHistorySnapshot baselineHistory, history;
    OK(UmiTradingWorkspaceDrawingHistory(f.workspace, &baselineHistory));
    UmiGtk4TradingPanelContext context = {f.workspace, &f.controller, 0};
    GtkWidget *root = UmiGtk4TradingInteractiveChartCreate(&context); CHECK(root != NULL); g_object_ref_sink(root);
    GtkWidget *load = Find(root, "trading.chart.coordinates-load"), *apply = Find(root, "trading.chart.coordinates-apply");
    GtkWidget *firstTime = Find(root, "trading.chart.coordinates-time-first"), *firstPrice = Find(root, "trading.chart.coordinates-price-first");
    GtkWidget *secondTime = Find(root, "trading.chart.coordinates-time-second"), *secondPrice = Find(root, "trading.chart.coordinates-price-second");
    GtkWidget *objects = Find(root, "trading.chart.drawings");
    CHECK(GTK_IS_BUTTON(load) && GTK_IS_BUTTON(apply) && GTK_IS_ENTRY(firstTime) && GTK_IS_ENTRY(firstPrice));
    CHECK(GTK_IS_ENTRY(secondTime) && GTK_IS_ENTRY(secondPrice) && GTK_IS_DROP_DOWN(objects));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(objects), 0U);
    if (strcmp(name, "no-load") != 0) g_signal_emit_by_name(load, "clicked");
    if (strcmp(name, "locked") == 0 || strcmp(name, "no-load") == 0) CHECK(!gtk_widget_is_sensitive(apply));
    else CHECK(gtk_widget_is_sensitive(apply));
    if (level) CHECK(!gtk_widget_is_sensitive(secondTime) && !gtk_widget_is_sensitive(secondPrice));
    int changes = strcmp(name, "apply") == 0 || strcmp(name, "undo") == 0 ||
        strcmp(name, "precision") == 0 || strcmp(name, "draft-refresh") == 0 || (level && strcmp(name, "level-no-op") != 0);
    int64_t expectedTime = strcmp(name, "precision") == 0 ? INT64_C(9007199254740993) : 70000;
    if (strcmp(name, "no-op") != 0 && strcmp(name, "level-no-op") != 0) {
        gtk_editable_set_text(GTK_EDITABLE(firstTime), strcmp(name, "precision") == 0 ? "9007199254740993" : "70000");
        gtk_editable_set_text(GTK_EDITABLE(firstPrice), "1.025e2");
        gtk_editable_set_text(GTK_EDITABLE(secondTime), strcmp(name, "precision") == 0 ? "9007199254741993" : "130000");
        gtk_editable_set_text(GTK_EDITABLE(secondPrice), "107.5");
    }
    if (strcmp(name, "invalid-time") == 0) gtk_editable_set_text(GTK_EDITABLE(firstTime), "-1");
    else if (strcmp(name, "overflow-time") == 0) gtk_editable_set_text(GTK_EDITABLE(firstTime), "9223372036854775808");
    else if (strcmp(name, "invalid-price") == 0) gtk_editable_set_text(GTK_EDITABLE(firstPrice), "nan");
    else if (strcmp(name, "comma-price") == 0) gtk_editable_set_text(GTK_EDITABLE(firstPrice), "102,5");
    else if (strcmp(name, "overflow-price") == 0) gtk_editable_set_text(GTK_EDITABLE(firstPrice), "1e999");
    else if (strcmp(name, "underflow-price") == 0) gtk_editable_set_text(GTK_EDITABLE(firstPrice), "1e-999");
    else if (strcmp(name, "empty") == 0) gtk_editable_set_text(GTK_EDITABLE(firstTime), "");
    else if (strcmp(name, "zero-area") == 0) gtk_editable_set_text(GTK_EDITABLE(secondPrice), "102.5");
    else if (strcmp(name, "stale") == 0) {
        OK(UmiTradingWorkspaceMoveChartDrawing(f.workspace, pane, drawing.id, drawing.revision,
            (UmiChartPoint){80000,99}, (UmiChartPoint){140000,108}));
        OK(umi_chart_drawing_registry_at(registry, 0U, &drawing));
        UmiGtk4TradingInteractiveChartRefresh(root);
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace, &baselineHistory));
    } else if (strcmp(name, "selection") == 0) {
        OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, pane, "range", (UmiChartPoint){180000,100}, (UmiChartPoint){240000,105}));
        UmiGtk4TradingInteractiveChartRefresh(root); gtk_drop_down_set_selected(GTK_DROP_DOWN(objects), 1U);
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace, &baselineHistory));
    } else if (strcmp(name, "instrument") == 0) {
        UmiInstrument other = test_instrument(); OK(umi_trading_workspace_select_instrument(f.workspace, other.instrument_id.value));
        OK(umi_trading_workspace_snapshot(f.workspace, &before));
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace, &baselineHistory));
    } else if (strcmp(name, "draft-refresh") == 0) {
        UmiGtk4TradingInteractiveChartRefresh(root); UmiGtk4TradingInteractiveChartRefresh(root);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(firstTime)), "70000") == 0);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(firstPrice)), "1.025e2") == 0);
    }
    if (strcmp(name, "retained") == 0) {
        g_object_ref(load); g_object_ref(apply); UmiGtk4TradingInteractiveChartDetach(root);
        g_signal_emit_by_name(load, "clicked"); g_signal_emit_by_name(apply, "clicked");
        g_object_unref(root); root = NULL;
        g_signal_emit_by_name(load, "clicked"); g_signal_emit_by_name(apply, "clicked");
        g_object_unref(load); g_object_unref(apply);
    } else g_signal_emit_by_name(apply, "clicked");
    OK(umi_chart_drawing_registry_find(registry, drawing.id, &current));
    OK(UmiTradingWorkspaceDrawingHistory(f.workspace, &history));
    if (changes) {
        CHECK(current.time1 == expectedTime && current.value1 == 102.5);
        CHECK(current.time2 == (level ? drawing.time2 : expectedTime + 60000) ||
            (strcmp(name, "precision") == 0 && current.time2 == expectedTime + 1000));
        CHECK(current.value2 == (level ? 102.5 : 107.5));
        CHECK(history.undo_count == baselineHistory.undo_count + 1U);
        CHECK(strcmp(current.style, drawing.style) == 0 && strcmp(current.id, drawing.id) == 0);
        if (strcmp(name, "precision") == 0) CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(firstTime)), "9007199254740993") == 0);
        if (strcmp(name, "undo") == 0) {
            OK(UmiTradingWorkspaceUndoDrawing(f.workspace, pane, history.revision));
            OK(umi_chart_drawing_registry_find(registry, drawing.id, &current));
            CHECK(current.time1 == drawing.time1 && current.value1 == drawing.value1 && current.time2 == drawing.time2 && current.value2 == drawing.value2);
        }
    } else {
        CHECK(current.time1 == drawing.time1 && current.time2 == drawing.time2 && current.value1 == drawing.value1 && current.value2 == drawing.value2);
        CHECK(current.revision == drawing.revision && history.revision == baselineHistory.revision);
    }
    OK(umi_trading_workspace_snapshot(f.workspace, &after));
    CHECK(after.order_count == before.order_count && after.environment == before.environment && after.live_armed == before.live_armed);
    CHECK(memcmp(&after.draft_order, &before.draft_order, sizeof before.draft_order) == 0);
    if (root != NULL) { UmiGtk4TradingInteractiveChartDetach(root); g_object_unref(root); }
    umi_trading_workspace_destroy(f.workspace); return 0;
}
