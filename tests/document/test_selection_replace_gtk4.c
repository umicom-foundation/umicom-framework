/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_selection_replace_gtk4.c
 *
 * PURPOSE:
 *   Exercise the native selected replacement review with explicit comparison, approval and owner
 * retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "native_edit_fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/selection_replace.h"
#include <stdio.h>
#define CHECK(c)                                                                                   \
  do {                                                                                             \
    if (!(c)) {                                                                                    \
      fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                              \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
static GtkWidget *Find(GtkWidget *root, const char *id) {
  return umi_gtk4_automation_find_tagged_widget(root, id);
}
static GtkWindow *Review(void) {
  GListModel *windows = gtk_window_get_toplevels();
  for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
    GtkWindow *window = g_list_model_get_item(windows, i);
    if (Find(GTK_WIDGET(window), "document.proposal.review"))
      return window;
    g_object_unref(window);
  }
  return NULL;
}
static void UnbindOnCreate(GListModel *model, guint position, guint removed, guint added,
                           gpointer data) {
  (void)model;
  (void)position;
  (void)removed;
  if (added) {
    NativeEditFixture *f = data;
    (void)UmiGtk4AdapterBindDocumentEditing(f->adapter, NULL, NULL, NULL);
  }
}
/* A toplevel observer can re-enter the API before proposal-dialog is published. */
typedef struct DuringConstruction {
  UmiGtk4Adapter *adapter;
  int called;
  UmiStatus status;
} DuringConstruction;
static void RepeatOnCreate(GListModel *model, guint position, guint removed, guint added,
                           gpointer data) {
  (void)model;
  (void)position;
  (void)removed;
  DuringConstruction *request = data;
  if (added && !request->called) {
    request->called = 1;
    request->status =
        UmiGtk4AdapterReviewSelectionReplacement(request->adapter, "old", "nested", NULL);
  }
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  const char *cases[] = {"prefill",
                         "apply",
                         "undo",
                         "cancel",
                         "no-preview",
                         "no-approval",
                         "edit-after-preview",
                         "stale",
                         "read-only",
                         "empty-selection",
                         "no-match",
                         "busy",
                         "unbind",
                         "constructor-unbind",
                         "constructor-repeat",
                         "completion-retire",
                         "delete",
                         "arguments",
                         "options"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  NativeEditFixture f = {0};
  GtkWindow *dialog = NULL;
  GtkWidget *apply = NULL, *input = NULL;
  char *prefill = NULL;
  gulong observer = 0UL;
  DuringConstruction reenter = {0};
  const char *source = !strcmp(name, "options") ? "old OLD old" : "old old old";
  const char *needle = !strcmp(name, "no-match") ? "absent" : "old";
  const char *replacement = !strcmp(name, "delete") ? "" : "new";
  UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE, 0, 0, 0U};
  CHECK(NativeEditStart(&f, source, 4U, 3U, !strcmp(name, "read-only")));
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.view, &view) == UMI_STATUS_OK);
  view.cursor_offset = 4U;
  view.selection_length = !strcmp(name, "empty-selection") ? 0U : 3U;
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4AdapterReviewSelectionReplacement(NULL, needle, replacement, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4AdapterReviewSelectionReplacement(f.adapter, NULL, replacement, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4AdapterReviewSelectionReplacement(f.adapter, needle, NULL, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (!strcmp(name, "constructor-unbind"))
    observer = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                G_CALLBACK(UnbindOnCreate), &f);
  UmiStatus status =
      UmiGtk4AdapterReviewSelectionReplacement(f.adapter, needle, replacement, &options);
  if (observer) {
    g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
    observer = 0UL;
  }
  if (!strcmp(name, "constructor-unbind")) {
    CHECK(status == UMI_STATUS_CANCELLED);
    dialog = Review();
    CHECK(dialog == NULL && f.calls == 0U && NativeEditText(&f, source));
    goto cleanup;
  }
  if (!strcmp(name, "read-only") || !strcmp(name, "empty-selection")) {
    CHECK(status == (!strcmp(name, "read-only") ? UMI_STATUS_PERMISSION_DENIED
                                                : UMI_STATUS_INVALID_ARGUMENT));
    dialog = Review();
    CHECK(dialog == NULL && NativeEditText(&f, source));
    goto cleanup;
  }
  CHECK(status == UMI_STATUS_OK);
  if (!strcmp(name, "constructor-repeat"))
    CHECK(reenter.called && reenter.status == UMI_STATUS_BUSY);
  dialog = Review();
  CHECK(dialog != NULL);
  GtkWidget *found = Find(GTK_WIDGET(dialog), "document.proposal.apply");
  CHECK(found != NULL);
  apply = g_object_ref(found);
  found = Find(GTK_WIDGET(dialog), "document.proposal.input");
  CHECK(found != NULL);
  input = g_object_ref(found);
  GtkWidget *preview = Find(GTK_WIDGET(dialog), "document.proposal.preview"),
            *approve = Find(GTK_WIDGET(dialog), "document.proposal.approve"),
            *cancel = Find(GTK_WIDGET(dialog), "document.proposal.cancel");
  CHECK(preview && approve && cancel && !gtk_check_button_get_active(GTK_CHECK_BUTTON(approve)) &&
        !gtk_widget_get_sensitive(apply));
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input));
  GtkTextIter first, last;
  gtk_text_buffer_get_bounds(buffer, &first, &last);
  prefill = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
  CHECK(!strcmp(prefill, !strcmp(name, "no-match") ? "old" : replacement) &&
        NativeEditText(&f, source));
  if (!strcmp(name, "prefill"))
    goto cleanup;
  if (!strcmp(name, "busy")) {
    CHECK(UmiGtk4AdapterReviewSelectionReplacement(f.adapter, needle, replacement, &options) ==
          UMI_STATUS_BUSY);
    CHECK(UmiGtk4AdapterReviewSelectedCode(f.adapter) == UMI_STATUS_BUSY);
    goto cleanup;
  }
  if (!strcmp(name, "cancel")) {
    g_signal_emit_by_name(cancel, "clicked");
    CHECK(f.calls == 1U && f.status == UMI_STATUS_CANCELLED && NativeEditText(&f, source));
    goto cleanup;
  }
  if (!strcmp(name, "unbind")) {
    CHECK(UmiGtk4AdapterBindDocumentEditing(f.adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    g_signal_emit_by_name(apply, "clicked");
    CHECK(f.calls == 0U && NativeEditText(&f, source));
    goto cleanup;
  }
  if (strcmp(name, "no-preview"))
    g_signal_emit_by_name(preview, "clicked");
  if (strcmp(name, "no-approval"))
    gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
  if (!strcmp(name, "edit-after-preview")) {
    gtk_text_buffer_set_text(buffer, "unreviewed", -1);
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(approve)) &&
          !gtk_widget_get_sensitive(apply));
  }
  if (!strcmp(name, "stale")) {
    CHECK(umi_ui_document_view_model_find(views, f.view, &view) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "later source", 12U) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "completion-retire"))
    f.retire_on_completion = 1;
  g_signal_emit_by_name(apply, "clicked");
  if (!strcmp(name, "no-preview") || !strcmp(name, "no-approval") ||
      !strcmp(name, "edit-after-preview") || !strcmp(name, "stale")) {
    CHECK(f.calls == 0U && NativeEditText(&f, !strcmp(name, "stale") ? "later source" : source));
  } else {
    CHECK(f.calls == 1U && f.status == UMI_STATUS_OK);
    CHECK(NativeEditText(&f, !strcmp(name, "delete")     ? "old  old"
                             : !strcmp(name, "no-match") ? source
                                                         : "old new old"));
    if (!strcmp(name, "undo")) {
      CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
            NativeEditText(&f, source));
    }
    g_signal_emit_by_name(apply, "clicked");
    CHECK(f.calls == 1U);
  }
cleanup:
  if (observer)
    g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
  if (f.adapter)
    (void)UmiGtk4AdapterBindDocumentEditing(f.adapter, NULL, NULL, NULL);
  if (dialog)
    gtk_window_destroy(dialog);
  g_clear_object(&dialog);
  g_clear_object(&apply);
  g_clear_object(&input);
  g_free(prefill);
  NativeEditStop(&f);
  return failed;
}
