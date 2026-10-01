/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_appearance/test_native.c
 * PURPOSE: Drive real native appearance controls, stale drafts and teardown boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#include "umicom/chart/drawing_appearance.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2); if (!gtk_init_check()) return 77;
    const char *name = argv[1]; ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingWorkspaceSnapshot before, after; OK(umi_trading_workspace_snapshot(f.workspace, &before));
    const char *pane = before.selected_instrument_id;
    OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, pane, "range", (UmiChartPoint){60000,100}, (UmiChartPoint){120000,105}));
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
    UmiChartDrawingSnapshot drawing, current; OK(umi_chart_drawing_registry_at(registry, 0, &drawing));
    if (strcmp(name, "invalid-utf8-id") == 0) {
        OK(umi_chart_drawing_registry_remove(registry, drawing.id));
        drawing.id[0] = (char)0xff;
        OK(umi_chart_drawing_registry_upsert(registry, &drawing));
    } else if (strcmp(name, "legacy") == 0) {
        strcpy(drawing.style, "unrecognised-style"); OK(umi_chart_drawing_registry_upsert(registry, &drawing));
    } else if (strcmp(name, "locked-hidden") == 0) {
        drawing.locked = 1; drawing.visibility_flags = 1; OK(umi_chart_drawing_registry_upsert(registry, &drawing));
    }
    UmiGtk4TradingPanelContext context = {f.workspace, &f.controller, 0};
    GtkWidget *root = UmiGtk4TradingInteractiveChartCreate(&context); CHECK(root != NULL); g_object_ref_sink(root);
    GtkWidget *load = Find(root, "trading.chart.appearance-load"), *apply = Find(root, "trading.chart.appearance-apply");
    GtkWidget *defaults = Find(root, "trading.chart.appearance-defaults"), *color = Find(root, "trading.chart.appearance-color");
    GtkWidget *width = Find(root, "trading.chart.appearance-width"), *fill = Find(root, "trading.chart.appearance-fill");
    GtkWidget *objects = Find(root, "trading.chart.drawings"), *note = Find(root, "trading.chart.appearance-note");
    CHECK(GTK_IS_BUTTON(load) && GTK_IS_BUTTON(apply) && GTK_IS_BUTTON(defaults) && GTK_IS_ENTRY(color));
    CHECK(GTK_IS_SPIN_BUTTON(width) && GTK_IS_SPIN_BUTTON(fill) && GTK_IS_LABEL(note));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(objects), 0);
    if (strcmp(name, "no-load") == 0) {
        CHECK(!gtk_widget_is_sensitive(apply));
        g_signal_emit_by_name(apply, "clicked");
        OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] == '\0');
    } else {
        g_signal_emit_by_name(load, "clicked");
        CHECK(gtk_widget_is_sensitive(apply));
        if (strcmp(name, "invalid-utf8-id") == 0)
            CHECK(g_utf8_validate(gtk_label_get_text(GTK_LABEL(note)), -1, NULL));
        if (strcmp(name, "legacy") == 0) {
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "explicitly replaces") != NULL);
            OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(strcmp(current.style, "unrecognised-style") == 0);
        }
        gtk_editable_set_text(GTK_EDITABLE(color), "#33AAFF");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(width), 2.5);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(fill), 30);
        if (strcmp(name, "invalid") == 0) {
            gtk_editable_set_text(GTK_EDITABLE(color), "#zzzzzz");
            g_signal_emit_by_name(apply, "clicked");
            OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] == '\0');
        } else if (strcmp(name, "stale") == 0) {
            drawing.locked = 1; OK(umi_chart_drawing_registry_upsert(registry, &drawing));
            UmiGtk4TradingInteractiveChartRefresh(root);
            g_signal_emit_by_name(apply, "clicked");
            OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] == '\0' && current.locked);
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(color)), "#33AAFF") == 0);
            g_signal_emit_by_name(load, "clicked"); g_signal_emit_by_name(apply, "clicked");
            OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] != '\0');
        } else if (strcmp(name, "selection") == 0) {
            OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, pane, "range", (UmiChartPoint){180000,100}, (UmiChartPoint){240000,105}));
            UmiGtk4TradingInteractiveChartRefresh(root); gtk_drop_down_set_selected(GTK_DROP_DOWN(objects), 1);
            g_signal_emit_by_name(apply, "clicked");
            for (size_t i = 0; i < 2; ++i) { OK(umi_chart_drawing_registry_at(registry, i, &current)); CHECK(current.style[0] == '\0'); }
        } else if (strcmp(name, "instrument") == 0) {
            UmiInstrument other = test_instrument(); OK(umi_trading_workspace_select_instrument(f.workspace, other.instrument_id.value));
            OK(umi_trading_workspace_snapshot(f.workspace, &before));
            g_signal_emit_by_name(apply, "clicked");
            OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] == '\0');
        } else if (strcmp(name, "retained") == 0) {
            g_object_ref(load); g_object_ref(apply); g_object_ref(defaults);
            UmiGtk4TradingInteractiveChartDetach(root);
            g_signal_emit_by_name(load, "clicked"); g_signal_emit_by_name(apply, "clicked"); g_signal_emit_by_name(defaults, "clicked");
            g_object_unref(root); root = NULL;
            g_signal_emit_by_name(load, "clicked"); g_signal_emit_by_name(apply, "clicked"); g_signal_emit_by_name(defaults, "clicked");
            OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] == '\0');
            g_object_unref(load); g_object_unref(apply); g_object_unref(defaults);
        } else if (strcmp(name, "apply") == 0 || strcmp(name, "defaults") == 0 ||
            strcmp(name, "draft-refresh") == 0 || strcmp(name, "legacy") == 0 || strcmp(name, "locked-hidden") == 0 || strcmp(name, "invalid-utf8-id") == 0) {
            if (strcmp(name, "draft-refresh") == 0) {
                UmiGtk4TradingInteractiveChartRefresh(root); UmiGtk4TradingInteractiveChartRefresh(root);
                CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(color)), "#33AAFF") == 0);
                CHECK(gtk_spin_button_get_value(GTK_SPIN_BUTTON(width)) == 2.5);
                OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] == '\0');
            }
            g_signal_emit_by_name(apply, "clicked");
            OK(umi_chart_drawing_registry_at(registry, 0, &current));
            CHECK(strcmp(current.style, "umi-drawing:1:33AAFF:25:30") == 0);
            if (strcmp(name, "locked-hidden") == 0) CHECK(current.locked && current.visibility_flags == 1);
            if (strcmp(name, "defaults") == 0) {
                g_signal_emit_by_name(defaults, "clicked");
                OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] == '\0');
            }
        } else return 2;
    }
    OK(umi_trading_workspace_snapshot(f.workspace, &after));
    CHECK(after.order_count == before.order_count && after.environment == before.environment && after.live_armed == before.live_armed);
    CHECK(memcmp(&after.draft_order, &before.draft_order, sizeof before.draft_order) == 0);
    if (root != NULL) { UmiGtk4TradingInteractiveChartDetach(root); g_object_unref(root); }
    umi_trading_workspace_destroy(f.workspace); return 0;
}
