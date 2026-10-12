/*-----------------------------------------------------------------------------
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 *
 * LICENCE: MIT
 *
 * Umicom Framework
 * File: tests/chart/test_candle_inspection_gtk4.c
 * PURPOSE: Exercise the real chart data inspector, refresh, gap labels and detached-control
 * lifetime. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_timeframe/fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#include "umicom/ui/gtk4/automation.h"
static GtkWidget *Find(GtkWidget *root, const char *tag) {
  return umi_gtk4_automation_find_tagged_widget(root, tag);
}
/* Retain the actual controller, just as a native event system may retain an
 * event until after its old widget has left the product layout. */
static GtkEventController *Motion(GtkWidget *root) {
  GtkWidget *area = Find(root, "trading.chart.canvas");
  CHECK(area);
  GListModel *controllers = gtk_widget_observe_controllers(area);
  GtkEventController *result = NULL;
  for (guint index = 0U; index < g_list_model_get_n_items(controllers); ++index) {
    GObject *item = g_list_model_get_item(controllers, index);
    if (GTK_IS_EVENT_CONTROLLER_MOTION(item)) {
      result = GTK_EVENT_CONTROLLER(item);
      break;
    }
    g_object_unref(item);
  }
  g_object_unref(controllers);
  CHECK(result);
  return result;
}
static void Point(GtkWidget *root, GtkEventController *motion, double x, double y) {
  gtk_widget_allocate(root, 1200, 800, -1, NULL);
  GtkWidget *area = Find(root, "trading.chart.canvas");
  CHECK(area && gtk_widget_get_width(area) > 0 && gtk_widget_get_height(area) > 0);
  g_signal_emit_by_name(motion, "motion", x * gtk_widget_get_width(area) / UMI_TRADING_CHART_WIDTH,
                        y * gtk_widget_get_height(area) / UMI_TRADING_CHART_HEIGHT);
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!gtk_init_check())
    return 77;
  ReviewFixture fixture;
  ReviewFixtureInit(&fixture);
  TimeframeBars(&fixture, 3U);
  UmiTradingWorkspaceSnapshot before;
  OK(umi_trading_workspace_snapshot(fixture.workspace, &before));
  UmiGtk4TradingPanelContext context = {fixture.workspace, &fixture.controller, 0};
  GtkWidget *root = UmiGtk4TradingInteractiveChartCreate(&context);
  CHECK(root);
  g_object_ref_sink(root);
  GtkWidget *label = Find(root, "trading.chart.candle-inspection");
  CHECK(label);
  GtkEventController *motion = Motion(root);
  const char *mode = argv[1];
  if (strcmp(mode, "retained") == 0) {
    g_object_ref(label);
    g_object_unref(root);
    root = NULL;
    char *previous = g_strdup(gtk_label_get_text(GTK_LABEL(label)));
    g_signal_emit_by_name(motion, "motion", 100.0, 100.0);
    CHECK(strcmp(previous, gtk_label_get_text(GTK_LABEL(label))) == 0);
    g_free(previous);
    g_object_unref(label);
  } else {
    /* Three candles at 0, 60 and 120 seconds: the plot midpoint is
     * the second opening time. Gap checks stay away from a rounding tie;
     * the core regression verifies exact integer ties independently. */
    Point(root, motion, 456.0, 100.0);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "O 101  H 103  L 100  C 102  Volume 11"));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "offset "));
    if (strcmp(mode, "gap") == 0) {
      Point(root, motion, 200.0, 100.0);
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "Nearest candle"));
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "offset 0 ms") == NULL);
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "O 100"));
    } else if (strcmp(mode, "outside") == 0) {
      Point(root, motion, 456.0, 500.0);
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "Move across"));
    } else if (strcmp(mode, "leave") == 0) {
      g_signal_emit_by_name(motion, "leave");
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "Move across"));
    } else if (strcmp(mode, "refresh") == 0) {
      Point(root, motion, 880.0, 100.0);
      UmiBar bar;
      OK(umi_trading_workspace_selected_bar_at(fixture.workspace, 2U, &bar));
      bar.close = 103.5;
      OK(umi_trading_workspace_update_bar(fixture.workspace, &bar, 101));
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "C 103  Volume"));
      UmiGtk4TradingInteractiveChartRefresh(root);
      CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "C 103.5  Volume"));
    } else if (strcmp(mode, "detached") == 0) {
      char *previous = g_strdup(gtk_label_get_text(GTK_LABEL(label)));
      UmiGtk4TradingInteractiveChartDetach(root);
      Point(root, motion, 237.0, 100.0);
      CHECK(strcmp(previous, gtk_label_get_text(GTK_LABEL(label))) == 0);
      g_free(previous);
    } else
      CHECK(strcmp(mode, "values") == 0);
  }
  SameTrading(fixture.workspace, &before);
  g_object_unref(motion);
  if (root)
    g_object_unref(root);
  umi_trading_workspace_destroy(fixture.workspace);
  return 0;
}
