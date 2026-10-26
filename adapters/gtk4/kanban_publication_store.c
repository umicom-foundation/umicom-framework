/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_publication_store.c
 *
 * PURPOSE:
 *   Run bounded private plan I/O off the GTK thread and require explicit owner review for adoption.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban_publication_store.h"
#include "umicom/kanban/publication_plan_storage.h"
#include "umicom/platform/path.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/path_picker.h"
#include <string.h>
#include "focus_handoff_private.h"
typedef struct PlanPanel {
  GtkWidget *root, *directory, *filename, *status, *approval;
  GtkTextBuffer *preview;
  UmiKanbanPublicationPlan *loaded;
  UmiKanbanPublicPlanCapture capture;
  UmiKanbanPublicPlanAdopt adopt;
  gpointer context;
  GDestroyNotify destroy;
  gboolean pending, busy, retired, abandoned;
} PlanPanel;
typedef struct PlanJob {
  GtkWidget *root;
  char directory[UMI_PATH_CAPACITY], filename[256];
  gboolean save;
  UmiStatus status;
  UmiKanbanPublicationPlan *plan;
} PlanJob;
static PlanPanel *State(GtkWidget *root) {
  return GTK_IS_WIDGET(root) ? g_object_get_data(G_OBJECT(root), "umicom-public-plan-store") : NULL;
}
static void FreePanel(gpointer data) {
  PlanPanel *s = data;
  UmiKanbanPublicationPlanDestroy(s->loaded);
  if (s->destroy != NULL)
    s->destroy(s->context);
  g_object_unref(s->directory);
  g_object_unref(s->filename);
  g_object_unref(s->status);
  g_object_unref(s->approval);
  g_object_unref(s->preview);
  g_free(s);
}
static gboolean Live(PlanPanel *s) {
  return s != NULL && !s->retired && !s->busy && gtk_widget_get_mapped(s->root);
}
static gboolean Active(PlanPanel *s, GtkWidget *button) {
  return Live(s) && !s->pending && gtk_widget_get_mapped(button) &&
         gtk_widget_is_ancestor(button, s->root);
}
static void Message(PlanPanel *s, const char *message) {
  if (Live(s))
    gtk_label_set_text(GTK_LABEL(s->status), message);
}
static void Forget(PlanPanel *s) {
  UmiKanbanPublicationPlanDestroy(s->loaded);
  s->loaded = NULL;
  gtk_check_button_set_active(GTK_CHECK_BUTTON(s->approval), FALSE);
  gtk_text_buffer_set_text(s->preview, "", -1);
}
static void Preview(PlanPanel *s) {
  UmiKanbanPublicationPlanInfo info;
  if (UmiKanbanPublicationPlanRead(s->loaded, &info) != UMI_STATUS_OK)
    return;
  GString *text = g_string_new(NULL);
  g_string_append_printf(text,
                         "PRIVATE saved selection\nBoard ID: %s\nSaved revision: %" G_GUINT64_FORMAT
                         "\nPublic title: %s\nSelected cards: %zu\n",
                         info.board_id, (guint64)info.reviewed_revision, info.title,
                         info.card_count);
  for (size_t i = 0U; i < info.card_count; ++i) {
    const UmiKanbanPublicCard *c = UmiKanbanPublicationPlanAt(s->loaded, i);
    g_string_append_printf(
        text, "\nCard %zu (private ID %s)\nTitle: %s\nSummary: %s\nPublic developer: %s\n", i + 1U,
        c->card_id, c->title, c->summary, c->developer);
  }
  gtk_text_buffer_set_text(s->preview, text->str, (gint)text->len);
  g_string_free(text, TRUE);
}
static void JobFree(gpointer data) {
  PlanJob *j = data;
  UmiKanbanPublicationPlanDestroy(j->plan);
  g_object_unref(j->root);
  g_free(j);
}
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  PlanJob *j = data;
  j->status = j->save ? UmiKanbanPublicationPlanSave(j->directory, j->filename, j->plan)
                      : UmiKanbanPublicationPlanLoad(j->directory, j->filename, &j->plan);
  g_task_return_boolean(task, TRUE);
}
static void Complete(GObject *source, GAsyncResult *result, gpointer data) {
  (void)source;
  (void)data;
  PlanJob *j = g_task_get_task_data(G_TASK(result));
  PlanPanel *s = State(j->root);
  if (!s->abandoned && Live(s)) {
    if (j->status == UMI_STATUS_OK && !j->save) {
      s->loaded = j->plan;
      j->plan = NULL;
      Preview(s);
      Message(s, "Loaded private selection. Review every field, approve, then Adopt; no board or "
                 "website has changed.");
    } else
      Message(s, j->status == UMI_STATUS_OK
                     ? "Private selection saved. Publish generated HTML only, never this plan file."
                     : "Plan operation failed. Check the private directory, .umipublic filename "
                       "and complete archive.");
  }
  s->pending = FALSE;
}
static void Dispatch(GtkButton *button, gpointer data) {
  PlanPanel *s = State(data);
  if (!Active(s, GTK_WIDGET(button)))
    return;
  g_object_ref(s->root);
  s->pending = TRUE;
  s->abandoned = FALSE;
  PlanJob *j = g_new0(PlanJob, 1);
  j->root = g_object_ref(s->root);
  j->save = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "save-plan")) != 0;
  const char *dir = gtk_editable_get_text(GTK_EDITABLE(s->directory));
  const char *file = gtk_editable_get_text(GTK_EDITABLE(s->filename));
  gboolean valid = strlen(dir) < sizeof j->directory && strlen(file) < sizeof j->filename;
  if (valid) {
    memcpy(j->directory, dir, strlen(dir) + 1U);
    memcpy(j->filename, file, strlen(file) + 1U);
  }
  if (!j->save)
    Forget(s);
  UmiStatus status = valid ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK && Live(s) && !s->abandoned && j->save)
    status = s->capture(s->context, &j->plan);
  if (status == UMI_STATUS_OK && Live(s) && !s->abandoned) {
    Message(s, j->save ? "Saving the captured private selection..."
                       : "Reading a private publication selection...");
    if (Live(s) && !s->abandoned) {
      GTask *task = g_task_new(NULL, NULL, Complete, NULL);
      g_task_set_task_data(task, j, JobFree);
      g_task_run_in_thread(task, Worker);
      g_object_unref(task);
      j = NULL;
    }
  } else
    Message(s, "Unable to capture this selection. Refresh the board and review the public fields.");
  if (j != NULL) {
    s->pending = FALSE;
    JobFree(j);
  }
  g_object_unref(s->root);
}
static void Adopt(GtkButton *button, gpointer data) {
  PlanPanel *s = State(data);
  if (!Active(s, GTK_WIDGET(button)) || s->loaded == NULL ||
      !gtk_check_button_get_active(GTK_CHECK_BUTTON(s->approval)))
    return;
  g_object_ref(s->root);
  s->pending = TRUE;
  UmiStatus status = s->adopt(s->context, s->loaded);
  if (Live(s)) {
    gtk_check_button_set_active(GTK_CHECK_BUTTON(s->approval), FALSE);
    Message(s, status == UMI_STATUS_OK ? "Reviewed public fields adopted. Generate HTML separately "
                                         "to inspect the current snapshot."
                                       : "Adoption refused. Check board identity, active cards and "
                                         "current revision; no selection was replaced.");
  }
  s->pending = FALSE;
  g_object_unref(s->root);
}
static void Discard(GtkButton *button, gpointer data) {
  PlanPanel *s = State(data);
  if (!Active(s, GTK_WIDGET(button)))
    return;
  g_object_ref(s->root);
  s->pending = TRUE;
  Forget(s);
  Message(s, "Loaded selection discarded. Its saved file is unchanged.");
  s->pending = FALSE;
  g_object_unref(s->root);
}
static void Unmap(GtkWidget *widget, gpointer data) {
  (void)widget;
  PlanPanel *s = data;
  if (s->pending)
    s->abandoned = TRUE;
}
void UmiKanbanPublicPlanGtk4SetBusy(GtkWidget *root, gboolean busy) {
  PlanPanel *s = State(root);
  if (s != NULL)
    s->busy = busy;
}
void UmiKanbanPublicPlanGtk4Retire(GtkWidget *root) {
  PlanPanel *s = State(root);
  if (s == NULL || s->retired)
    return;
  g_object_ref(root);
  s->retired = TRUE;
  s->abandoned = TRUE;
  /* Callback retirement does not undo a save already dispatched to storage.
   * End input focus while the original text hierarchy is still sensitive. */
  UmiGtk4ReleaseSubtreeFocus(root);
  gtk_widget_set_sensitive(root, FALSE);
  g_object_unref(root);
}
gboolean UmiKanbanPublicPlanGtk4Pending(GtkWidget *root) {
  PlanPanel *s = State(root);
  return s != NULL && s->pending;
}
static GtkWidget *Entry(PlanPanel *s, const char *hint, const char *tag, int maximum) {
  GtkWidget *w = gtk_entry_new();
  g_object_ref_sink(w);
  gtk_entry_set_placeholder_text(GTK_ENTRY(w), hint);
  gtk_entry_set_max_length(GTK_ENTRY(w), maximum);
  (void)umi_gtk4_automation_tag_widget(w, tag);
  gtk_box_append(GTK_BOX(s->root), w);
  return w;
}
static void Button(PlanPanel *s, GtkWidget *row, const char *title, const char *tag,
                   GCallback callback, gboolean save) {
  GtkWidget *w = gtk_button_new_with_label(title);
  (void)umi_gtk4_automation_tag_widget(w, tag);
  g_object_set_data(G_OBJECT(w), "save-plan", GINT_TO_POINTER(save));
  g_signal_connect_object(w, "clicked", callback, s->root, 0);
  gtk_box_append(GTK_BOX(row), w);
}
GtkWidget *UmiKanbanPublicPlanGtk4Create(UmiKanbanPublicPlanCapture capture,
                                         UmiKanbanPublicPlanAdopt adopt, gpointer context,
                                         GDestroyNotify destroy) {
  if (capture == NULL || adopt == NULL)
    return NULL;
  PlanPanel *s = g_new0(PlanPanel, 1);
  s->capture = capture;
  s->adopt = adopt;
  s->context = context;
  s->destroy = destroy;
  s->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  s->directory =
      Entry(s, "Existing private directory", "kanban.plan.directory", UMI_PATH_CAPACITY - 1);
  GtkWidget *pick = UmiPathPickerGtk4Create(GTK_ENTRY(s->directory), UMI_PATH_PICKER_DIRECTORY,
                                            "Private selection directory", NULL);
  if (pick != NULL)
    gtk_box_append(GTK_BOX(s->root), pick);
  s->filename = Entry(s, "Private .umipublic filename", "kanban.plan.filename", 255);
  gtk_editable_set_text(GTK_EDITABLE(s->filename), "reviewed.umipublic");
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  Button(s, row, "Save current public selection", "kanban.plan.save", G_CALLBACK(Dispatch), TRUE);
  Button(s, row, "Load selection for review", "kanban.plan.load", G_CALLBACK(Dispatch), FALSE);
  Button(s, row, "Discard loaded selection", "kanban.plan.discard", G_CALLBACK(Discard), FALSE);
  gtk_box_append(GTK_BOX(s->root), row);
  s->preview = gtk_text_buffer_new(NULL);
  GtkWidget *view = gtk_text_view_new_with_buffer(s->preview);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
  (void)umi_gtk4_automation_tag_widget(view, "kanban.plan.preview");
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 180);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
  gtk_box_append(GTK_BOX(s->root), scroll);
  s->approval = gtk_check_button_new_with_label("I reviewed these public fields for this board");
  g_object_ref_sink(s->approval);
  (void)umi_gtk4_automation_tag_widget(s->approval, "kanban.plan.approval");
  gtk_box_append(GTK_BOX(s->root), s->approval);
  row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  Button(s, row, "Adopt reviewed fields", "kanban.plan.adopt", G_CALLBACK(Adopt), FALSE);
  gtk_box_append(GTK_BOX(s->root), row);
  s->status = gtk_label_new(
      "Keep selection files private. Loading does not generate HTML or change the board.");
  g_object_ref_sink(s->status);
  gtk_label_set_wrap(GTK_LABEL(s->status), TRUE);
  (void)umi_gtk4_automation_tag_widget(s->status, "kanban.plan.status");
  gtk_box_append(GTK_BOX(s->root), s->status);
  (void)umi_gtk4_automation_tag_widget(s->root, "kanban.plan.panel");
  g_object_set_data_full(G_OBJECT(s->root), "umicom-public-plan-store", s, FreePanel);
  g_signal_connect(s->root, "unmap", G_CALLBACK(Unmap), s);
  return s->root;
}
