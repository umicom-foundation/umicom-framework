/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_chart_native.c
 * PURPOSE: Exercise native chart controls, coordinate gestures and widget replacement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "order_review_fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#define CHECK REVIEW_CHECK
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static void ClickPrice(GtkWidget *root, double fraction_x, double fraction_y)
{
    GtkWidget *area = Find(root, "trading.chart.canvas"); CHECK(area != NULL);
    gtk_widget_allocate(root, 1200, 800, -1, NULL);
    int width = gtk_widget_get_width(area), height = gtk_widget_get_height(area);
    CHECK(width > 0 && height > 0);
    GListModel *controllers = gtk_widget_observe_controllers(area);
    GtkGestureClick *gesture = NULL;
    for (guint i = 0; i < g_list_model_get_n_items(controllers); ++i) {
        GObject *item = g_list_model_get_item(controllers, i);
        if (GTK_IS_GESTURE_CLICK(item)) { gesture = GTK_GESTURE_CLICK(item); break; }
        g_object_unref(item);
    }
    CHECK(gesture != NULL);
    g_signal_emit_by_name(gesture, "pressed", 1, width * fraction_x, height * fraction_y);
    g_object_unref(gesture); g_object_unref(controllers);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2); if (!gtk_init_check()) return 77;
    ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingMarketSnapshot market;
    CHECK(umi_trading_workspace_selected_market(f.workspace, &market) == UMI_STATUS_OK);
    for (int i = 0; i < 40; ++i) {
        UmiBar bar = {0}; bar.instrument = market.instrument;
        bar.start_time_ms = 60000 * (i + 1); bar.end_time_ms = bar.start_time_ms + 59999;
        bar.open = 100; bar.close = 101; bar.low = 90; bar.high = 110; bar.volume = 100;
        CHECK(umi_trading_workspace_update_bar(f.workspace, &bar, 100) == UMI_STATUS_OK);
    }
    UmiGtk4TradingPanelContext context = {f.workspace, &f.controller, 0};
    UmiUiWorkspaceWindow window = {0}; strcpy(window.tool_id, "chart");
    GtkWidget *root = umi_gtk4_trading_panel_create(&window, &context); CHECK(root != NULL); g_object_ref_sink(root);
    const char *id = market.instrument.instrument_id.value;
    UmiChartNavigation navigation;
    if (strcmp(argv[1], "zoom") == 0) {
        g_signal_emit_by_name(Find(root, "trading.chart.zoom-in"), "clicked");
        CHECK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation) == UMI_STATUS_OK && navigation.visible_bars == 30);
        g_signal_emit_by_name(Find(root, "trading.chart.earlier"), "clicked");
        CHECK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation) == UMI_STATUS_OK && navigation.pinned);
        g_signal_emit_by_name(Find(root, "trading.chart.fit"), "clicked");
        CHECK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation) == UMI_STATUS_OK && !navigation.pinned && navigation.visible_bars == 0);
    } else if (strcmp(argv[1], "refresh") == 0) {
        GtkWidget *canvas = Find(root, "trading.chart.canvas");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Find(root, "trading.chart.trend")), TRUE);
        ClickPrice(root, 0.2, 0.3);
        CHECK(UmiGtk4TradingPanelRefresh(root, 0) == UMI_STATUS_OK);
        CHECK(Find(root, "trading.chart.canvas") == canvas);
        ClickPrice(root, 0.7, 0.4);
        CHECK(umi_chart_drawing_registry_count(umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace))) == 1);
    } else if (strcmp(argv[1], "drawings") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Find(root, "trading.chart.support")), TRUE);
        ClickPrice(root, 0.5, 0.9); /* Volume pane must not create price drawings. */
        UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
        CHECK(umi_chart_drawing_registry_count(registry) == 0);
        ClickPrice(root, 0.5, 0.3); CHECK(umi_chart_drawing_registry_count(registry) == 1);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Find(root, "trading.chart.trend")), TRUE);
        ClickPrice(root, 0.2, 0.3); CHECK(umi_chart_drawing_registry_count(registry) == 1);
        ClickPrice(root, 0.7, 0.4); CHECK(umi_chart_drawing_registry_count(registry) == 2);
        g_object_unref(root); root = UmiGtk4TradingInteractiveChartCreate(&context); g_object_ref_sink(root);
        GtkWidget *objects = Find(root, "trading.chart.drawings");
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(objects))) == 2);
        g_signal_emit_by_name(Find(root, "trading.chart.remove-drawing"), "clicked");
        CHECK(umi_chart_drawing_registry_count(registry) == 1);
    } else if (strcmp(argv[1], "ticket") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Find(root, "trading.chart.sell-limit")), TRUE);
        UmiTradingWorkspaceSnapshot before, after;
        CHECK(umi_trading_workspace_snapshot(f.workspace, &before) == UMI_STATUS_OK);
        ClickPrice(root, 0.5, 0.3);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK);
        CHECK(after.order_count == before.order_count && after.draft_order.side == UMI_SIDE_SELL && !after.has_draft_risk);
        CHECK(after.draft_order.limit_price > 90 && after.draft_order.limit_price < 110);
        UmiInstrument other = test_instrument(); CHECK(umi_trading_workspace_select_instrument(f.workspace, other.instrument_id.value) == UMI_STATUS_OK);
        ClickPrice(root, 0.5, 0.3);
        CHECK(umi_trading_ui_controller_snapshot(&f.controller).last_status == UMI_STATUS_INVALID_STATE);
    } else {
        CHECK(strcmp(argv[1], "retained-widget") == 0);
        GtkWidget *button = Find(root, "trading.chart.zoom-in"); GtkWidget *area = Find(root, "trading.chart.canvas");
        g_object_ref(button); g_object_ref(area); g_object_unref(root); root = NULL;
        g_signal_emit_by_name(button, "clicked");
        CHECK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation) == UMI_STATUS_OK && navigation.visible_bars == 0);
        g_object_unref(area); g_object_unref(button);
    }
    if (root != NULL) g_object_unref(root);
    umi_trading_workspace_destroy(f.workspace); return 0;
}
