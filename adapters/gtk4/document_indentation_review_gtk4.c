/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_indentation_review_gtk4.c
 *
 * PURPOSE:
 *   Apply reviewed per-document indentation through weak editing ownership and retained native
 * controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"
#include "umicom/ui/gtk4/document_indentation.h"
#include "umicom/ui/gtk4/editor_indentation.h"
#include "umicom/ui/gtk4/indentation_preferences.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <string.h>
typedef struct DocumentIndentationForm {
  GObject *lifetime;
  GtkWidget *controls, *message;
  UmiDocumentIndentationReview review;
  int busy;
} DocumentIndentationForm;
static void DocumentIndentationWeakFree(gpointer data) {
  GWeakRef *reference = data;
  g_weak_ref_clear(reference);
  g_free(reference);
}
static void DocumentIndentationFormFree(gpointer data) {
  DocumentIndentationForm *state = data;
  g_object_unref(state->lifetime);
  g_free(state);
}
static void DocumentIndentationClose(GtkButton *button, GtkWindow *window) {
  (void)button;
  gtk_window_destroy(window);
}
static UmiGtk4Adapter *DocumentIndentationOwner(DocumentIndentationForm *state) {
  UmiGtk4Adapter *owner = g_object_get_data(state->lifetime, "adapter");
  return owner != NULL && owner->edit_coordinator == state->review.owner ? owner : NULL;
}
static int DocumentIndentationBusy(UmiGtk4Adapter *owner) {
  return owner->applying_document_state || owner->edit_cancel != NULL ||
         UmiGtk4AdapterDocumentSaveAllBusy(owner) || UmiGtk4AdapterDocumentCloseBusy(owner) ||
         UmiGtk4AdapterReplacementReviewBusy(owner) || UmiGtk4AdapterDocumentProposalBusy(owner) ||
         UmiGtk4AdapterCompletionReviewBusy(owner);
}
static void DocumentIndentationApply(GtkButton *button, GtkWindow *window) {
  DocumentIndentationForm *state =
      g_object_get_data(G_OBJECT(window), "umicom-document-indentation");
  if (state == NULL || state->busy || !UmiGtk4WindowIsOpen(window))
    return;
  g_object_ref(window);
  state->busy = 1;
  int inherit = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "inherit"));
  UmiEditorIndentation choice;
  UmiStatus status =
      inherit ? UMI_STATUS_OK : UmiGtk4IndentationPreferencesRead(state->controls, &choice);
  UmiGtk4Adapter *owner = DocumentIndentationOwner(state);
  int applied = 0;
  if (owner == NULL)
    status = UMI_STATUS_CANCELLED;
  else if (DocumentIndentationBusy(owner))
    status = UMI_STATUS_BUSY;
  else if (status == UMI_STATUS_OK) {
    status = UmiDocumentCoordinatorApplyIndentation(owner->edit_coordinator, &state->review,
                                                    inherit ? NULL : &choice, &state->review);
    if (status == UMI_STATUS_OK) {
      applied = 1;
      /* The service mutation has completed before any GTK notification.
       * Retirement during refresh cannot turn it into an unapplied change. */
      status = UmiGtk4AdapterRefreshIndentation(owner);
    }
  }
  owner = DocumentIndentationOwner(state);
  UmiGtk4DocumentEditResultFn completed = owner != NULL ? owner->edit_completed : NULL;
  void *context = owner != NULL ? owner->edit_context : NULL;
  if (UmiGtk4WindowIsOpen(window)) {
    gchar *message =
        applied
            ? g_strdup_printf("%s %u columns, %s. Existing text and files were not changed.%s",
                              state->review.overridden ? "Applied to this document:"
                                                       : "Using session defaults:",
                              state->review.effective.tab_width,
                              state->review.effective.insert_spaces ? "spaces" : "tabs",
                              status == UMI_STATUS_OK
                                  ? ""
                                  : " Native refresh was interrupted; refresh the editor again.")
            : g_strdup(
                  status == UMI_STATUS_INVALID_STATE
                      ? "Settings changed elsewhere. Close and reopen this form before applying."
                  : status == UMI_STATUS_NOT_FOUND
                      ? "This document has closed. No settings were changed."
                  : status == UMI_STATUS_CANCELLED
                      ? "The editor binding has retired. No settings were changed."
                  : status == UMI_STATUS_BUSY
                      ? "Finish the current document operation, then try again."
                      : "Enter a whole tab width from 1 to 16. No settings were changed.");
    gtk_label_set_text(GTK_LABEL(state->message), message);
    g_free(message);
  }
  /* Message observers may retire/rebind the owner too. Completion is last. */
  owner = DocumentIndentationOwner(state);
  if (owner != NULL && owner->edit_completed == completed && owner->edit_context == context &&
      completed != NULL)
    completed(context, status);
  state->busy = 0;
  g_object_unref(window);
}
UmiStatus UmiGtk4AdapterReviewDocumentIndentation(UmiGtk4Adapter *adapter, UmiDocumentId id) {
  if (adapter == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL ||
      adapter->window == NULL || !UmiGtk4WindowIsOpen(adapter->window))
    return UMI_STATUS_UNAVAILABLE;
  GObject *lifetime = g_object_ref(adapter->edit_lifetime);
  GWeakRef *previous = g_object_get_data(lifetime, "document-indentation-window");
  GtkWindow *existing = previous != NULL ? g_weak_ref_get(previous) : NULL;
  const int alreadyOpen = existing != NULL && UmiGtk4WindowIsOpen(existing);
  g_clear_object(&existing);
  if (GPOINTER_TO_INT(g_object_get_data(lifetime, "document-indentation-opening")) || alreadyOpen ||
      DocumentIndentationBusy(adapter)) {
    g_object_unref(lifetime);
    return UMI_STATUS_BUSY;
  }
  UmiDocumentWorkingCopySnapshot captured = {0};
  UmiStatus status = UMI_STATUS_NOT_FOUND;
  for (size_t i = 0U; i < umi_document_coordinator_count(adapter->edit_coordinator); ++i) {
    UmiDocumentWorkingCopySnapshot candidate;
    status = umi_document_coordinator_at(adapter->edit_coordinator, i, &candidate);
    if (status != UMI_STATUS_OK)
      break;
    status = UMI_STATUS_NOT_FOUND;
    if ((id != 0U && candidate.document_id == id) || (id == 0U && candidate.active)) {
      captured = candidate;
      status = UMI_STATUS_OK;
      break;
    }
  }
  UmiDocumentIndentationReview review;
  if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorInspectIndentation(adapter->edit_coordinator,
                                                      captured.document_id, &review);
  if (status != UMI_STATUS_OK) {
    g_object_unref(lifetime);
    return status;
  }
  GtkWindow *parent = g_object_ref(adapter->window);
  g_object_set_data(lifetime, "document-indentation-opening", GINT_TO_POINTER(1));
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  g_object_ref(window);
  DocumentIndentationForm *state = g_new0(DocumentIndentationForm, 1);
  state->lifetime = g_object_ref(lifetime);
  state->review = review;
  g_object_set_data_full(G_OBJECT(window), "umicom-document-indentation", state,
                         DocumentIndentationFormFree);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_set_margin_start(box, 12);
  gtk_widget_set_margin_end(box, 12);
  gtk_widget_set_margin_top(box, 12);
  gtk_widget_set_margin_bottom(box, 12);
  gchar *validName = g_utf8_make_valid(captured.display_name, -1);
  gchar *title = g_strdup_printf("Indentation: %s", validName);
  gtk_window_set_title(window, title);
  g_free(title);
  g_free(validName);
  gtk_widget_set_name(GTK_WIDGET(window), "document.indentation.review");
  GtkWidget *help =
      gtk_label_new("Apply to this captured document, or use session defaults. Switching tabs does "
                    "not change the target. Overrides last until the document closes.");
  gtk_label_set_wrap(GTK_LABEL(help), TRUE);
  gtk_box_append(GTK_BOX(box), help);
  status = UmiGtk4IndentationPreferencesCreate(&review.effective, &state->controls);
  if (status == UMI_STATUS_OK) {
    gtk_widget_set_name(UmiGtk4IndentationPreferencesWidth(state->controls),
                        "document.indentation.width");
    gtk_widget_set_name(UmiGtk4IndentationPreferencesSpaces(state->controls),
                        "document.indentation.spaces");
    gtk_box_append(GTK_BOX(box), state->controls);
    state->message =
        gtk_label_new(review.overridden ? "This document has an override."
                                        : "This document currently inherits session defaults.");
    gtk_label_set_wrap(GTK_LABEL(state->message), TRUE);
    gtk_widget_set_name(state->message, "document.indentation.message");
    gtk_box_append(GTK_BOX(box), state->message);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *apply = gtk_button_new_with_label("Apply to this document");
    GtkWidget *inherit = gtk_button_new_with_label("Use session defaults");
    GtkWidget *close = gtk_button_new_with_label("Close");
    gtk_widget_set_name(apply, "document.indentation.apply");
    gtk_widget_set_name(inherit, "document.indentation.inherit");
    gtk_widget_set_name(close, "document.indentation.close");
    g_object_set_data(G_OBJECT(inherit), "inherit", GINT_TO_POINTER(1));
    g_signal_connect_object(apply, "clicked", G_CALLBACK(DocumentIndentationApply), window, 0);
    g_signal_connect_object(inherit, "clicked", G_CALLBACK(DocumentIndentationApply), window, 0);
    g_signal_connect_object(close, "clicked", G_CALLBACK(DocumentIndentationClose), window, 0);
    gtk_box_append(GTK_BOX(actions), apply);
    gtk_box_append(GTK_BOX(actions), inherit);
    gtk_box_append(GTK_BOX(actions), close);
    gtk_box_append(GTK_BOX(box), actions);
  }
  gtk_window_set_child(window, box);
  gtk_window_set_default_size(window, 540, 300);
  gtk_window_set_transient_for(window, parent);
  gtk_window_set_destroy_with_parent(window, TRUE);
  UmiGtk4Adapter *owner = DocumentIndentationOwner(state);
  if (owner == NULL || !UmiGtk4WindowIsOpen(parent) || !UmiGtk4WindowIsOpen(window))
    status = UMI_STATUS_CANCELLED;
  else if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorCheckIndentation(owner->edit_coordinator, &review);
  if (status == UMI_STATUS_OK) {
    /* A weak reference avoids both a token/window cycle and stale pointers
     * when a destroyed form is retained by automation or a caller. */
    GWeakRef *reference = g_new0(GWeakRef, 1);
    g_weak_ref_init(reference, window);
    g_object_set_data_full(lifetime, "document-indentation-window", reference,
                           DocumentIndentationWeakFree);
    gtk_window_present(window);
    if (DocumentIndentationOwner(state) == NULL || !UmiGtk4WindowIsOpen(window))
      status = UMI_STATUS_CANCELLED;
  }
  g_object_set_data(lifetime, "document-indentation-opening", NULL);
  if (status != UMI_STATUS_OK && UmiGtk4WindowIsOpen(window))
    gtk_window_destroy(window);
  g_object_unref(parent);
  g_object_unref(lifetime);
  g_object_unref(window);
  return status;
}
