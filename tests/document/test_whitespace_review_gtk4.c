/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_whitespace_review_gtk4.c
 *
 * PURPOSE:
 *   Exercise native review ownership, cancellation and explicit draft application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "native_edit_fixture.h"
#include "umicom/ui/gtk4/document_whitespace.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name) {
  if (!strcmp(gtk_widget_get_name(root), name))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = Find(child, name);
    if (found)
      return found;
  }
  return NULL;
}
static GtkWindow *Form(void) {
  GListModel *windows = gtk_window_get_toplevels();
  for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
    GtkWindow *w = g_list_model_get_item(windows, i);
    if (!strcmp(gtk_widget_get_name(GTK_WIDGET(w)), "document.whitespace.review") &&
        UmiGtk4WindowIsOpen(w))
      return w;
    g_object_unref(w);
  }
  return NULL;
}
static void Retire(GObject *object, GParamSpec *spec, gpointer data) {
  (void)object;
  (void)spec;
  NativeEditFixture *f = data;
  if (f->adapter) {
    umi_gtk4_adapter_destroy(f->adapter);
    f->adapter = NULL;
  }
}
typedef struct ConstructorRetirement {
  NativeEditFixture *fixture;
  int armed;
} ConstructorRetirement;
static void ConstructorChanged(GListModel *model, guint position, guint removed, guint added,
                               gpointer data) {
  (void)model;
  (void)position;
  (void)removed;
  ConstructorRetirement *state = data;
  if (added && state->armed) {
    state->armed = 0;
    Retire(NULL, NULL, state->fixture);
  }
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1], *names[] = {"entry",
                                          "apply",
                                          "expand",
                                          "compact",
                                          "selection",
                                          "stale",
                                          "inactive",
                                          "closed-document",
                                          "readonly",
                                          "retired",
                                          "rebound",
                                          "cancel",
                                          "repeat",
                                          "completion-retire",
                                          "message-retire",
                                          "arguments",
                                          "constructor-retire"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  NativeEditFixture f = {0};
  int failed = 0, applied = 0;
  GtkWindow *form = NULL;
  GtkWidget *held_apply = NULL, *held_message = NULL;
  gulong signal = 0U;
  const char *source = "a \nb \n", *expected = "a\nb\n";
  UmiEditorWhitespaceOptions options = {UMI_EDITOR_WHITESPACE_TRIM_TRAILING, 4U, 0};
  if (!strcmp(name, "expand")) {
    source = "\tx";
    expected = "    x";
    options.operation = UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS;
  }
  if (!strcmp(name, "compact")) {
    source = "    x";
    expected = "\tx";
    options.operation = UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES;
  }
  if (!strcmp(name, "selection")) {
    options.selected_lines = 1;
    expected = "a\nb \n";
  }
  CHECK(NativeEditStart(&f, source, 0U, 0U, !strcmp(name, "readonly")));
  ConstructorRetirement retirement = {&f, !strcmp(name, "constructor-retire")};
  gulong constructor_signal = retirement.armed
                                  ? g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                                     G_CALLBACK(ConstructorChanged), &retirement)
                                  : 0U;
  UmiStatus status = UmiGtk4AdapterReviewWhitespace(f.adapter, 0U, &options);
  if (constructor_signal)
    g_signal_handler_disconnect(gtk_window_get_toplevels(), constructor_signal);
  if (!strcmp(name, "constructor-retire")) {
    CHECK(status == UMI_STATUS_CANCELLED && f.adapter == NULL && NativeEditText(&f, source));
    goto cleanup;
  }
  if (!strcmp(name, "readonly")) {
    CHECK(status == UMI_STATUS_PERMISSION_DENIED && Form() == NULL);
    goto cleanup;
  }
  CHECK(status == UMI_STATUS_OK);
  form = Form();
  CHECK(form);
  CHECK(UmiGtk4WhitespaceReviewStatus(form, &applied) == UMI_STATUS_UNAVAILABLE && !applied);
  CHECK(Find(GTK_WIDGET(form), "document.whitespace.comparison"));
  held_apply = g_object_ref(Find(GTK_WIDGET(form), "document.whitespace.apply"));
  held_message = g_object_ref(Find(GTK_WIDGET(form), "document.whitespace.message"));
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4AdapterReviewWhitespace(NULL, 0U, &options) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4WhitespaceReviewStatus(form, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  CHECK(UmiGtk4AdapterReviewWhitespace(f.adapter, f.id, &options) == UMI_STATUS_BUSY);
  if (!strcmp(name, "entry") || !strcmp(name, "cancel")) {
    gtk_window_destroy(form);
    CHECK(NativeEditText(&f, source));
    goto cleanup;
  }
  if (!strcmp(name, "stale")) {
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.view, &view) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "changed", 7U) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "inactive")) {
    char view[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(f.documents, "other.c", view, sizeof view) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "closed-document"))
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
  if (!strcmp(name, "retired")) {
    umi_gtk4_adapter_destroy(f.adapter);
    f.adapter = NULL;
  }
  if (!strcmp(name, "rebound"))
    CHECK(UmiGtk4AdapterBindDocumentEditing(f.adapter, f.documents, NativeEditCompleted, &f) ==
          UMI_STATUS_OK);
  if (!strcmp(name, "completion-retire"))
    f.retire_on_completion = 1;
  if (!strcmp(name, "message-retire"))
    signal = g_signal_connect(held_message, "notify::label", G_CALLBACK(Retire), &f);
  g_signal_emit_by_name(held_apply, "clicked");
  status = UmiGtk4WhitespaceReviewStatus(form, &applied);
  if (!strcmp(name, "retired"))
    CHECK(!applied && NativeEditText(&f, source));
  else if (!strcmp(name, "rebound"))
    CHECK(status == UMI_STATUS_CANCELLED && !applied && NativeEditText(&f, source));
  else if (!strcmp(name, "closed-document"))
    CHECK(status == UMI_STATUS_NOT_FOUND && !applied);
  else if (!strcmp(name, "stale"))
    CHECK(status == UMI_STATUS_INVALID_STATE && !applied && NativeEditText(&f, "changed"));
  else {
    CHECK(applied && NativeEditText(&f, expected));
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
          NativeEditText(&f, source));
  }
  if (!strcmp(name, "repeat")) {
    g_signal_emit_by_name(held_apply, "clicked");
    CHECK(UmiGtk4WhitespaceReviewStatus(form, &applied) == UMI_STATUS_ALREADY_EXISTS && applied);
  }
cleanup:
  if (signal && held_message)
    g_signal_handler_disconnect(held_message, signal);
  if (form && UmiGtk4WindowIsOpen(form))
    gtk_window_destroy(form);
  g_clear_object(&held_apply);
  g_clear_object(&held_message);
  g_clear_object(&form);
  NativeEditStop(&f);
  return failed;
}
