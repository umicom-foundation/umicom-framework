/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_public_freshness.c
 *
 * PURPOSE:
 *   Keep reviewed website export tied to board revisions and current public selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/kanban.h"
#include "umicom/ui/gtk4/text_export.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
static GtkWidget *Named(GtkWidget *root, const char *name) {
  if (!strcmp(gtk_widget_get_name(root), name))
    return root;
  if (GTK_IS_EXPANDER(root))
    gtk_expander_set_expanded(GTK_EXPANDER(root), TRUE);
  for (GtkWidget *c = gtk_widget_get_first_child(root); c; c = gtk_widget_get_next_sibling(c)) {
    GtkWidget *found = Named(c, name);
    if (found)
      return found;
  }
  return NULL;
}
static GtkWidget *Button(GtkWidget *root, const char *label) {
  if (GTK_IS_BUTTON(root) && g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), label) == 0)
    return root;
  if (GTK_IS_EXPANDER(root))
    gtk_expander_set_expanded(GTK_EXPANDER(root), TRUE);
  for (GtkWidget *c = gtk_widget_get_first_child(root); c; c = gtk_widget_get_next_sibling(c)) {
    GtkWidget *found = Button(c, label);
    if (found)
      return found;
  }
  return NULL;
}
static void Click(GtkWidget *root, const char *label) {
  GtkWidget *b = Button(root, label);
  CHECK(b != NULL);
  g_signal_emit_by_name(b, "clicked");
}
static void Set(GtkWidget *root, const char *name, const char *text) {
  GtkWidget *field = Named(root, name);
  CHECK(GTK_IS_EDITABLE(field));
  gtk_editable_set_text(GTK_EDITABLE(field), text);
}
static void External(UmiKanbanBoard *board) {
  UmiKanbanEdit *edit = g_new0(UmiKanbanEdit, 1);
  edit->kind = UMI_KANBAN_CREATE_CARD;
  strcpy(edit->card.id, "external");
  strcpy(edit->card.title, "PRIVATE_EXTERNAL");
  strcpy(edit->column_id, "todo");
  CHECK(umi_kanban_board_apply(board, umi_kanban_board_revision(board), "external", edit, 1U,
                               NULL) == UMI_STATUS_OK);
  g_free(edit);
}
typedef struct Reentry {
  GtkWidget *panel, *save;
  UmiKanbanBoard *board;
  const char *mode;
  unsigned calls;
} Reentry;
static void Changed(GtkTextBuffer *buffer, gpointer data) {
  Reentry *r = data;
  if (r->calls || gtk_text_buffer_get_char_count(buffer) == 0)
    return;
  ++r->calls;
  if (!strcmp(r->mode, "reentrant-title"))
    Set(r->panel, "kanban-public-board-title", "Changed during render");
  else if (!strcmp(r->mode, "reentrant-edit"))
    External(r->board);
  else
    g_signal_emit_by_name(r->save, "clicked");
}
static void Wait(GtkWidget *root) {
  gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
  while (UmiTextExportGtk4Busy(root) && g_get_monotonic_time() < end) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000);
  }
  CHECK(!UmiTextExportGtk4Busy(root));
}
int main(int argc, char **argv) {
  CHECK(argc == 3);
  if (!gtk_init_check())
    return 77;
  char *directory = g_build_filename(argv[2], "preview-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory) != NULL);
  char *path = g_build_filename(directory, "index.html", NULL);
  UmiKanbanBoard *board = NULL;
  CHECK(umi_kanban_board_create_workflow("private", "PRIVATE_BOARD", &board) == UMI_STATUS_OK);
  GtkWidget *panel = UmiKanbanGtk4Create(board);
  CHECK(panel != NULL);
  g_object_ref_sink(panel);
  Set(panel, "kanban-title", "PRIVATE_CARD");
  Click(panel, "Add card");
  Set(panel, "kanban-public-title", "Reviewed feature");
  Click(panel, "Include reviewed card");
  GtkTextBuffer *buffer =
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(Named(panel, "kanban-public-source")));
  GtkWidget *export = umi_gtk4_automation_find_tagged_widget(panel, "text.export");
  CHECK(export != NULL);
  gtk_expander_set_expanded(GTK_EXPANDER(export), TRUE);
  GtkWidget *entry = umi_gtk4_automation_find_tagged_widget(export, "text.export.path");
  GtkWidget *save = umi_gtk4_automation_find_tagged_widget(export, "text.export.save");
  CHECK(entry && save);
  gtk_editable_set_text(GTK_EDITABLE(entry), path);
  Reentry reentry = {panel, save, board, argv[1], 0U};
  if (!strncmp(argv[1], "reentrant", 9U))
    g_signal_connect(buffer, "changed", G_CALLBACK(Changed), &reentry);
  Click(panel, "Generate website");
  bool allowed = !strcmp(argv[1], "normal") || !strcmp(argv[1], "reentrant-export") ||
                 !strcmp(argv[1], "regenerate") || !strcmp(argv[1], "refused-edit") ||
                 !strcmp(argv[1], "accepted-snapshot");
  if (!strncmp(argv[1], "reentrant", 9U)) {
    CHECK(reentry.calls == 1U);
    CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
  } else
    CHECK(gtk_text_buffer_get_char_count(buffer) > 0);
  if (!strcmp(argv[1], "include-change") || !strcmp(argv[1], "regenerate")) {
    Set(panel, "kanban-public-title", "Updated public title");
    Click(panel, "Include reviewed card");
    CHECK(gtk_text_buffer_get_char_count(buffer) == 0);
    if (allowed)
      Click(panel, "Generate website");
  } else if (!strcmp(argv[1], "title-change")) {
    Set(panel, "kanban-public-board-title", "New public board");
    CHECK(gtk_text_buffer_get_char_count(buffer) == 0);
  } else if (!strcmp(argv[1], "board-edit")) {
    Set(panel, "kanban-title", "PRIVATE_RENAME");
    Click(panel, "Rename selected");
    CHECK(gtk_text_buffer_get_char_count(buffer) == 0);
  } else if (!strcmp(argv[1], "external") || !strcmp(argv[1], "external-refresh") ||
             !strcmp(argv[1], "generate-refused")) {
    External(board);
    if (!strcmp(argv[1], "external-refresh"))
      CHECK(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
    if (!strcmp(argv[1], "generate-refused"))
      Click(panel, "Generate website");
    CHECK((gtk_text_buffer_get_char_count(buffer) > 0) == !strcmp(argv[1], "external"));
  } else if (!strcmp(argv[1], "clear")) {
    Click(panel, "Clear selection");
    CHECK(gtk_text_buffer_get_char_count(buffer) == 0);
  } else if (!strcmp(argv[1], "retired")) {
    UmiKanbanGtk4Retire(panel);
  } else if (!strcmp(argv[1], "refused-edit")) {
    Set(panel, "kanban-title", "");
    Click(panel, "Rename selected");
    CHECK(gtk_text_buffer_get_char_count(buffer) > 0);
  }
  g_signal_emit_by_name(save, "clicked");
  if (!strcmp(argv[1], "accepted-snapshot"))
    Set(panel, "kanban-public-board-title", "Later title");
  Wait(export);
  CHECK(g_file_test(path, G_FILE_TEST_EXISTS) == allowed);
  if (allowed) {
    char *html = NULL;
    CHECK(g_file_get_contents(path, &html, NULL, NULL));
    CHECK(strstr(html, "PRIVATE") == NULL);
    CHECK(strstr(html, !strcmp(argv[1], "regenerate") ? "Updated public title"
                                                      : "Reviewed feature") != NULL);
    if (!strcmp(argv[1], "accepted-snapshot"))
      CHECK(strstr(html, "Later title") == NULL);
    g_free(html);
  }
  UmiKanbanGtk4Retire(panel);
  g_object_unref(panel);
  umi_kanban_board_destroy(board);
  g_free(directory);
  g_free(path);
  return 0;
}
