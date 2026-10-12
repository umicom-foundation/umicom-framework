/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_candle_range_gtk4.c
 * PURPOSE: Exercise chart measurement gestures, keyboard controls and retained-widget ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_timeframe/fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#include "umicom/ui/gtk4/automation.h"
static GtkWidget *Find(GtkWidget *root, const char *tag) {
  return umi_gtk4_automation_find_tagged_widget(root, tag);
}
static GtkEventController *Controller(GtkWidget *root, GType type) {
  GtkWidget *area = Find(root, "trading.chart.canvas");
  CHECK(area);
  GListModel *controllers = gtk_widget_observe_controllers(area);
  GtkEventController *result = NULL;
  for (guint index = 0U; index < g_list_model_get_n_items(controllers); ++index) {
    GObject *item = g_list_model_get_item(controllers, index);
    if (g_type_is_a(G_OBJECT_TYPE(item), type)) {
      result = GTK_EVENT_CONTROLLER(item);
      break;
    }
    g_object_unref(item);
  }
  g_object_unref(controllers);
  CHECK(result);
  return result;
}
static void ClickChart(GtkWidget *root, GtkEventController *click, double x, double y) {
  gtk_widget_allocate(root, 1200, 1100, -1, NULL);
  GtkWidget *area = Find(root, "trading.chart.canvas");
  CHECK(area && gtk_widget_get_width(area) > 0 && gtk_widget_get_height(area) > 0);
  g_signal_emit_by_name(click, "pressed", 1,
                        x * gtk_widget_get_width(area) / UMI_TRADING_CHART_WIDTH,
                        y * gtk_widget_get_height(area) / UMI_TRADING_CHART_HEIGHT);
}
static const char *Result(GtkWidget *root) {
  return gtk_label_get_text(GTK_LABEL(Find(root, "trading.chart.measurement-result")));
}
/* Read the explicit Copy action through the clipboard's public asynchronous
 * API. The bounded wait reports failure rather than treating a queued request
 * as evidence that the expected bytes were copied. */
