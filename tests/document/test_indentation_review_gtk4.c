/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_indentation_review_gtk4.c
 *
 * PURPOSE:
 *   Verify explicit document preference review, stale-choice rejection and weak host ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "native_edit_fixture.h"
#include "umicom/ui/gtk4/document_indentation.h"
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
  for (guint i = 0; i < g_list_model_get_n_items(windows); ++i) {
    GtkWindow *window = g_list_model_get_item(windows, i);
    if (!strcmp(gtk_widget_get_name(GTK_WIDGET(window)), "document.indentation.review") &&
        UmiGtk4WindowIsOpen(window))
      return window;
    g_object_unref(window);
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
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *names[] = {"entry",         "apply",          "inherit",           "invalid",
                         "stale-default", "stale-override", "inactive",          "closed-document",
                         "readonly",      "retired",        "rebound",           "busy",
                         "cancel",        "repeat",         "completion-retire", "message-retire",
                         "arguments"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  NativeEditFixture f = {0};
  GtkWindow *form = NULL;
  int failed = 0;
  gulong signal = 0;
  GtkWidget *message = NULL, *heldApply = NULL, *heldMessage = NULL;
  CHECK(NativeEditStart(&f, "ab", 1U, 0U, !strcmp(name, "readonly")));
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4AdapterReviewDocumentIndentation(NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4AdapterReviewDocumentIndentation(f.adapter, UINT64_MAX) == UMI_STATUS_NOT_FOUND);
    goto cleanup;
  }
  UmiDocumentIndentationReview review;
  UmiEditorIndentation initial = {8U, 0};
  if (!strcmp(name, "inherit")) {
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &review, &initial, NULL) ==
          UMI_STATUS_OK);
  }
  CHECK(UmiGtk4AdapterReviewDocumentIndentation(f.adapter, 0U) == UMI_STATUS_OK);
  form = Form();
  CHECK(form != NULL);
  GtkWidget *width = Find(GTK_WIDGET(form), "document.indentation.width"),
            *spaces = Find(GTK_WIDGET(form), "document.indentation.spaces"),
            *apply = Find(GTK_WIDGET(form), "document.indentation.apply");
  message = Find(GTK_WIDGET(form), "document.indentation.message");
  CHECK(width && spaces && apply && message);
  heldApply = g_object_ref(apply);
  heldMessage = g_object_ref(message);
  if (!strcmp(name, "entry")) {
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), "inherits"));
    goto cleanup;
  }
  if (!strcmp(name, "busy")) {
    CHECK(UmiGtk4AdapterReviewDocumentIndentation(f.adapter, f.id) == UMI_STATUS_BUSY);
    goto cleanup;
  }
  if (!strcmp(name, "cancel") || !strcmp(name, "repeat")) {
    gtk_window_destroy(form);
    if (!strcmp(name, "repeat")) {
      CHECK(UmiGtk4AdapterReviewDocumentIndentation(f.adapter, f.id) == UMI_STATUS_OK);
      GtkWindow *second = Form();
      CHECK(second != NULL && second != form);
      gtk_window_destroy(second);
      g_object_unref(second);
    }
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK &&
          !review.overridden);
    goto cleanup;
  }
  gtk_editable_set_text(GTK_EDITABLE(width), !strcmp(name, "invalid") ? "99" : "2");
  gtk_check_button_set_active(GTK_CHECK_BUTTON(spaces), TRUE);
  if (!strcmp(name, "stale-default"))
    CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &initial) == UMI_STATUS_OK);
  if (!strcmp(name, "stale-override")) {
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &review, &initial, NULL) ==
          UMI_STATUS_OK);
  }
  UmiDocumentId otherId = 0U;
  if (!strcmp(name, "inactive")) {
    char other[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_new(f.documents, "other.c", other, sizeof other) ==
          UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &snapshot) == UMI_STATUS_OK);
    otherId = snapshot.document_id;
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
    signal = g_signal_connect(message, "notify::label", G_CALLBACK(Retire), &f);
  g_signal_emit_by_name(
      !strcmp(name, "inherit") ? Find(GTK_WIDGET(form), "document.indentation.inherit") : apply,
      "clicked");
  UmiStatus wanted = !strcmp(name, "invalid") ? UMI_STATUS_INVALID_ARGUMENT
                     : (!strcmp(name, "stale-default") || !strcmp(name, "stale-override"))
                         ? UMI_STATUS_INVALID_STATE
                     : !strcmp(name, "closed-document") ? UMI_STATUS_NOT_FOUND
                                                        : UMI_STATUS_OK;
  int retired =
      !strcmp(name, "retired") || !strcmp(name, "rebound") || !strcmp(name, "message-retire");
  if (!retired)
    CHECK(f.calls == 1U && f.status == wanted);
  else
    CHECK(f.calls == 0U);
  if (strcmp(name, "closed-document")) {
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK);
    int applied = wanted == UMI_STATUS_OK && strcmp(name, "retired") && strcmp(name, "rebound");
    if (applied) {
      CHECK(review.overridden == !!strcmp(name, "inherit"));
      CHECK(review.effective.tab_width == (!strcmp(name, "inherit") ? 4U : 2U));
    } else if (!strcmp(name, "stale-override"))
      CHECK(review.overridden && review.effective.tab_width == 8U);
    else
      CHECK(!review.overridden);
    CHECK(NativeEditText(&f, "ab"));
  }
  if (otherId) {
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, otherId, &review) ==
              UMI_STATUS_OK &&
          !review.overridden);
  }
cleanup:
  if (signal)
    g_signal_handler_disconnect(message, signal);
  if (form) {
    if (UmiGtk4WindowIsOpen(form))
      gtk_window_destroy(form);
    g_object_unref(form);
  }
  g_clear_object(&heldApply);
  g_clear_object(&heldMessage);
  NativeEditStop(&f);
  return failed;
}
