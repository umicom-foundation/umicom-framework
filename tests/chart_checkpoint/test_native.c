/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_checkpoint/test_native.c
 * PURPOSE: Exercise actual chart persistence controls and retained-widget teardown.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "fixture.h"
#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#include "umicom/trading_ui/gtk4/trading_suite_workstation.h"
/* The chart inspector owns controls while collapsed. Shared logical lookup replaces the rendered-child walk, retained here for review. The previous implementation is retained for engineering review. */
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
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    /* Read controls owned by collapsed chart inspectors without changing layout. */
    return umi_gtk4_automation_find_tagged_widget(root, id);
}
static void Click(GtkWidget *root, const char *id)
{ GtkWidget *button = Find(root, id); CHECK(GTK_IS_BUTTON(button)); g_signal_emit_by_name(button, "clicked"); }
int main(int argc, char **argv)
{
    CHECK(argc == 2); if (!gtk_init_check()) return 77;
    ReviewFixture fixture; ReviewFixtureInit(&fixture);
    UmiTradingMarketSnapshot market; CHECK(umi_trading_workspace_selected_market(fixture.workspace, &market) == UMI_STATUS_OK);
    const char *id = market.instrument.instrument_id.value;
    for (int i = 0; i < 40; ++i) {
        UmiBar bar = {0}; bar.instrument = market.instrument;
        bar.start_time_ms = 60000 * (i + 1); bar.end_time_ms = bar.start_time_ms + 59999;
        bar.open = 100; bar.close = 101; bar.low = 90; bar.high = 110; bar.volume = 100;
        CHECK(umi_trading_workspace_update_bar(fixture.workspace, &bar, 100) == UMI_STATUS_OK);
    }
    UmiDataServer *server = NULL; CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UmiGtk4TradingSuiteWorkstation *suite = NULL;
    UmiGtk4TradingSuiteWorkstationConfig config = umi_gtk4_trading_suite_workstation_config_default(fixture.workspace);
    config.seed_simulation_market = 0; config.animate_simulation_market = 0;
    CHECK(umi_gtk4_trading_suite_workstation_create(&config, &suite) == UMI_STATUS_OK);
    CHECK(UmiGtk4TradingSuiteBindChartStorage(suite, server, "test") == UMI_STATUS_OK);
    GtkWidget *window = gtk_window_new(); g_object_ref_sink(window);
    GtkWidget *root = umi_gtk4_trading_suite_workstation_widget(suite);
    gtk_window_set_child(GTK_WINDOW(window), root);
    GtkWidget *chart = Find(root, "trading.chart.panel"); CHECK(chart != NULL);
    GtkWidget *section = Find(chart, "trading.chart.saved-review");
    CHECK(GTK_IS_EXPANDER(section) && !gtk_expander_get_expanded(GTK_EXPANDER(section)));
    GtkWidget *restore = Find(chart, "trading.chart.restore"); CHECK(restore != NULL && !gtk_widget_get_sensitive(restore));
    Click(chart, "trading.chart.save");
    UmiChartDocument *saved = NULL; UmiChartCheckpointReport report;
    CHECK(UmiChartCheckpointLoad(server, "test", id, &saved, &report) == UMI_STATUS_OK && report.storage_revision == 1U);
    UmiChartDocumentDestroy(saved); saved = NULL;
    Click(chart, "trading.chart.zoom-in"); Click(chart, "trading.chart.preview");
    /* Preview opens the existing review surface, making its text reachable.
     * It must not require a second, undocumented click to reveal evidence. */
    CHECK(gtk_expander_get_expanded(GTK_EXPANDER(section)));
    CHECK(gtk_widget_get_sensitive(restore));
    GtkWidget *details = Find(chart, "trading.chart.saved-details"); CHECK(GTK_IS_TEXT_VIEW(details));
    GtkTextIter start, end; GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(details));
    gtk_text_buffer_get_bounds(buffer, &start, &end); char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    CHECK(strstr(text, id) != NULL && strstr(text, "memory only") != NULL); g_free(text);
    /* Reopening a collapsed review keeps the same controls and copied text
     * owner. A repeated preview still requires an explicit Restore command. */
    gtk_expander_set_expanded(GTK_EXPANDER(section), FALSE);
    Click(chart, "trading.chart.preview");
    CHECK(gtk_expander_get_expanded(GTK_EXPANDER(section)));
    CHECK(Find(chart, "trading.chart.saved-details") == details);
    UmiChartNavigation before_restore;
    CHECK(UmiTradingWorkspaceGetChartNavigation(fixture.workspace, id, &before_restore) == UMI_STATUS_OK);
    CHECK(before_restore.visible_bars != 0U);

    if (strcmp(argv[1], "restore") == 0) {
        Click(chart, "trading.chart.restore"); UmiChartNavigation navigation;
        CHECK(UmiTradingWorkspaceGetChartNavigation(fixture.workspace, id, &navigation) == UMI_STATUS_OK && navigation.visible_bars == 0U);
        CHECK(!gtk_widget_get_sensitive(restore));
    } else if (strcmp(argv[1], "selection") == 0) {
        UmiInstrument other = test_instrument();
        CHECK(umi_trading_workspace_select_instrument(fixture.workspace, other.instrument_id.value) == UMI_STATUS_OK);
        Click(chart, "trading.chart.restore");
        UmiChartDocument *live = NULL; UmiChartDocumentSummary summary;
        CHECK(UmiTradingWorkspaceCaptureChart(fixture.workspace, id, &live) == UMI_STATUS_OK);
        CHECK(UmiChartDocumentGetSummary(live, &summary) == UMI_STATUS_OK && summary.navigation.visible_bars != 0U);
        UmiChartDocumentDestroy(live);
    } else if (strcmp(argv[1], "unbind") == 0) {
        CHECK(UmiGtk4TradingSuiteBindChartStorage(suite, NULL, NULL) == UMI_STATUS_OK);
        CHECK(!gtk_widget_get_sensitive(restore)); Click(chart, "trading.chart.save");
        CHECK(UmiChartCheckpointLoad(server, "test", id, &saved, &report) == UMI_STATUS_OK && report.storage_revision == 1U);
        UmiChartDocumentDestroy(saved);
    } else {
        CHECK(strcmp(argv[1], "retained-root") == 0 || strcmp(argv[1], "retained-body") == 0);
        GtkWidget *retained = chart;
        if (strcmp(argv[1], "retained-body") == 0) {
            /* Finalizing a provider mount must detach an independently retained
             * chart body even before the entire suite closes. */
            UmiGtk4TradingPanelContext context = {fixture.workspace, &fixture.controller, 0};
            UmiUiWorkspaceWindow spec = {0}; strcpy(spec.tool_id, "chart");
            GtkWidget *mount = umi_gtk4_trading_panel_create(&spec, &context); CHECK(mount != NULL); g_object_ref_sink(mount);
            UmiGtk4TradingPanelBindChartPersistence(mount, UmiGtk4TradingSuiteChartPersistence(suite));
            retained = Find(mount, "trading.chart.panel"); CHECK(retained != NULL); g_object_ref(retained);
            g_object_unref(mount); CHECK(!gtk_widget_get_sensitive(retained));
        } else g_object_ref(retained);
        umi_gtk4_trading_suite_workstation_destroy(suite); suite = NULL;
        umi_trading_workspace_destroy(fixture.workspace); fixture.workspace = NULL;
        Click(retained, "trading.chart.save"); Click(retained, "trading.chart.restore"); Click(retained, "trading.chart.zoom-in");
        CHECK(!gtk_widget_get_sensitive(retained));
        /* A retained chart used to keep its timer pointing into the old owner. */
        g_usleep(1100000); unsigned int turns = 0U;
        while (g_main_context_pending(NULL) && turns++ < 256U) (void)g_main_context_iteration(NULL, FALSE);
        CHECK(turns < 256U); g_object_unref(retained);
    }
    if (suite != NULL) umi_gtk4_trading_suite_workstation_destroy(suite);
    gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(window);
    if (fixture.workspace != NULL) umi_trading_workspace_destroy(fixture.workspace);
    /* The suite borrows this server, so the owner still has valid access. */
    CHECK(umi_data_server_count(server) > 0U); umi_data_server_destroy(server); return 0;
}
