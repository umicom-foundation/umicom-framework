/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/event_calendar/test_event_calendar_gtk4.c
 *
 * PURPOSE:
 *   Exercise real calendar signals, stable mounts and retained-widget lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "calendar_fixture.h"
#include "umicom/trading_ui/gtk4/event_calendar.h"
#include "umicom/trading_ui/gtk4/trading_panels.h"
#include "umicom/ui/gtk4/automation.h"
static GtkWidget *Find(GtkWidget *root, const char *id) {
  GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, id);
  CHECK(widget != NULL);
  return widget;
}
static size_t Rows(GtkWidget *root) {
  GtkWidget *list = Find(root, "trading.calendar.rows");
  size_t count = 0U;
  for (GtkWidget *row = gtk_widget_get_first_child(list); row != NULL;
       row = gtk_widget_get_next_sibling(row))
    ++count;
  return count;
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!gtk_init_check())
    return 77;
  UmiTradingWorkspace *workspace = NULL;
  CHECK(umi_trading_workspace_create(NULL, &workspace) == UMI_STATUS_OK);
  UmiTradingUiController controller;
  CHECK(umi_trading_ui_controller_init(&controller, workspace, NULL) == UMI_STATUS_OK);
  uint64_t revision = umi_trading_ui_controller_snapshot(&controller).revision;
  UmiGtk4TradingPanelContext context = {workspace, &controller, 0};
  UmiUiWorkspaceWindow window = {0};
  strcpy(window.tool_id, "economic-calendar");
  GtkWidget *mount = umi_gtk4_trading_panel_create(&window, &context);
  CHECK(mount != NULL);
  g_object_ref_sink(mount);
  GtkWidget *panel = Find(mount, "trading.calendar");
  CHECK(UmiGtk4EventCalendarLoadCsv(panel, CALENDAR_CSV, strlen(CALENDAR_CSV)) == UMI_STATUS_OK &&
        Rows(panel) == 2U);
  GtkWidget *search = Find(panel, "trading.calendar.search"),
            *apply = Find(panel, "trading.calendar.apply");
  if (strcmp(argv[1], "filter") == 0 || strcmp(argv[1], "quote-refresh") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(search), "company");
    g_signal_emit_by_name(apply, "clicked");
    CHECK(Rows(panel) == 1U);
    if (strcmp(argv[1], "quote-refresh") == 0) {
      CHECK(UmiGtk4TradingPanelRefresh(mount, 1) == UMI_STATUS_OK);
      CHECK(Find(mount, "trading.calendar") == panel && Rows(panel) == 1U);
      CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(search)), "company") == 0);
    }
  } else if (strcmp(argv[1], "search-enter") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(search), "company");
    g_signal_emit_by_name(search, "activate");
    CHECK(Rows(panel) == 1U);
  } else if (strcmp(argv[1], "clear-filters") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(search), "no matching event");
    gtk_editable_set_text(GTK_EDITABLE(Find(panel, "trading.calendar.from")), "invalid");
    gtk_editable_set_text(GTK_EDITABLE(Find(panel, "trading.calendar.through")), "2020-01-01");
    gtk_editable_set_text(GTK_EDITABLE(Find(panel, "trading.calendar.country")), "GB");
    gtk_editable_set_text(GTK_EDITABLE(Find(panel, "trading.calendar.currency")), "GBP");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(panel, "trading.calendar.kind")), 2U);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(panel, "trading.calendar.importance")), 3U);
    g_signal_emit_by_name(Find(panel, "trading.calendar.clear"), "clicked");
    const char *ids[] = {"search", "from", "through", "country", "currency"};
    for (size_t i = 0U; i < sizeof ids / sizeof ids[0]; ++i) {
      char *tag = g_strconcat("trading.calendar.", ids[i], NULL);
      CHECK(gtk_editable_get_text(GTK_EDITABLE(Find(panel, tag)))[0] == '\0');
      g_free(tag);
    }
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(Find(panel, "trading.calendar.kind"))) == 0U);
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(Find(panel, "trading.calendar.importance"))) ==
          0U);
    CHECK(Rows(panel) == 2U);
  } else if (strcmp(argv[1], "today") == 0) {
    GDateTime *before = g_date_time_new_now_local();
    CHECK(before != NULL);
    char *first = g_date_time_format(before, "%Y-%m-%d");
    g_signal_emit_by_name(Find(panel, "trading.calendar.today"), "clicked");
    GDateTime *after = g_date_time_new_now_local();
    CHECK(after != NULL);
    char *last = g_date_time_format(after, "%Y-%m-%d");
    const char *from = gtk_editable_get_text(GTK_EDITABLE(Find(panel, "trading.calendar.from")));
    const char *through =
        gtk_editable_get_text(GTK_EDITABLE(Find(panel, "trading.calendar.through")));
    CHECK(strcmp(from, through) == 0 && (strcmp(from, first) == 0 || strcmp(from, last) == 0));
    /* The former assertion used the observation date rather than the event
     * date. Retain it for review; visible rows follow the source event date. */
#if 0
    CHECK(Rows(panel) == (strcmp(from, "2026-10-30") == 0 ? 2U : 0U));
#endif
    CHECK(Rows(panel) == (strcmp(from, "2026-11-02") == 0 ? 2U : 0U));
    g_free(first);
    g_free(last);
    g_date_time_unref(before);
    g_date_time_unref(after);
  } else if (strcmp(argv[1], "kind") == 0) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(panel, "trading.calendar.kind")), 2U);
    g_signal_emit_by_name(apply, "clicked");
    CHECK(Rows(panel) == 1U);
  } else if (strcmp(argv[1], "invalid-filter") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(Find(panel, "trading.calendar.from")), "2026-02-30");
    g_signal_emit_by_name(apply, "clicked");
    CHECK(Rows(panel) == 2U);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(panel, "trading.calendar.message"))),
                 "not applied") != NULL);
  } else if (strcmp(argv[1], "invalid-import") == 0) {
    CHECK(UmiGtk4EventCalendarLoadCsv(panel, "invalid", 7U) == UMI_STATUS_PARSE_ERROR &&
          Rows(panel) == 2U);
  } else if (strcmp(argv[1], "details") == 0) {
    GtkListBox *rows = GTK_LIST_BOX(Find(panel, "trading.calendar.rows"));
    gtk_list_box_select_row(rows, gtk_list_box_get_row_at_index(rows, 1));
    GtkTextBuffer *buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(panel, "trading.calendar.details")));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    CHECK(strstr(text, "America/New_York") != NULL && strstr(text, "Not supplied") != NULL);
    CHECK(strstr(text, "Example company calendar") != NULL &&
          strstr(text, "2026-10-30T08:00:00Z") != NULL);
    g_free(text);
  } else if (strcmp(argv[1], "retained") == 0) {
    g_object_ref(panel);
    g_object_unref(mount);
    mount = NULL;
    CHECK(UmiGtk4EventCalendarLoadCsv(panel, CALENDAR_CSV, strlen(CALENDAR_CSV)) ==
          UMI_STATUS_INVALID_STATE);
    gtk_editable_set_text(GTK_EDITABLE(search), "absent");
    g_signal_emit_by_name(apply, "clicked");
    CHECK(Rows(panel) == 2U);
    g_object_unref(panel);
  } else {
    CHECK(strcmp(argv[1], "detached") == 0);
    UmiGtk4TradingPanelDetachChart(mount);
    CHECK(!gtk_widget_is_sensitive(panel));
    CHECK(UmiGtk4EventCalendarLoadCsv(panel, CALENDAR_CSV, strlen(CALENDAR_CSV)) ==
          UMI_STATUS_INVALID_STATE);
  }
  CHECK(umi_trading_ui_controller_snapshot(&controller).revision == revision);
  if (mount != NULL)
    g_object_unref(mount);
  umi_trading_workspace_destroy(workspace);
  return 0;
}
