/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_inspection_gtk4.c
 *
 * PURPOSE:
 *   Construct source-inspection windows with captured native lifetime and immutable document
 * ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"
#include "umicom/ui/gtk4/document_inspection.h"
#include "umicom/ui/gtk4/text_inspection.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
typedef struct InspectionForm {
  UmiDocumentInspection *inspection;
  GtkWidget *body;
} InspectionForm;
static void InspectionFormFree(gpointer data) {
  InspectionForm *form = data;
  UmiDocumentInspectionDestroy(form->inspection);
  g_clear_object(&form->body);
  g_free(form);
}
static void InspectionWeakFree(gpointer data) {
  GWeakRef *weak = data;
  g_weak_ref_clear(weak);
  g_free(weak);
}
static void InspectionClose(GtkButton *button, GtkWindow *window) {
  (void)button;
  gtk_window_destroy(window);
}
UmiStatus UmiGtk4DocumentInspectionRead(GtkWindow *window, UmiDocumentInspectionInfo *info,
                                        UmiEditorTextInspectionSummary *summary) {
  if (!GTK_IS_WINDOW(window) || info == NULL || summary == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  InspectionForm *form = g_object_get_data(G_OBJECT(window), "umicom-text-inspection");
  if (form == NULL)
    return UMI_STATUS_NOT_FOUND;
  UmiDocumentInspectionInfo i;
  UmiEditorTextInspectionSummary s;
  UmiStatus status = UmiDocumentInspectionInfoRead(form->inspection, &i);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextInspectionSummaryRead(UmiDocumentInspectionText(form->inspection), &s);
  if (status == UMI_STATUS_OK) {
    *info = i;
    *summary = s;
  }
  return status;
}
UmiStatus UmiGtk4AdapterInspectDocumentText(UmiGtk4Adapter *adapter, UmiDocumentId id) {
  if (adapter == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (adapter->window == NULL || adapter->edit_lifetime == NULL ||
      adapter->edit_coordinator == NULL || !UmiGtk4WindowIsOpen(adapter->window))
    return UMI_STATUS_UNAVAILABLE;
  GObject *token = g_object_ref(adapter->edit_lifetime);
  GWeakRef *weak = g_object_get_data(token, "text-inspection-window");
  GtkWindow *prior = weak != NULL ? g_weak_ref_get(weak) : NULL;
  int opened = prior != NULL && UmiGtk4WindowIsOpen(prior);
  g_clear_object(&prior);
  if (opened || g_object_get_data(token, "text-inspection-opening") != NULL ||
      adapter->applying_document_state || adapter->edit_cancel != NULL ||
      UmiGtk4AdapterDocumentSaveAllBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter)) {
    g_object_unref(token);
    return UMI_STATUS_BUSY;
  }
  UmiDocumentCoordinator *captured_owner = adapter->edit_coordinator;
  if (id == 0U) {
    UmiDocumentWorkingCopySnapshot current;
    UmiStatus status = umi_document_coordinator_active_snapshot(captured_owner, &current);
    if (status != UMI_STATUS_OK) {
      g_object_unref(token);
      return status;
    }
    id = current.document_id;
  }
  UmiDocumentInspection *inspection = NULL;
  UmiStatus status = UmiDocumentCoordinatorInspectText(captured_owner, id, &inspection);
  if (status != UMI_STATUS_OK) {
    g_object_unref(token);
    return status;
  }
  UmiDocumentInspectionInfo info;
  status = UmiDocumentInspectionInfoRead(inspection, &info);
  if (status != UMI_STATUS_OK) {
    UmiDocumentInspectionDestroy(inspection);
    g_object_unref(token);
    return status;
  }
  /* Capture everything needed from the adapter before GTK constructor observers
   * can close the parent or retire/rebind the editor. */
  GtkWindow *parent = g_object_ref(adapter->window);
  g_object_set_data(token, "text-inspection-opening", GINT_TO_POINTER(1));
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  g_object_ref(window);
  InspectionForm *form = g_new0(InspectionForm, 1);
  form->inspection = inspection;
  form->body = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
  g_object_set_data_full(G_OBJECT(window), "umicom-text-inspection", form, InspectionFormFree);
  gtk_widget_set_name(GTK_WIDGET(window), "document.text-inspection");
  gchar *name = g_utf8_make_valid(info.display_name, -1),
        *title = g_strdup_printf("Source inspection: %s", name);
  gtk_window_set_title(window, title);
  g_free(name);
  g_free(title);
  GtkWidget *contents = NULL;
  status = UmiGtk4TextInspectionCreate(UmiDocumentInspectionText(inspection), &contents);
  if (status == UMI_STATUS_OK)
    gtk_box_append(GTK_BOX(form->body), contents);
  GtkWidget *close = gtk_button_new_with_label("Close");
  gtk_widget_set_name(close, "document.text-inspection.close");
  g_signal_connect_object(close, "clicked", G_CALLBACK(InspectionClose), window, 0);
  gtk_box_append(GTK_BOX(form->body), close);
  gtk_window_set_child(window, form->body);
  gtk_window_set_default_size(window, 800, 750);
  gtk_window_set_default_widget(window, close);
  gtk_window_set_transient_for(window, parent);
  gtk_window_set_destroy_with_parent(window, TRUE);
  UmiGtk4Adapter *owner = g_object_get_data(token, "adapter");
  if (owner == NULL || owner->edit_coordinator != captured_owner || !UmiGtk4WindowIsOpen(parent) ||
      !UmiGtk4WindowIsOpen(window))
    status = UMI_STATUS_CANCELLED;
  if (status == UMI_STATUS_OK) {
    GWeakRef *next = g_new0(GWeakRef, 1);
    g_weak_ref_init(next, window);
    g_object_set_data_full(token, "text-inspection-window", next, InspectionWeakFree);
    gtk_window_present(window);
    owner = g_object_get_data(token, "adapter");
    if (owner == NULL || owner->edit_coordinator != captured_owner || !UmiGtk4WindowIsOpen(window))
      status = UMI_STATUS_CANCELLED;
  }
  g_object_set_data(token, "text-inspection-opening", NULL);
  if (status != UMI_STATUS_OK && UmiGtk4WindowIsOpen(window))
    gtk_window_destroy(window);
  g_object_unref(parent);
  g_object_unref(token);
  g_object_unref(window);
  return status;
}
