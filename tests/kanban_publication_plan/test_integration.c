/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_publication_plan/test_integration.c
 *
 * PURPOSE:
 *   Verify native board adoption checks and private-field exclusion through generated HTML.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "fixture.h"
#include "umicom/kanban/publication_plan_storage.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/kanban.h"
#include "umicom/ui/gtk4/kanban_publication_store.h"
static GtkWidget *Named(GtkWidget *root, const char *name) {
  if (!strcmp(gtk_widget_get_name(root), name))
    return root;
  for (GtkWidget *c = gtk_widget_get_first_child(root); c != NULL;
       c = gtk_widget_get_next_sibling(c)) {
    GtkWidget *found = Named(c, name);
    if (found != NULL)
      return found;
  }
  return NULL;
}
static GtkWidget *ButtonNamed(GtkWidget *root, const char *name) {
  if (GTK_IS_BUTTON(root) && g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), name) == 0)
    return root;
  for (GtkWidget *c = gtk_widget_get_first_child(root); c != NULL;
       c = gtk_widget_get_next_sibling(c)) {
    GtkWidget *found = ButtonNamed(c, name);
    if (found != NULL)
      return found;
  }
  return NULL;
}
static GtkWidget *Tagged(GtkWidget *root, const char *tag) {
  GtkWidget *w = umi_gtk4_automation_find_tagged_widget(root, tag);
  REQUIRE(w != NULL);
  return w;
}
static void Drain(void) {
  for (unsigned i = 0U; i < 30U; ++i) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000U);
  }
}
int main(int argc, char **argv) {
  REQUIRE(argc == 2);
  const char *name = argv[1], *cases[] = {"normal",   "wrong-board", "stale-view", "review-current",
                                          "archived", "load-only",   "busy",       "retired"};
  CaseKnown(name, cases, sizeof cases / sizeof cases[0]);
  if (!gtk_init_check())
    return 77;
  UmiKanbanBoard *board = PlanBoard();
  UmiKanbanPublicationPlan *plan = Plan(board);
  char directory[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  REQUIRE(UmiKanbanPublicationPlanSave(directory, "reviewed.umipublic", plan) == UMI_STATUS_OK);
  UmiKanbanPublicationPlanDestroy(plan);
  if (!strcmp(name, "wrong-board")) {
    umi_kanban_board_destroy(board);
    REQUIRE(umi_kanban_board_create_workflow("different", "Different", &board) == UMI_STATUS_OK);
  }
  GtkWidget *panel = UmiKanbanGtk4Create(board);
  REQUIRE(panel != NULL);
  g_object_ref_sink(panel);
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  gtk_window_set_child(window, panel);
  gtk_window_present(window);
  REQUIRE(umi_gtk4_automation_reveal_tagged_widget(GTK_WIDGET(window), "kanban.plan.load") ==
          UMI_STATUS_OK);
  Drain();
  GtkWidget *store = Tagged(panel, "kanban.plan.panel"), *load = Tagged(panel, "kanban.plan.load");
  g_object_ref(store);
  gtk_editable_set_text(GTK_EDITABLE(Tagged(panel, "kanban.plan.directory")), directory);
  g_signal_emit_by_name(load, "clicked");
  gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
  while (UmiKanbanPublicPlanGtk4Pending(store) && g_get_monotonic_time() < end) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000U);
  }
  REQUIRE(!UmiKanbanPublicPlanGtk4Pending(store));
  Drain();
  UmiKanbanEdit edit;
  if (!strcmp(name, "stale-view") || !strcmp(name, "review-current"))
    Move(board, &edit, "card.first", "doing");
  if (!strcmp(name, "archived")) {
    Action(&edit, UMI_KANBAN_ARCHIVE_CARD, "card.first", NULL);
    Apply(board, &edit, 1U);
  }
  if (!strcmp(name, "review-current") || !strcmp(name, "archived"))
    REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
  if (!strcmp(name, "busy"))
    UmiKanbanGtk4SetBusy(panel, true);
  if (!strcmp(name, "retired"))
    UmiKanbanGtk4Retire(panel);
  if (strcmp(name, "load-only")) {
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Tagged(panel, "kanban.plan.approval")), TRUE);
    g_signal_emit_by_name(Tagged(panel, "kanban.plan.adopt"), "clicked");
  }
  GtkWidget *source = Named(panel, "kanban-public-source");
  REQUIRE(GTK_IS_TEXT_VIEW(source));
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source));
  REQUIRE(gtk_text_buffer_get_char_count(buffer) == 0);
  GtkWidget *generate = ButtonNamed(panel, "Generate website");
  REQUIRE(GTK_IS_BUTTON(generate));
  g_signal_emit_by_name(generate, "clicked");
  GtkTextIter start, finish;
  gtk_text_buffer_get_bounds(buffer, &start, &finish);
  char *html = gtk_text_buffer_get_text(buffer, &start, &finish, FALSE);
  bool accepted = !strcmp(name, "normal") || !strcmp(name, "review-current");
  REQUIRE((strstr(html, "Reviewed feature") != NULL) == accepted);
  REQUIRE(strstr(html, "PRIVATE_") == NULL);
  g_free(html);
  UmiKanbanGtk4Retire(panel);
  gtk_window_destroy(window);
  g_object_unref(store);
  g_object_unref(panel);
  Drain();
  umi_kanban_board_destroy(board);
  return 0;
}