typedef struct ClipboardRead {
  int done;
  char *text;
  GError *error;
} ClipboardRead;
static void ClipboardReady(GObject *object, GAsyncResult *result, gpointer data) {
  ClipboardRead *read = data;
  read->text = gdk_clipboard_read_text_finish(GDK_CLIPBOARD(object), result, &read->error);
  read->done = 1;
}
static void CheckClipboard(GtkWidget *root) {
  ClipboardRead read = {0};
  GCancellable *cancel = g_cancellable_new();
  gdk_clipboard_read_text_async(gtk_widget_get_clipboard(root), cancel, ClipboardReady, &read);
  gint64 deadline = g_get_monotonic_time() + 2000000;
  while (!read.done && g_get_monotonic_time() < deadline) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000);
  }
  if (!read.done) {
    g_cancellable_cancel(cancel);
    /* Keep callback storage alive until cancellation has delivered its result. */
    while (!read.done)
      g_main_context_iteration(NULL, TRUE);
  }
  CHECK(read.error == NULL && read.text != NULL);
  CHECK(strstr(read.text, "\"summary\"") && strstr(read.text, "\"candle\""));
  CHECK(strstr(read.text, "\"100\",\"104\",\"99\",\"103\",\"33\""));
  g_free(read.text);
  g_clear_error(&read.error);
  g_object_unref(cancel);
}
static void DetachOnExpand(GObject *object, GParamSpec *spec, gpointer root) {
  (void)object;
  (void)spec;
  UmiGtk4TradingInteractiveChartDetach(root);
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
  GtkWidget *visible = Find(root, "trading.chart.measurement-visible");
  GtkWidget *rows = Find(root, "trading.chart.measurement-rows");
  GtkWidget *tool = Find(root, "trading.chart.measure");
  GtkWidget *label = Find(root, "trading.chart.measurement-result");
  GtkWidget *section = Find(root, "trading.chart.measurement");
  CHECK(visible && rows && tool && label && section);
  GtkEventController *click = Controller(root, GTK_TYPE_GESTURE_CLICK);
  const char *mode = argv[1];
  if (strcmp(mode, "retained") == 0) {
    g_object_ref(visible);
    g_object_ref(label);
    g_object_ref(tool);
    char *previous = g_strdup(Result(root));
    g_object_unref(root);
    root = NULL;
    g_signal_emit_by_name(visible, "clicked");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tool), TRUE);
    g_signal_emit_by_name(click, "pressed", 1, 100.0, 100.0);
    CHECK(strcmp(previous, gtk_label_get_text(GTK_LABEL(label))) == 0);
    g_free(previous);
    g_object_unref(visible);
    g_object_unref(label);
    g_object_unref(tool);
  } else if (strcmp(mode, "detached") == 0) {
    char *previous = g_strdup(Result(root));
    UmiGtk4TradingInteractiveChartDetach(root);
    g_signal_emit_by_name(visible, "clicked");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tool), TRUE);
    CHECK(strcmp(previous, Result(root)) == 0);
    g_free(previous);
  } else if (strcmp(mode, "reentrant-detach") == 0) {
    g_signal_connect(section, "notify::expanded", G_CALLBACK(DetachOnExpand), root);
    g_signal_emit_by_name(visible, "clicked");
    CHECK(!gtk_widget_get_sensitive(root));
    CHECK(strstr(Result(root), "Captured") == NULL);
  } else if (strcmp(mode, "rows") == 0 || strcmp(mode, "invalid-rows") == 0) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "trading.chart.measurement-start")), 3);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "trading.chart.measurement-end")),
                              strcmp(mode, "rows") == 0 ? 2 : 256);
    g_signal_emit_by_name(rows, "clicked");
    if (strcmp(mode, "rows") == 0) {
      CHECK(strstr(Result(root), "2 candles") && strstr(Result(root), "Volume 23"));
      CHECK(strstr(Result(root), "Open 101 | Close 103 | Change 2"));
    } else
      CHECK(strstr(Result(root), "Captured") == NULL);
  } else {
    if (strcmp(mode, "visible") == 0 || strcmp(mode, "snapshot") == 0 ||
        strcmp(mode, "clear") == 0 || strcmp(mode, "timeframe") == 0 ||
        strcmp(mode, "instrument") == 0 || strcmp(mode, "copy") == 0) {
      g_signal_emit_by_name(visible, "clicked");
      CHECK(strstr(Result(root), "3 candles") && strstr(Result(root), "Volume 33"));
      CHECK(strstr(Result(root), "Open 100 | Close 103 | Change 3"));
      CHECK(gtk_expander_get_expanded(GTK_EXPANDER(section)));
    } else {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tool), TRUE);
      ClickChart(root, click, 30, 100);
      CHECK(strstr(Result(root), "First candle captured"));
      if (strcmp(mode, "escape") == 0) {
        GtkEventController *key = Controller(root, GTK_TYPE_EVENT_CONTROLLER_KEY);
        gboolean handled = FALSE;
        g_signal_emit_by_name(key, "key-pressed", GDK_KEY_Escape, 0U, (GdkModifierType)0, &handled);
        CHECK(handled && strstr(Result(root), "First candle captured") == NULL);
        g_object_unref(key);
      } else if (strcmp(mode, "projection") == 0) {
        UmiBar bar;
        OK(umi_trading_workspace_selected_bar_at(fixture.workspace, 2, &bar));
        bar.high = 200;
        OK(umi_trading_workspace_update_bar(fixture.workspace, &bar, 102));
        UmiGtk4TradingInteractiveChartRefresh(root);
        CHECK(strstr(Result(root), "First candle captured") == NULL);
      } else if (strcmp(mode, "outside") == 0) {
        ClickChart(root, click, 880, 500);
        CHECK(strstr(Result(root), "First candle captured"));
      } else {
        if (strcmp(mode, "pending-update") == 0) {
          UmiBar bar;
          OK(umi_trading_workspace_selected_bar_at(fixture.workspace, 2, &bar));
          bar.close = 103.5;
          OK(umi_trading_workspace_update_bar(fixture.workspace, &bar, 102));
          UmiGtk4TradingInteractiveChartRefresh(root);
        } else
          CHECK(strcmp(mode, "gesture") == 0);
        ClickChart(root, click, 880, 100);
        CHECK(strstr(Result(root), "3 candles") && strstr(Result(root), "Close 103 |"));
      }
    }
    if (strcmp(mode, "copy") == 0) {
      g_signal_emit_by_name(Find(root, "trading.chart.measurement-copy"), "clicked");
      CheckClipboard(root);
    }
    if (strcmp(mode, "snapshot") == 0) {
      char *previous = g_strdup(Result(root));
      UmiBar bar;
      OK(umi_trading_workspace_selected_bar_at(fixture.workspace, 2, &bar));
      bar.close = 103.5;
      OK(umi_trading_workspace_update_bar(fixture.workspace, &bar, 102));
      UmiGtk4TradingInteractiveChartRefresh(root);
      CHECK(strcmp(previous, Result(root)) == 0);
      g_free(previous);
    } else if (strcmp(mode, "clear") == 0) {
      g_signal_emit_by_name(Find(root, "trading.chart.measurement-clear"), "clicked");
      CHECK(strstr(Result(root), "Captured") == NULL);
    } else if (strcmp(mode, "timeframe") == 0) {
      gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "trading.chart.timeframe")), 1U);
      UmiGtk4TradingInteractiveChartRefresh(root);
      CHECK(strstr(Result(root), "Captured") == NULL);
    } else if (strcmp(mode, "instrument") == 0) {
      UmiInstrument other = test_instrument();
      OK(umi_trading_workspace_select_instrument(fixture.workspace, other.instrument_id.value));
      /* The test deliberately changes selection. Capture its consequences
       * before checking that refreshing the chart causes no further trade changes. */
      OK(umi_trading_workspace_snapshot(fixture.workspace, &before));
      UmiGtk4TradingInteractiveChartRefresh(root);
      CHECK(strstr(Result(root), "Captured") == NULL);
    }
  }
  SameTrading(fixture.workspace, &before);
  g_object_unref(click);
  if (root)
    g_object_unref(root);
  umi_trading_workspace_destroy(fixture.workspace);
  return 0;
}
