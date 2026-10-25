/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/native_edit_fixture.h
 *
 * PURPOSE:
 *   Share real GTK source editing setup with deterministic ownership and checked model reads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_NATIVE_EDIT_FIXTURE_H
#define UMICOM_NATIVE_EDIT_FIXTURE_H
#include "umicom/document/commands.h"
#include "umicom/ui/gtk4/document_commands.h"
#include <gtk/gtk.h>
#include <string.h>
typedef struct NativeEditFixture {
  UmiCommandRegistry *commands;
  UmiUiWorkbench *workbench;
  UmiUiApplicationShell *shell;
  UmiDocumentStore *store;
  UmiDocumentCoordinator *documents;
  UmiGtk4Adapter *adapter;
  GtkApplication *application;
  GtkWidget *editor;
  GtkEventController *keys;
  UmiDocumentId id;
  char view[UMI_UI_ID_CAPACITY];
  unsigned calls;
  UmiStatus status;
  int retire_on_completion;
} NativeEditFixture;
static void NativeEditCompleted(void *data, UmiStatus status) {
  NativeEditFixture *f = data;
  f->status = status;
  ++f->calls;
  if (f->retire_on_completion) {
    umi_gtk4_adapter_destroy(f->adapter);
    f->adapter = NULL;
  }
}
static GtkWidget *NativeEditFind(GtkWidget *root) {
  if (GTK_IS_TEXT_VIEW(root) && gtk_widget_has_css_class(root, "umicom-editor"))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = NativeEditFind(child);
    if (found != NULL)
      return found;
  }
  return NULL;
}
static GtkEventController *NativeEditKeys(GtkWidget *view) {
  GListModel *list = gtk_widget_observe_controllers(view);
  GtkEventController *found = NULL;
  for (guint i = 0U; i < g_list_model_get_n_items(list); ++i) {
    GtkEventController *item = g_list_model_get_item(list, i);
    if (g_strcmp0(gtk_event_controller_get_name(item), "umicom-source-editor-commands") == 0) {
      found = item;
      break;
    }
    g_object_unref(item);
  }
  g_object_unref(list);
  return found;
}
static int NativeEditText(NativeEditFixture *f, const char *expected) {
  char *text = NULL;
  size_t bytes = 0U;
  UmiStatus status = UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench),
                                                    f->view, &text, &bytes);
  int same = status == UMI_STATUS_OK && bytes == strlen(expected) && !strcmp(text, expected);
  UmiUiDocumentViewModelFreeText(text);
  return same;
}
/* The caller always calls Stop, including partial-start failure. */
static int NativeEditStart(NativeEditFixture *f, const char *text, size_t cursor, size_t selection,
                           int readonly) {
  f->application = gtk_application_new("org.umicom.native-edit-test", G_APPLICATION_NON_UNIQUE);
  if (!g_application_register(G_APPLICATION(f->application), NULL, NULL))
    return 0;
  if (umi_command_registry_create(&f->commands) != UMI_STATUS_OK ||
      umi_ui_workbench_create("native-edit", f->commands, &f->workbench) != UMI_STATUS_OK ||
      umi_ui_application_shell_create("org.umicom.native-edit-test", "Source editing", f->workbench,
                                      &f->shell) != UMI_STATUS_OK ||
      umi_document_store_create(&f->store) != UMI_STATUS_OK ||
      umi_document_coordinator_create(f->store, f->workbench, NULL, &f->documents) !=
          UMI_STATUS_OK ||
      umi_document_coordinator_new(f->documents, "test.c", f->view, sizeof f->view) !=
          UMI_STATUS_OK)
    return 0;
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
  UmiUiDocumentViewSnapshot view;
  if (umi_ui_document_view_model_find(views, f->view, &view) != UMI_STATUS_OK)
    return 0;
  view.cursor_offset = cursor;
  view.selection_length = selection;
  view.read_only = readonly;
  view.dirty = 1;
  if (UmiUiDocumentViewModelUpsertText(views, &view, text, strlen(text)) != UMI_STATUS_OK ||
      umi_document_coordinator_sync_active(f->documents) != UMI_STATUS_OK)
    return 0;
  UmiDocumentWorkingCopySnapshot target;
  if (umi_document_coordinator_active_snapshot(f->documents, &target) != UMI_STATUS_OK)
    return 0;
  f->id = target.document_id;
  if (umi_gtk4_adapter_create(f->application, &f->adapter) != UMI_STATUS_OK ||
      umi_gtk4_adapter_prepare(f->adapter, f->shell) != UMI_STATUS_OK ||
      UmiGtk4AdapterBindDocumentEditing(f->adapter, f->documents, NativeEditCompleted, f) !=
          UMI_STATUS_OK)
    return 0;
  f->editor = NativeEditFind(umi_gtk4_adapter_native_window(f->adapter));
  if (f->editor == NULL)
    return 0;
  g_object_ref(f->editor);
  f->keys = NativeEditKeys(f->editor);
  return f->keys != NULL;
}
static void NativeEditStop(NativeEditFixture *f) {
  if (f->adapter != NULL)
    umi_gtk4_adapter_destroy(f->adapter);
  g_clear_object(&f->keys);
  g_clear_object(&f->editor);
  umi_document_coordinator_destroy(f->documents);
  umi_document_store_destroy(f->store);
  umi_ui_application_shell_destroy(f->shell);
  umi_ui_workbench_destroy(f->workbench);
  umi_command_registry_destroy(f->commands);
  g_clear_object(&f->application);
}
#endif
