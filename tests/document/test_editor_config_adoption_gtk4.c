/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_editor_config_adoption_gtk4.c
 *
 * PURPOSE:
 *   Verify asynchronous reinspection, cancellation and irreversible policy completion are reported
 * accurately.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "editor_config_adoption_fixture.h"
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/ui/gtk4/editor_config_adoption.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
typedef struct NativeOwner {
  UmiGtk4Adapter *adapter;
  unsigned calls;
  UmiStatus status;
} NativeOwner;
static void Completed(void *data, UmiStatus status) {
  NativeOwner *owner = data;
  ++owner->calls;
  owner->status = status;
}
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
static GtkWidget *Editor(GtkWidget *root) {
  if (GTK_IS_TEXT_VIEW(root) && gtk_widget_has_css_class(root, "umicom-editor"))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = Editor(child);
    if (found)
      return found;
  }
  return NULL;
}
static GtkWindow *Form(void) {
  GListModel *windows = gtk_window_get_toplevels();
  for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
    GtkWindow *window = g_list_model_get_item(windows, i);
    if (!strcmp(gtk_widget_get_name(GTK_WIDGET(window)), "document.editor-config.review") &&
        UmiGtk4WindowIsOpen(window))
      return window;
    g_object_unref(window);
  }
  return NULL;
}
static void Drain(GtkWindow *window) {
  gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
  while (UmiGtk4EditorConfigAdoptionBusy(window) && g_get_monotonic_time() < deadline) {
    while (g_main_context_pending(NULL))
      g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  CHECK(!UmiGtk4EditorConfigAdoptionBusy(window));
}
static void Retire(GObject *object, GParamSpec *spec, gpointer data) {
  (void)object;
  (void)spec;
  NativeOwner *owner = data;
  if (owner->adapter) {
    umi_gtk4_adapter_destroy(owner->adapter);
    owner->adapter = NULL;
  }
}
static void CloseApplied(GObject *object, GParamSpec *spec, gpointer data) {
  (void)spec;
  if (strstr(gtk_label_get_text(GTK_LABEL(object)), "indentation applied"))
    gtk_window_destroy(GTK_WINDOW(data));
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *names[] = {"inspect",       "apply",          "invalid",
                         "unsupported",   "cancel-inspect", "cancel-apply",
                         "change-config", "add-config",     "stale-default",
                         "closed-target", "inactive",       "retire-inspect",
                         "retire-apply",  "close-inspect",  "close-apply",
                         "repeat",        "busy",           "not-inspected",
                         "arguments",     "refresh-retire", "applied-message-close"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  AdoptionFixture f = {0};
  const char *config = !strcmp(name, "invalid")       ? "[*]\nindent_size=0\n"
                       : !strcmp(name, "unsupported") ? "[*]\nindent_size=2\ntab_width=4\n"
                                                      : "[*]\nindent_size=2\n";
  AdoptionStartFixture(&f, config);
  GtkApplication *app =
      gtk_application_new("org.umicom.ConfigAdoptionTest", G_APPLICATION_NON_UNIQUE);
  CHECK(g_application_register(G_APPLICATION(app), NULL, NULL));
  UmiUiApplicationShell *shell = NULL;
  NativeOwner owner = {0};
  CHECK(umi_ui_application_shell_create("org.umicom.ConfigAdoptionTest", "Configuration adoption",
                                        f.workbench, &shell) == UMI_STATUS_OK);
  CHECK(umi_gtk4_adapter_create(app, &owner.adapter) == UMI_STATUS_OK);
  CHECK(umi_gtk4_adapter_prepare(owner.adapter, shell) == UMI_STATUS_OK);
  CHECK(UmiGtk4AdapterBindDocumentEditing(owner.adapter, f.documents, Completed, &owner) ==
        UMI_STATUS_OK);
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4AdapterReviewEditorConfig(NULL, f.root, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4AdapterReviewEditorConfig(owner.adapter, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4AdapterReviewEditorConfig(owner.adapter, f.root, UINT64_MAX) ==
          UMI_STATUS_NOT_FOUND);
    CHECK(UmiGtk4EditorConfigAdoptionStatus(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  CHECK(UmiGtk4AdapterReviewEditorConfig(owner.adapter, f.root, 0U) == UMI_STATUS_OK);
  GtkWindow *form = Form();
  CHECK(form);
  CHECK(UmiGtk4EditorConfigAdoptionStatus(form) == UMI_STATUS_UNAVAILABLE);
  GtkWidget *inspect = Find(GTK_WIDGET(form), "document.editor-config.inspect"),
            *apply = Find(GTK_WIDGET(form), "document.editor-config.apply"),
            *cancel = Find(GTK_WIDGET(form), "document.editor-config.cancel"),
            *message = Find(GTK_WIDGET(form), "document.editor-config.message");
  CHECK(inspect && apply && cancel && message);
  if (!strcmp(name, "not-inspected")) {
    g_signal_emit_by_name(apply, "clicked");
    CHECK(UmiGtk4EditorConfigAdoptionStatus(form) == UMI_STATUS_INVALID_STATE);
  } else {
    g_signal_emit_by_name(inspect, "clicked");
    CHECK(UmiGtk4EditorConfigAdoptionBusy(form));
    if (!strcmp(name, "busy"))
      CHECK(UmiGtk4AdapterReviewEditorConfig(owner.adapter, f.root, f.id) == UMI_STATUS_BUSY);
    if (!strcmp(name, "cancel-inspect"))
      g_signal_emit_by_name(cancel, "clicked");
    if (!strcmp(name, "retire-inspect")) {
      umi_gtk4_adapter_destroy(owner.adapter);
      owner.adapter = NULL;
    }
    if (!strcmp(name, "close-inspect"))
      gtk_window_destroy(form);
    Drain(form);
    UmiStatus expected = !strcmp(name, "invalid")       ? UMI_STATUS_PARSE_ERROR
                         : !strcmp(name, "unsupported") ? UMI_STATUS_NOT_IMPLEMENTED
                         : (!strcmp(name, "cancel-inspect") || !strcmp(name, "retire-inspect") ||
                            !strcmp(name, "close-inspect"))
                             ? UMI_STATUS_CANCELLED
                             : UMI_STATUS_OK;
    CHECK(UmiGtk4EditorConfigAdoptionStatus(form) == expected);
    if (expected == UMI_STATUS_OK && strcmp(name, "inspect") && strcmp(name, "busy")) {
      if (!strcmp(name, "change-config"))
        AdoptionSave(&f, ".editorconfig", "[*]\nindent_size=8\n");
      if (!strcmp(name, "add-config"))
        AdoptionSave(&f, "src/.editorconfig", "[*]\nindent_size=2\n");
      if (!strcmp(name, "stale-default")) {
        UmiEditorIndentation next = {8U, 0};
        CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &next) == UMI_STATUS_OK);
      }
      if (!strcmp(name, "closed-target"))
        CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
      UmiDocumentId other = 0U;
      if (!strcmp(name, "inactive")) {
        char view[UMI_UI_ID_CAPACITY];
        UmiDocumentWorkingCopySnapshot snapshot;
        CHECK(umi_document_coordinator_new(f.documents, "peer.c", view, sizeof view) ==
              UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &snapshot) == UMI_STATUS_OK);
        other = snapshot.document_id;
      }
      GtkWidget *observed = NULL;
      gulong handler = 0;
      if (!strcmp(name, "refresh-retire")) {
        observed = Editor(umi_gtk4_adapter_native_window(owner.adapter));
        CHECK(observed);
        g_object_ref(observed);
        const char *property =
            g_object_class_find_property(G_OBJECT_GET_CLASS(observed), "tab-width")
                ? "notify::tab-width"
                : "notify::tabs";
        handler = g_signal_connect(observed, property, G_CALLBACK(Retire), &owner);
      }
      if (!strcmp(name, "applied-message-close")) {
        observed = g_object_ref(message);
        handler = g_signal_connect(message, "notify::label", G_CALLBACK(CloseApplied), form);
      }
      g_signal_emit_by_name(apply, "clicked");
      if (!strcmp(name, "cancel-apply"))
        g_signal_emit_by_name(cancel, "clicked");
      if (!strcmp(name, "retire-apply")) {
        umi_gtk4_adapter_destroy(owner.adapter);
        owner.adapter = NULL;
      }
      if (!strcmp(name, "close-apply"))
        gtk_window_destroy(form);
      Drain(form);
      if (handler)
        g_signal_handler_disconnect(observed, handler);
      g_clear_object(&observed);
      int applied = !strcmp(name, "apply") || !strcmp(name, "inactive") ||
                    !strcmp(name, "repeat") || !strcmp(name, "refresh-retire") ||
                    !strcmp(name, "applied-message-close");
      expected = (!strcmp(name, "change-config") || !strcmp(name, "add-config") ||
                  !strcmp(name, "stale-default"))
                     ? UMI_STATUS_INVALID_STATE
                 : !strcmp(name, "closed-target") ? UMI_STATUS_NOT_FOUND
                 : (!strcmp(name, "cancel-apply") || !strcmp(name, "retire-apply") ||
                    !strcmp(name, "close-apply") || !strcmp(name, "refresh-retire") ||
                    !strcmp(name, "applied-message-close"))
                     ? UMI_STATUS_CANCELLED
                     : UMI_STATUS_OK;
      CHECK(UmiGtk4EditorConfigAdoptionStatus(form) == expected);
      if (strcmp(name, "closed-target")) {
        UmiDocumentIndentationReview review;
        CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) ==
              UMI_STATUS_OK);
        CHECK(review.overridden == applied);
        if (applied)
          CHECK(review.effective.tab_width == 2U);
        AdoptionExpectText(&f, "ab\nc");
        if (other) {
          CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, other, &review) ==
                    UMI_STATUS_OK &&
                !review.overridden);
        }
      }
      if (!strcmp(name, "repeat")) {
        g_signal_emit_by_name(apply, "clicked");
        CHECK(UmiGtk4EditorConfigAdoptionStatus(form) == UMI_STATUS_ALREADY_EXISTS);
      }
    } else if (strcmp(name, "closed-target")) {
      UmiDocumentIndentationReview review;
      CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK &&
            !review.overridden);
    }
  }
  if (UmiGtk4WindowIsOpen(form))
    gtk_window_destroy(form);
  g_object_unref(form);
cleanup:
  if (owner.adapter)
    umi_gtk4_adapter_destroy(owner.adapter);
  umi_ui_application_shell_destroy(shell);
  AdoptionStopFixture(&f);
  g_object_unref(app);
  return 0;
}
