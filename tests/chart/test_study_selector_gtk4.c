/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_study_selector_gtk4.c
 * PURPOSE: Drive the native study selector while proving that chart-only changes preserve trading state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_timeframe/fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag && strcmp(tag, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found)
            return found;
    }
    return NULL;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check())
        return 77;
    ReviewFixture fixture;
    ReviewFixtureInit(&fixture);
    TimeframeBars(&fixture, 40U);
    UmiTradingWorkspaceSnapshot before, after;
    OK(umi_trading_workspace_snapshot(fixture.workspace, &before));
    UmiGtk4TradingPanelContext context = {fixture.workspace, &fixture.controller, 0};
    GtkWidget *root = UmiGtk4TradingInteractiveChartCreate(&context);
    CHECK(root);
    g_object_ref_sink(root);
    GtkWidget *selector = Find(root, "trading.chart.study");
    CHECK(selector);
    guint selected;
    if (strcmp(argv[1], "weighted") == 0)
        selected = (guint)UMI_TRADING_CHART_STUDY_VOLUME_WEIGHTED;
    else if (strcmp(argv[1], "bollinger") == 0)
        selected = (guint)UMI_TRADING_CHART_STUDY_BOLLINGER;
    else if (strcmp(argv[1], "donchian") == 0)
        selected = (guint)UMI_TRADING_CHART_STUDY_DONCHIAN;
    else
    {
        CHECK(strcmp(argv[1], "profile") == 0 || strcmp(argv[1], "retained") == 0 ||
              strcmp(argv[1], "detached") == 0);
        selected = (guint)UMI_TRADING_CHART_STUDY_VOLUME_PROFILE;
    }
    CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(selector))) == 7U);
    if (strcmp(argv[1], "retained") == 0)
    {
        g_object_ref(selector);
        g_object_unref(root);
        root = NULL;
    }
    if (strcmp(argv[1], "detached") == 0)
        UmiGtk4TradingInteractiveChartDetach(root);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(selector), selected);
    OK(umi_trading_workspace_snapshot(fixture.workspace, &after));
    CHECK(after.chart_study == ((strcmp(argv[1], "retained") == 0 || strcmp(argv[1], "detached") == 0)
                                    ? before.chart_study
                                    : (UmiTradingChartStudy)selected));
    SameTrading(fixture.workspace, &before);
    if (root)
        g_object_unref(root);
    else
        g_object_unref(selector);
    umi_trading_workspace_destroy(fixture.workspace);
    return 0;
}
