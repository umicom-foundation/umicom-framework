/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_language_override_gtk4.c
 *
 * PURPOSE:
 *   Exercise document language review through actual controls, stale observations and native owner
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
#include "umicom/ui/gtk4/document_language.h"
#include "umicom/ui/gtk4/language_choice.h"
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
    if (!strcmp(gtk_widget_get_name(GTK_WIDGET(w)), "document.language.review") &&
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
  const char *name = argv[1], *cases[] = {"entry",
                                          "cancel",
                                          "apply",
                                          "automatic",
                                          "repeat",
                                          "stale",
                                          "text-change",
                                          "inactive",
                                          "closed",
                                          "readonly",
                                          "unbind",
                                          "parent-close",
                                          "completion-retire",
                                          "constructor-retire",
                                          "arguments"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    known |= !strcmp(name, cases[i]);
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  NativeEditFixture f = {0};
  GtkWindow *form = NULL;
  CHECK(NativeEditStart(&f, "alpha\nbeta", 2U, 3U, !strcmp(name, "readonly")));
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4AdapterReviewDocumentLanguage(NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  ConstructorRetirement retirement = {&f, !strcmp(name, "constructor-retire")};
  gulong observer = 0U;
  if (retirement.armed)
    observer = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                G_CALLBACK(ConstructorChanged), &retirement);
  UmiStatus opened = UmiGtk4AdapterReviewDocumentLanguage(f.adapter, 0U);
  if (observer)
    g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
  if (!strcmp(name, "constructor-retire")) {
    CHECK(opened == UMI_STATUS_CANCELLED);
    goto cleanup;
  }
  CHECK(opened == UMI_STATUS_OK);
  form = Form();
  CHECK(form);
  GtkWidget *choice = Find(GTK_WIDGET(form), "document.language.choice"),
            *apply = Find(GTK_WIDGET(form), "document.language.apply");
  CHECK(GTK_IS_DROP_DOWN(choice) && GTK_IS_BUTTON(apply));
  UmiDocumentLanguageReview initial, current;
  CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, f.id, &initial) == UMI_STATUS_OK);
  CHECK(UmiGtk4LanguageChoiceSelect(choice, "python3") == UMI_STATUS_OK);
  if (!strcmp(name, "repeat")) {
    CHECK(UmiGtk4AdapterReviewDocumentLanguage(f.adapter, 0U) == UMI_STATUS_BUSY);
    goto cleanup;
  }
  if (!strcmp(name, "entry"))
    goto cleanup;
  if (!strcmp(name, "cancel")) {
    g_signal_emit_by_name(Find(GTK_WIDGET(form), "document.language.close"), "clicked");
    CHECK(UmiDocumentCoordinatorCheckLanguage(f.documents, &initial) == UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "stale"))
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &initial, "sql", &current) ==
          UMI_STATUS_OK);
  if (!strcmp(name, "text-change"))
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(f.editor)), "pending", 7);
  if (!strcmp(name, "inactive"))
    CHECK(umi_document_coordinator_new(f.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
  if (!strcmp(name, "closed"))
    CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
  if (!strcmp(name, "unbind"))
    CHECK(UmiGtk4AdapterBindDocumentEditing(f.adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
  if (!strcmp(name, "parent-close"))
    gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(f.adapter)));
  f.retire_on_completion = !strcmp(name, "completion-retire");
  g_signal_emit_by_name(apply, "clicked");
  if (!strcmp(name, "closed")) {
    CHECK(f.status == UMI_STATUS_NOT_FOUND);
    goto cleanup;
  }
  CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, f.id, &current) == UMI_STATUS_OK);
  if (!strcmp(name, "stale"))
    CHECK(!strcmp(current.effective.language_id, "sql") && f.status == UMI_STATUS_INVALID_STATE);
  else if (!strcmp(name, "unbind") || !strcmp(name, "parent-close"))
    CHECK(!current.overridden && f.calls == 0U);
  else {
    CHECK(current.overridden && !strcmp(current.effective.language_id, "python3") &&
          f.status == UMI_STATUS_OK && f.calls == 1U);
    CHECK(NativeEditText(&f, !strcmp(name, "text-change") ? "pending" : "alpha\nbeta"));
    if (!strcmp(name, "automatic")) {
      g_signal_emit_by_name(Find(GTK_WIDGET(form), "document.language.inherit"), "clicked");
      CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, f.id, &current) == UMI_STATUS_OK &&
            !current.overridden && !strcmp(current.effective.language_id, "c"));
    }
  }
  if (f.retire_on_completion)
    CHECK(f.adapter == NULL);
cleanup:
  if (form) {
    if (UmiGtk4WindowIsOpen(form))
      gtk_window_destroy(form);
    g_object_unref(form);
  }
  NativeEditStop(&f);
  return failed;
}
