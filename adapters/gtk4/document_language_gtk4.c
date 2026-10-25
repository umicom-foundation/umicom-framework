/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_language_gtk4.c
 *
 * PURPOSE:
 *   Apply reviewed language identity through weak editing ownership without changing source text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"
#include "umicom/ui/gtk4/document_language.h"

#include "umicom/ui/gtk4/language_choice.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <string.h>
typedef struct DocumentLanguageForm {
  GObject *lifetime;
  GtkWidget *controls, *message;
  UmiDocumentLanguageReview review;
  int busy;
} DocumentLanguageForm;
static void DocumentLanguageWeakFree(gpointer data) {
  GWeakRef *reference = data;
  g_weak_ref_clear(reference);
  g_free(reference);
}
static void DocumentLanguageFormFree(gpointer data) {
  DocumentLanguageForm *state = data;
  g_object_unref(state->lifetime);
  g_free(state);
}
static void DocumentLanguageClose(GtkButton *button, GtkWindow *window) {
  (void)button;
  gtk_window_destroy(window);
}
static UmiGtk4Adapter *DocumentLanguageOwner(DocumentLanguageForm *state) {
  UmiGtk4Adapter *owner = g_object_get_data(state->lifetime, "adapter");
  return owner != NULL && owner->edit_coordinator == state->review.owner ? owner : NULL;
}
static int DocumentLanguageBusy(UmiGtk4Adapter *owner) {
  return owner->applying_document_state || owner->edit_cancel != NULL ||
         UmiGtk4AdapterDocumentSaveAllBusy(owner) || UmiGtk4AdapterDocumentCloseBusy(owner) ||
         UmiGtk4AdapterReplacementReviewBusy(owner) || UmiGtk4AdapterDocumentProposalBusy(owner) ||
         UmiGtk4AdapterCompletionReviewBusy(owner);
}
static void DocumentLanguageApply(GtkButton *button, GtkWindow *window) {
  DocumentLanguageForm *state = g_object_get_data(G_OBJECT(window), "umicom-document-language");
  if (state == NULL || state->busy || !UmiGtk4WindowIsOpen(window))
    return;
  g_object_ref(window);
  state->busy = 1;
  int inherit = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "inherit"));
  const UmiDocumentLanguageChoice *choice = NULL;
  UmiStatus status = inherit ? UMI_STATUS_OK : UmiGtk4LanguageChoiceRead(state->controls, &choice);
  UmiGtk4Adapter *owner = DocumentLanguageOwner(state);
  int applied = 0;
  if (owner == NULL)
    status = UMI_STATUS_CANCELLED;
  else if (DocumentLanguageBusy(owner))
    status = UMI_STATUS_BUSY;
  else if (status == UMI_STATUS_OK) {
    status =
        UmiDocumentCoordinatorApplyLanguage(owner->edit_coordinator, &state->review,
                                            inherit ? NULL : choice->language_id, &state->review);
    if (status == UMI_STATUS_OK) {
      applied = 1;
      /* The service mutation has completed before any GTK notification.
       * Retirement during refresh cannot turn it into an unapplied change. */
      status = umi_gtk4_refresh_documents(owner, umi_ui_application_shell_workbench(owner->shell));
    }
  }
  owner = DocumentLanguageOwner(state);
  UmiGtk4DocumentEditResultFn completed = owner != NULL ? owner->edit_completed : NULL;
  void *context = owner != NULL ? owner->edit_context : NULL;
  if (UmiGtk4WindowIsOpen(window)) {
    gchar *message =
        applied
            ? g_strdup_printf("%s %s. Text, selection and files were not changed.%s",
                              state->review.overridden ? "Language override:"
                                                       : "Filename language:",
                              state->review.effective.language_id,
                              status == UMI_STATUS_OK
                                  ? ""
                                  : " Native refresh was interrupted; refresh the editor again.")
            : g_strdup(status == UMI_STATUS_INVALID_STATE
                           ? "The language policy or filename changed. Close and reopen this form."
                       : status == UMI_STATUS_NOT_FOUND ? "The captured document has closed."
                       : status == UMI_STATUS_CANCELLED ? "The editor binding has retired."
                       : status == UMI_STATUS_BUSY
                           ? "Finish the current document operation, then try again."
                           : "Select a supported language. No policy was changed.");
    gtk_label_set_text(GTK_LABEL(state->message), message);
    g_free(message);
  }
  /* Message observers may retire/rebind the owner too. Completion is last. */
  owner = DocumentLanguageOwner(state);
  if (owner != NULL && owner->edit_completed == completed && owner->edit_context == context &&
      completed != NULL)
    completed(context, status);
  state->busy = 0;
  g_object_unref(window);
}
UmiStatus UmiGtk4AdapterReviewDocumentLanguage(UmiGtk4Adapter *adapter, UmiDocumentId id) {
  if (adapter == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL ||
      adapter->window == NULL || !UmiGtk4WindowIsOpen(adapter->window))
    return UMI_STATUS_UNAVAILABLE;
  GObject *lifetime = g_object_ref(adapter->edit_lifetime);
  GWeakRef *previous = g_object_get_data(lifetime, "document-language-window");
  GtkWindow *existing = previous != NULL ? g_weak_ref_get(previous) : NULL;
  const int alreadyOpen = existing != NULL && UmiGtk4WindowIsOpen(existing);
  g_clear_object(&existing);
  if (GPOINTER_TO_INT(g_object_get_data(lifetime, "document-language-opening")) || alreadyOpen ||
      DocumentLanguageBusy(adapter)) {
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
  UmiDocumentLanguageReview review;
  if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorInspectLanguage(adapter->edit_coordinator, captured.document_id,
                                                   &review);
  if (status != UMI_STATUS_OK) {
    g_object_unref(lifetime);
    return status;
  }
  GtkWindow *parent = g_object_ref(adapter->window);
  g_object_set_data(lifetime, "document-language-opening", GINT_TO_POINTER(1));
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  g_object_ref(window);
  /* Toplevel construction emits synchronously. A host observer may have
   * retired the editing binding before any form body is attached. */
  UmiGtk4Adapter *constructing = g_object_get_data(lifetime, "adapter");
  if (constructing == NULL || constructing->edit_coordinator != review.owner ||
      !UmiGtk4WindowIsOpen(parent) || !UmiGtk4WindowIsOpen(window)) {
    g_object_set_data(lifetime, "document-language-opening", NULL);
    if (UmiGtk4WindowIsOpen(window))
      gtk_window_destroy(window);
    g_object_unref(window);
    g_object_unref(parent);
    g_object_unref(lifetime);
    return UMI_STATUS_CANCELLED;
  }
  DocumentLanguageForm *state = g_new0(DocumentLanguageForm, 1);
  state->lifetime = g_object_ref(lifetime);
  state->review = review;
  g_object_set_data_full(G_OBJECT(window), "umicom-document-language", state,
                         DocumentLanguageFormFree);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  g_object_ref_sink(
      box); /* Constructor observers may close the parent; hold body until setup ends. */
  gtk_widget_set_margin_start(box, 12);
  gtk_widget_set_margin_end(box, 12);
  gtk_widget_set_margin_top(box, 12);
  gtk_widget_set_margin_bottom(box, 12);
  gchar *validName = g_utf8_make_valid(captured.display_name, -1);
  gchar *title = g_strdup_printf("Document language: %s", validName);
  gtk_window_set_title(window, title);
  g_free(title);
  g_free(validName);
  gtk_widget_set_name(GTK_WIDGET(window), "document.language.review");
  GtkWidget *help =
      gtk_label_new("Choose the syntax language for this captured document. Switching tabs does "
                    "not change the target. Overrides last until the document closes.");
  gtk_label_set_wrap(GTK_LABEL(help), TRUE);
  gtk_box_append(GTK_BOX(box), help);
  status = UmiGtk4LanguageChoiceCreate(review.effective.language_id, &state->controls);
  if (status == UMI_STATUS_OK) {
    gtk_box_append(GTK_BOX(box), state->controls);
    state->message =
        gtk_label_new(review.overridden ? "This document has an override."
                                        : "This document currently uses filename detection.");
    gtk_label_set_wrap(GTK_LABEL(state->message), TRUE);
    gtk_widget_set_name(state->message, "document.language.message");
    gtk_box_append(GTK_BOX(box), state->message);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *apply = gtk_button_new_with_label("Apply to this document");
    GtkWidget *inherit = gtk_button_new_with_label("Use filename detection");
    GtkWidget *close = gtk_button_new_with_label("Close");
    gtk_widget_set_name(apply, "document.language.apply");
    gtk_widget_set_name(inherit, "document.language.inherit");
    gtk_widget_set_name(close, "document.language.close");
    g_object_set_data(G_OBJECT(inherit), "inherit", GINT_TO_POINTER(1));
    g_signal_connect_object(apply, "clicked", G_CALLBACK(DocumentLanguageApply), window, 0);
    g_signal_connect_object(inherit, "clicked", G_CALLBACK(DocumentLanguageApply), window, 0);
    g_signal_connect_object(close, "clicked", G_CALLBACK(DocumentLanguageClose), window, 0);
    gtk_box_append(GTK_BOX(actions), apply);
    gtk_box_append(GTK_BOX(actions), inherit);
    gtk_box_append(GTK_BOX(actions), close);
    gtk_box_append(GTK_BOX(box), actions);
  }
  gtk_window_set_child(window, box);
  gtk_window_set_default_size(window, 540, 300);
  gtk_window_set_transient_for(window, parent);
  gtk_window_set_destroy_with_parent(window, TRUE);
  UmiGtk4Adapter *owner = DocumentLanguageOwner(state);
  if (owner == NULL || !UmiGtk4WindowIsOpen(parent) || !UmiGtk4WindowIsOpen(window))
    status = UMI_STATUS_CANCELLED;
  else if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorCheckLanguage(owner->edit_coordinator, &review);
  if (status == UMI_STATUS_OK) {
    /* A weak reference avoids both a token/window cycle and stale pointers
     * when a destroyed form is retained by automation or a caller. */
    GWeakRef *reference = g_new0(GWeakRef, 1);
    g_weak_ref_init(reference, window);
    g_object_set_data_full(lifetime, "document-language-window", reference,
                           DocumentLanguageWeakFree);
    gtk_window_present(window);
    if (DocumentLanguageOwner(state) == NULL || !UmiGtk4WindowIsOpen(window))
      status = UMI_STATUS_CANCELLED;
  }
  g_object_set_data(lifetime, "document-language-opening", NULL);
  if (status != UMI_STATUS_OK && UmiGtk4WindowIsOpen(window))
    gtk_window_destroy(window);
  g_object_unref(box);
  g_object_unref(parent);
  g_object_unref(lifetime);
  g_object_unref(window);
  return status;
}
