/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_whitespace_gtk4.c
 *
 * PURPOSE:
 *   Present complete source comparisons while retaining review identity across native callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"
#include "umicom/ui/gtk4/document_whitespace.h"
#include "umicom/ui/gtk4/text_comparison.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <string.h>
typedef struct WhitespaceForm {
  GObject *lifetime;
  UmiDocumentCoordinator *owner;
  UmiDocumentWhitespaceReview *review;
  GtkWidget *body, *message;
  UmiStatus status;
  int busy, applied;
} WhitespaceForm;
static void WhitespaceWeakFree(gpointer data) {
  GWeakRef *weak = data;
  g_weak_ref_clear(weak);
  g_free(weak);
}
static void WhitespaceFormFree(gpointer data) {
  WhitespaceForm *form = data;
  UmiDocumentWhitespaceReviewDestroy(form->review);
  g_clear_object(&form->body);
  g_object_unref(form->lifetime);
  g_free(form);
}
static UmiGtk4Adapter *WhitespaceOwner(WhitespaceForm *form) {
  UmiGtk4Adapter *owner = g_object_get_data(form->lifetime, "adapter");
  return owner != NULL && owner->edit_coordinator == form->owner ? owner : NULL;
}
static int WhitespaceBusy(UmiGtk4Adapter *owner) {
  return owner->applying_document_state || owner->edit_cancel != NULL ||
         UmiGtk4AdapterDocumentSaveAllBusy(owner) || UmiGtk4AdapterDocumentCloseBusy(owner) ||
         UmiGtk4AdapterReplacementReviewBusy(owner) || UmiGtk4AdapterDocumentProposalBusy(owner) ||
         UmiGtk4AdapterCompletionReviewBusy(owner);
}
UmiStatus UmiGtk4WhitespaceReviewStatus(GtkWindow *window, int *out_applied) {
  if (!GTK_IS_WINDOW(window) || out_applied == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  WhitespaceForm *form = g_object_get_data(G_OBJECT(window), "umicom-whitespace-review");
  if (form == NULL)
    return UMI_STATUS_NOT_FOUND;
  *out_applied = form->applied;
  return form->status;
}
static void WhitespaceClose(GtkButton *button, GtkWindow *window) {
  (void)button;
  gtk_window_destroy(window);
}
static void WhitespaceApply(GtkButton *button, GtkWindow *window) {
  (void)button;
  WhitespaceForm *form = g_object_get_data(G_OBJECT(window), "umicom-whitespace-review");
  if (form == NULL || form->busy || !UmiGtk4WindowIsOpen(window))
    return;
  g_object_ref(window);
  form->busy = 1;
  UmiGtk4Adapter *owner = WhitespaceOwner(form);
  UmiStatus status =
      owner == NULL ? UMI_STATUS_CANCELLED
      : WhitespaceBusy(owner)
          ? UMI_STATUS_BUSY
          : UmiDocumentWhitespaceReviewApply(owner->edit_coordinator, form->review, 1);
  if (status == UMI_STATUS_OK) {
    form->applied = 1;
    /* Capture the irreversible part before notifications. Refresh failure
     * cannot label the already committed draft as an unapplied operation. */
    status = umi_gtk4_refresh_documents(owner, umi_ui_application_shell_workbench(owner->shell));
  }
  owner = WhitespaceOwner(form);
  if (owner == NULL)
    status = UMI_STATUS_CANCELLED;
  form->status = status;
  UmiGtk4DocumentEditResultFn completed = owner != NULL ? owner->edit_completed : NULL;
  void *context = owner != NULL ? owner->edit_context : NULL;
  if (UmiGtk4WindowIsOpen(window))
    gtk_label_set_text(
        GTK_LABEL(form->message),
        form->applied ? status == UMI_STATUS_OK
                            ? "Applied to the draft. Undo restores it; Save is separate."
                            : "The draft was applied. Native refresh or a repeated request did not "
                              "complete; inspect the editor."
        : status == UMI_STATUS_INVALID_STATE ? "The source changed. Close and request a new review."
        : status == UMI_STATUS_CANCELLED     ? "The editing owner retired. No change was applied."
        : status == UMI_STATUS_NOT_FOUND     ? "The document closed. No change was applied."
        : status == UMI_STATUS_BUSY ? "Finish the active document operation before applying."
                                    : umi_status_text(status));
  owner = WhitespaceOwner(form);
  if (owner != NULL && owner->edit_completed == completed && owner->edit_context == context &&
      completed != NULL)
    completed(context, status);
  form->busy = 0;
  g_object_unref(window);
}
UmiStatus UmiGtk4AdapterReviewWhitespace(UmiGtk4Adapter *adapter, UmiDocumentId id,
                                         const UmiEditorWhitespaceOptions *options) {
  if (adapter == NULL || options == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (adapter->window == NULL || adapter->edit_lifetime == NULL ||
      adapter->edit_coordinator == NULL || !UmiGtk4WindowIsOpen(adapter->window))
    return UMI_STATUS_UNAVAILABLE;
  GObject *token = g_object_ref(adapter->edit_lifetime);
  GWeakRef *previous = g_object_get_data(token, "whitespace-review-window");
  GtkWindow *existing = previous != NULL ? g_weak_ref_get(previous) : NULL;
  int open = existing != NULL && UmiGtk4WindowIsOpen(existing);
  g_clear_object(&existing);
  if (open || g_object_get_data(token, "whitespace-review-opening") != NULL ||
      WhitespaceBusy(adapter)) {
    g_object_unref(token);
    return UMI_STATUS_BUSY;
  }
  if (id == 0U) {
    UmiDocumentWorkingCopySnapshot active;
    UmiStatus status = umi_document_coordinator_active_snapshot(adapter->edit_coordinator, &active);
    if (status != UMI_STATUS_OK) {
      g_object_unref(token);
      return status;
    }
    id = active.document_id;
  }
  UmiDocumentWhitespaceReview *review = NULL;
  UmiStatus status =
      UmiDocumentWhitespaceReviewCreate(adapter->edit_coordinator, id, options, &review);
  if (status != UMI_STATUS_OK) {
    g_object_unref(token);
    return status;
  }
  UmiDocumentWhitespaceSummary summary;
  const char *before, *after;
  size_t before_bytes, after_bytes;
  status = UmiDocumentWhitespaceReviewInspect(review, &summary);
  if (status == UMI_STATUS_OK)
    status = UmiDocumentWhitespaceReviewTexts(review, &before, &before_bytes, &after, &after_bytes);
  if (status != UMI_STATUS_OK) {
    UmiDocumentWhitespaceReviewDestroy(review);
    g_object_unref(token);
    return status;
  }
  /* Constructor observers can retire the binding before a new window returns. */
  UmiDocumentCoordinator *captured_owner = adapter->edit_coordinator;
  GtkWindow *parent = g_object_ref(adapter->window);
  g_object_set_data(token, "whitespace-review-opening", GINT_TO_POINTER(1));
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  g_object_ref(window);
  WhitespaceForm *form = g_new0(WhitespaceForm, 1);
  form->lifetime = g_object_ref(token);
  form->owner = captured_owner;
  form->review = review;
  form->status = UMI_STATUS_UNAVAILABLE;
  form->body = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
  g_object_set_data_full(G_OBJECT(window), "umicom-whitespace-review", form, WhitespaceFormFree);
  gtk_widget_set_name(GTK_WIDGET(window), "document.whitespace.review");
  gchar *valid = g_utf8_make_valid(summary.display_name, -1);
  gchar *title = g_strdup_printf("Review whitespace: %s", valid);
  gtk_window_set_title(window, title);
  g_free(title);
  g_free(valid);
  gchar *details = g_strdup_printf(
      "%zu changed lines; %zu bytes removed, %zu inserted. %s at width %u. "
      "Whitespace can affect Makefiles, Markdown and string literals. Review the complete text "
      "before Apply.",
      summary.text.changed_lines, summary.text.removed_bytes, summary.text.inserted_bytes,
      summary.options.selected_lines ? "Captured selected lines" : "Entire document",
      summary.options.tab_width);
  GtkWidget *help = gtk_label_new(details);
  g_free(details);
  gtk_label_set_wrap(GTK_LABEL(help), TRUE);
  gtk_box_append(GTK_BOX(form->body), help);
  GtkWidget *comparison = NULL;
  status = UmiGtk4TextComparisonCreate(before, before_bytes, after, after_bytes, "Captured source",
                                       "Proposed source", &comparison);
  if (status == UMI_STATUS_OK) {
    gtk_widget_set_name(comparison, "document.whitespace.comparison");
    gtk_widget_set_vexpand(comparison, TRUE);
    gtk_box_append(GTK_BOX(form->body), comparison);
  }
  form->message = gtk_label_new("Review ready. Source and saved files are unchanged.");
  gtk_widget_set_name(form->message, "document.whitespace.message");
  gtk_label_set_wrap(GTK_LABEL(form->message), TRUE);
  gtk_box_append(GTK_BOX(form->body), form->message);
  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  GtkWidget *apply = gtk_button_new_with_label("Apply reviewed draft"),
            *close = gtk_button_new_with_label("Close");
  gtk_widget_set_name(apply, "document.whitespace.apply");
  gtk_widget_set_name(close, "document.whitespace.close");
  g_signal_connect_object(apply, "clicked", G_CALLBACK(WhitespaceApply), window, 0);
  g_signal_connect_object(close, "clicked", G_CALLBACK(WhitespaceClose), window, 0);
  gtk_box_append(GTK_BOX(actions), apply);
  gtk_box_append(GTK_BOX(actions), close);
  gtk_box_append(GTK_BOX(form->body), actions);
  gtk_window_set_child(window, form->body);
  gtk_window_set_default_size(window, 1000, 650);
  gtk_window_set_transient_for(window, parent);
  gtk_window_set_destroy_with_parent(window, TRUE);
  UmiGtk4Adapter *owner = WhitespaceOwner(form);
  if (owner == NULL || !UmiGtk4WindowIsOpen(parent) || !UmiGtk4WindowIsOpen(window))
    status = UMI_STATUS_CANCELLED;
  else if (status == UMI_STATUS_OK)
    status = UmiDocumentWhitespaceReviewCheck(owner->edit_coordinator, review);
  if (status == UMI_STATUS_OK) {
    GWeakRef *weak = g_new0(GWeakRef, 1);
    g_weak_ref_init(weak, window);
    g_object_set_data_full(token, "whitespace-review-window", weak, WhitespaceWeakFree);
    gtk_window_present(window);
    if (WhitespaceOwner(form) == NULL || !UmiGtk4WindowIsOpen(window))
      status = UMI_STATUS_CANCELLED;
  }
  g_object_set_data(token, "whitespace-review-opening", NULL);
  if (status != UMI_STATUS_OK && UmiGtk4WindowIsOpen(window))
    gtk_window_destroy(window);
  g_object_unref(parent);
  g_object_unref(token);
  g_object_unref(window);
  return status;
}

UmiStatus UmiGtk4AdapterReviewWhitespaceCommand(UmiGtk4Adapter *adapter, const char *command_id) {
  if (adapter == NULL || command_id == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (adapter->edit_coordinator == NULL)
    return UMI_STATUS_UNAVAILABLE;
  UmiDocumentWorkingCopySnapshot active;
  UmiEditorWhitespaceOptions options;
  UmiStatus status = umi_document_coordinator_active_snapshot(adapter->edit_coordinator, &active);
  if (status == UMI_STATUS_OK)
    status = UmiDocumentWhitespaceCommandOptions(adapter->edit_coordinator, active.document_id,
                                                 command_id, &options);
  if (status == UMI_STATUS_OK)
    status = UmiGtk4AdapterReviewWhitespace(adapter, active.document_id, &options);
  return status;
}
