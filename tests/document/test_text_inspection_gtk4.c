/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_text_inspection_gtk4.c
 *
 * PURPOSE:
 *   Exercise immutable native source inspection through actual GTK controls and owner retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "native_edit_fixture.h"
#include "umicom/ui/gtk4/document_inspection.h"
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
    if (!strcmp(gtk_widget_get_name(GTK_WIDGET(w)), "document.text-inspection") &&
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
  const char *name = argv[1];
  const char *names[] = {"entry",
                         "readonly",
                         "selection",
                         "retained",
                         "edited",
                         "inactive",
                         "repeat",
                         "close-reopen",
                         "unbind",
                         "parent-close",
                         "constructor-retire",
                         "arguments"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  NativeEditFixture f = {0};
  GtkWindow *form = NULL;
  CHECK(NativeEditStart(&f, "a\n\xc3\xa9\xe2\x80\xae", !strcmp(name, "selection") ? 2U : 0U,
                        !strcmp(name, "selection") ? 2U : 0U, !strcmp(name, "readonly")));
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4AdapterInspectDocumentText(NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  ConstructorRetirement retirement = {&f, !strcmp(name, "constructor-retire")};
  gulong observer = 0U;
  if (retirement.armed)
    observer = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                G_CALLBACK(ConstructorChanged), &retirement);
  UmiStatus status = UmiGtk4AdapterInspectDocumentText(f.adapter, 0U);
  if (observer)
    g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
  /* Opening an observation, including interrupted construction, must leave
   * the source working copy untouched. Check it before deliberate edits. */
  CHECK(NativeEditText(&f, "a\n\xc3\xa9\xe2\x80\xae"));
  if (!strcmp(name, "constructor-retire")) {
    CHECK(status == UMI_STATUS_CANCELLED && Form() == NULL);
    goto cleanup;
  }
  CHECK(status == UMI_STATUS_OK);
  form = Form();
  CHECK(form != NULL);
  CHECK(GTK_IS_GRID(Find(GTK_WIDGET(form), "text.inspection.metrics")));
  GtkWidget *notices = Find(GTK_WIDGET(form), "text.inspection.notices");
  CHECK(GTK_IS_TEXT_VIEW(notices) && !gtk_text_view_get_editable(GTK_TEXT_VIEW(notices)));
  if (!strcmp(name, "repeat"))
    CHECK(UmiGtk4AdapterInspectDocumentText(f.adapter, 0U) == UMI_STATUS_BUSY);
  if (!strcmp(name, "edited")) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(f.editor));
    gtk_text_buffer_set_text(buffer, "later", -1);
  }
  if (!strcmp(name, "inactive"))
    CHECK(umi_document_coordinator_new(f.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
  if (!strcmp(name, "unbind"))
    CHECK(UmiGtk4AdapterBindDocumentEditing(f.adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
  if (!strcmp(name, "parent-close"))
    gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(f.adapter)));
  if (!strcmp(name, "retained") || !strcmp(name, "close-reopen"))
    g_signal_emit_by_name(Find(GTK_WIDGET(form), "document.text-inspection.close"), "clicked");
  UmiDocumentInspectionInfo info;
  UmiEditorTextInspectionSummary summary;
  CHECK(UmiGtk4DocumentInspectionRead(form, &info, &summary) == UMI_STATUS_OK);
  CHECK(info.document_id == f.id && summary.document.bytes == 7U && summary.notice_count == 1U &&
        f.calls == 0U);
  if (!strcmp(name, "readonly"))
    CHECK(info.read_only);
  if (!strcmp(name, "selection"))
    CHECK(summary.selection.scalars == 1U && summary.selection.bytes == 2U);
  if (!strcmp(name, "close-reopen")) {
    CHECK(UmiGtk4AdapterInspectDocumentText(f.adapter, 0U) == UMI_STATUS_OK);
    GtkWindow *next = Form();
    CHECK(next != NULL && next != form);
    gtk_window_destroy(next);
    g_object_unref(next);
  }
cleanup:
  if (form) {
    if (UmiGtk4WindowIsOpen(form))
      gtk_window_destroy(form);
    g_object_unref(form);
  }
  NativeEditStop(&f);
  return failed;
}
