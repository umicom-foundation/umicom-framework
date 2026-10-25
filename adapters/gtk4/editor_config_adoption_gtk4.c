/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_config_adoption_gtk4.c
 *
 * PURPOSE:
 *   Re-read configuration on a worker and apply only a reviewed named document policy through a
 * live editing binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"
#include "umicom/document/editor_config_adoption.h"
#include "umicom/platform/path.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/ui/gtk4/editor_config_adoption.h"
#include "umicom/ui/gtk4/editor_indentation.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <string.h>
typedef struct AdoptionJob {
  UmiEditorConfigInspectionScope scope;
  UmiCancellationToken *cancel;
  UmiEditorConfigInspection *inspection;
  UmiEditorConfigInspectionDiagnostic diagnostic;
  UmiStatus status;
  int apply;
} AdoptionJob;
typedef struct AdoptionWindow {
  GObject *lifetime;
  UmiDocumentId document;
  UmiEditorConfigInspectionScope scope;
  UmiDocumentEditorConfigPlan *plan;
  AdoptionJob *job;
  GtkWidget *body, *message;
  GtkTextBuffer *buffer;
  UmiStatus status;
  int busy, closed, applied;
} AdoptionWindow;
static void AdoptionWeakFree(gpointer data) {
  GWeakRef *weak = data;
  g_weak_ref_clear(weak);
  g_free(weak);
}
static void AdoptionJobFree(gpointer data) {
  AdoptionJob *job = data;
  umi_cancellation_token_destroy(job->cancel);
  UmiEditorConfigInspectionDestroy(job->inspection);
  g_free(job);
}
static void AdoptionWindowFree(gpointer data) {
  AdoptionWindow *state = data;
  UmiDocumentEditorConfigPlanDestroy(state->plan);
  g_object_unref(state->lifetime);
  g_object_unref(state->body);
  g_object_unref(state->buffer);
  g_free(state);
}
static void AdoptionRemoved(GtkWidget *window, gpointer data) {
  (void)window;
  AdoptionWindow *state = data;
  state->closed = 1;
  if (state->job)
    umi_cancellation_token_request(state->job->cancel);
}
static UmiGtk4Adapter *AdoptionOwner(AdoptionWindow *state) {
  return g_object_get_data(state->lifetime, "adapter");
}
static int AdoptionBusy(UmiGtk4Adapter *owner) {
  return owner->applying_document_state || owner->edit_cancel != NULL ||
         UmiGtk4AdapterDocumentSaveAllBusy(owner) || UmiGtk4AdapterDocumentCloseBusy(owner) ||
         UmiGtk4AdapterReplacementReviewBusy(owner) || UmiGtk4AdapterDocumentProposalBusy(owner) ||
         UmiGtk4AdapterCompletionReviewBusy(owner);
}
static void AdoptionWork(GTask *task, gpointer object, gpointer data, GCancellable *cancel) {
  (void)object;
  (void)cancel;
  AdoptionJob *job = data;
  job->status = UmiEditorConfigInspectionLoad(job->scope.project_root, job->scope.relative_file,
                                              job->cancel, &job->inspection, &job->diagnostic);
  g_task_return_boolean(task, TRUE);
}
static char *AdoptionReport(AdoptionWindow *state, AdoptionJob *job) {
  GString *text = g_string_new(NULL);
  g_string_append_printf(text, "Project boundary: %s\nCaptured file: %s\n\n",
                         state->scope.project_root, state->scope.relative_file);
  if (state->plan) {
    UmiDocumentEditorConfigSummary summary;
    if (UmiDocumentEditorConfigPlanInspect(state->plan, &summary) == UMI_STATUS_OK) {
      g_string_append_printf(
          text,
          "Document: %s\nCurrent at review: %u columns, %s\nProposed: %u columns, %s\n"
          "Other properties not applied: %zu (unknown: %zu; invalid values observed: %zu)\n\n",
          summary.display_name, summary.previous.tab_width,
          summary.previous.insert_spaces ? "spaces" : "tabs", summary.proposed.tab_width,
          summary.proposed.insert_spaces ? "spaces" : "tabs", summary.ignored_properties,
          summary.unknown_properties, summary.invalid_values);
      for (size_t i = 0U; i < summary.inspected_files; ++i) {
        UmiEditorConfigInspectionFile file;
        if (UmiDocumentEditorConfigPlanSource(state->plan, i, &file) == UMI_STATUS_OK)
          g_string_append_printf(text, "%s\n  %zu bytes; SHA-256 %s\n", file.relative_path,
                                 file.bytes, file.sha256);
      }
    }
  }
  g_string_append_printf(text, "\nResult: %s\n", umi_status_text(state->status));
  if (job->diagnostic.status != UMI_STATUS_OK)
    g_string_append_printf(text, "Configuration: %s; line %zu\n", job->diagnostic.relative_path,
                           job->diagnostic.line);
  g_string_append(text, "\nOnly indentation is adopted. Encoding, line endings, trimming and final "
                        "newlines are not applied. "
                        "Configuration is re-read before Apply; the observations do not lock files "
                        "against later external changes.\n");
  char *valid = g_utf8_make_valid(text->str, (gssize)text->len);
  g_string_free(text, TRUE);
  return valid;
}
static void AdoptionComplete(GObject *object, GAsyncResult *result, gpointer user) {
  (void)user;
  AdoptionWindow *state = g_object_get_data(object, "umicom-editor-config-adoption");
  AdoptionJob *job = g_task_get_task_data(G_TASK(result));
  (void)g_task_propagate_boolean(G_TASK(result), NULL);
  UmiGtk4Adapter *owner = AdoptionOwner(state);
  state->status = job->status;
  if (state->closed || !owner || umi_cancellation_token_is_requested(job->cancel))
    state->status = UMI_STATUS_CANCELLED;
  else if (state->status == UMI_STATUS_OK) {
    if (AdoptionBusy(owner))
      state->status = UMI_STATUS_BUSY;
    else if (!job->apply) {
      state->status = UmiDocumentEditorConfigPlanCreate(owner->edit_coordinator, state->document,
                                                        job->inspection, &state->plan);
      if (state->status == UMI_STATUS_OK)
        job->inspection = NULL;
    } else {
      state->status = UmiDocumentEditorConfigPlanApplyObserved(state->plan, owner->edit_coordinator,
                                                               job->inspection);
      if (state->status == UMI_STATUS_OK) {
        state->applied = 1;
        state->status = UmiGtk4AdapterRefreshIndentation(owner);
      }
    }
  }
  if (!state->closed) {
    char *report = AdoptionReport(state, job);
    gtk_text_buffer_set_text(state->buffer, report, -1);
    g_free(report);
    if (!state->closed) {
      const char *message =
          state->applied
              ? (state->status == UMI_STATUS_OK
                     ? "Document indentation applied. Source text and files were not changed."
                     : "Document indentation applied, but native refresh was interrupted.")
          : state->status == UMI_STATUS_OK
              ? "Review the proposed indentation. Apply will re-read configuration before adoption."
          : state->status == UMI_STATUS_NOT_IMPLEMENTED
              ? "The editor cannot represent these widths. Inspect and adjust the configuration "
                "before retrying."
          : state->status == UMI_STATUS_NOT_FOUND
              ? "No applicable indentation settings or live document were found."
          : state->status == UMI_STATUS_PARSE_ERROR
              ? "A configuration or indentation value is invalid. Nothing was applied."
          : state->status == UMI_STATUS_INVALID_STATE
              ? "The configuration, saved path or document settings changed. Inspect again; "
                "nothing was applied."
          : state->status == UMI_STATUS_CANCELLED
              ? "Inspection cancelled or the editor retired. Nothing was applied."
              : "Inspection or adoption did not complete. Nothing was applied; review the "
                "diagnostic.";
      gtk_label_set_text(GTK_LABEL(state->message), message);
    }
  }
  owner = AdoptionOwner(state);
  if (job->apply && owner && !state->closed && owner->edit_completed)
    owner->edit_completed(owner->edit_context, state->status);
  state->job = NULL;
  state->busy = 0;
}
static void AdoptionStart(GtkButton *button, gpointer data) {
  GtkWindow *window = data;
  AdoptionWindow *state = g_object_get_data(G_OBJECT(window), "umicom-editor-config-adoption");
  if (!state || state->closed || state->busy ||
      gtk_widget_get_root(GTK_WIDGET(button)) != GTK_ROOT(window))
    return;
  g_object_ref(window);
  state->busy = 1;
  UmiGtk4Adapter *owner = AdoptionOwner(state);
  int apply = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "apply"));
  UmiStatus status = owner ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
  if (owner && AdoptionBusy(owner))
    status = UMI_STATUS_BUSY;
  if (status == UMI_STATUS_OK && apply)
    status = state->plan ? UmiDocumentEditorConfigPlanCheck(state->plan, owner->edit_coordinator)
                         : UMI_STATUS_INVALID_STATE;
  if (status != UMI_STATUS_OK) {
    state->status = status;
    gtk_label_set_text(
        GTK_LABEL(state->message),
        status == UMI_STATUS_ALREADY_EXISTS
            ? "These settings were already applied. Inspect again for another review."
            : "Inspect the live document and its current settings before applying.");
    state->busy = 0;
    g_object_unref(window);
    return;
  }
  if (!apply) {
    UmiDocumentEditorConfigPlanDestroy(state->plan);
    state->plan = NULL;
    state->applied = 0;
  }
  AdoptionJob *job = g_new0(AdoptionJob, 1);
  job->scope = state->scope;
  job->apply = apply;
  status = umi_cancellation_token_create(&job->cancel);
  if (status != UMI_STATUS_OK) {
    state->status = status;
    state->busy = 0;
    AdoptionJobFree(job);
    g_object_unref(window);
    return;
  }
  state->job = job;
  state->status = UMI_STATUS_BUSY;
  gtk_label_set_text(GTK_LABEL(state->message), apply
                                                    ? "Re-reading configuration before adoption…"
                                                    : "Inspecting captured project configuration…");
  if (state->closed || !AdoptionOwner(state)) {
    state->job = NULL;
    state->busy = 0;
    state->status = UMI_STATUS_CANCELLED;
    AdoptionJobFree(job);
    g_object_unref(window);
    return;
  }
  GTask *task = g_task_new(window, NULL, AdoptionComplete, NULL);
  g_task_set_task_data(task, job, AdoptionJobFree);
  g_task_run_in_thread(task, AdoptionWork);
  g_object_unref(task);
  g_object_unref(window);
}
static void AdoptionCancel(GtkButton *button, gpointer data) {
  (void)button;
  AdoptionWindow *state = g_object_get_data(G_OBJECT(data), "umicom-editor-config-adoption");
  if (state && state->job)
    umi_cancellation_token_request(state->job->cancel);
}
static void AdoptionClose(GtkButton *button, gpointer data) {
  (void)button;
  gtk_window_destroy(GTK_WINDOW(data));
}
UmiStatus UmiGtk4AdapterReviewEditorConfig(UmiGtk4Adapter *adapter, const char *root,
                                           UmiDocumentId id) {
  if (!adapter || !root)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!adapter->edit_coordinator || !adapter->edit_lifetime || !adapter->window ||
      !UmiGtk4WindowIsOpen(adapter->window))
    return UMI_STATUS_UNAVAILABLE;
  GObject *lifetime = g_object_ref(adapter->edit_lifetime);
  GWeakRef *weak = g_object_get_data(lifetime, "editor-config-adoption-window");
  GtkWindow *existing = weak ? g_weak_ref_get(weak) : NULL;
  int open = existing && UmiGtk4WindowIsOpen(existing);
  g_clear_object(&existing);
  if (open || g_object_get_data(lifetime, "editor-config-adoption-opening") ||
      AdoptionBusy(adapter)) {
    g_object_unref(lifetime);
    return UMI_STATUS_BUSY;
  }
  UmiDocumentWorkingCopySnapshot document = {0};
  UmiStatus status = UMI_STATUS_NOT_FOUND;
  for (size_t i = 0U; i < umi_document_coordinator_count(adapter->edit_coordinator); ++i) {
    UmiDocumentWorkingCopySnapshot candidate;
    status = umi_document_coordinator_at(adapter->edit_coordinator, i, &candidate);
    if (status != UMI_STATUS_OK)
      break;
    status = UMI_STATUS_NOT_FOUND;
    if ((id && candidate.document_id == id) || (!id && candidate.active)) {
      document = candidate;
      status = UMI_STATUS_OK;
      break;
    }
  }
  UmiEditorConfigInspectionScope scope;
  if (status == UMI_STATUS_OK && !document.has_path)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_path_normalise(root, scope.project_root, sizeof scope.project_root);
  if (status == UMI_STATUS_OK)
    status = umi_path_relative(scope.project_root, document.path, scope.relative_file,
                               sizeof scope.relative_file);
  if (status == UMI_STATUS_OK)
    status = UmiRootedFileValidatePath(scope.project_root, scope.relative_file);
  if (status == UMI_STATUS_OK && (!g_utf8_validate(scope.project_root, -1, NULL) ||
                                  !g_utf8_validate(scope.relative_file, -1, NULL)))
    status = UMI_STATUS_INVALID_ARGUMENT;
  UmiDocumentIndentationReview policy;
  if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorInspectIndentation(adapter->edit_coordinator,
                                                      document.document_id, &policy);
  if (status != UMI_STATUS_OK) {
    g_object_unref(lifetime);
    return status;
  }
  GtkWindow *parent = g_object_ref(adapter->window);
  g_object_set_data(lifetime, "editor-config-adoption-opening", GINT_TO_POINTER(1));
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  g_object_ref(window);
  AdoptionWindow *state = g_new0(AdoptionWindow, 1);
  state->lifetime = g_object_ref(lifetime);
  state->document = document.document_id;
  state->scope = scope;
  state->busy = 1;
  state->status = UMI_STATUS_UNAVAILABLE;
  state->body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  g_object_ref_sink(state->body);
  state->buffer = gtk_text_buffer_new(NULL);
  g_object_set_data_full(G_OBJECT(window), "umicom-editor-config-adoption", state,
                         AdoptionWindowFree);
  (void)UmiGtk4WatchWindowClosed(window, G_OBJECT(window), AdoptionRemoved, state);
  gtk_widget_set_name(GTK_WIDGET(window), "document.editor-config.review");
  gtk_window_set_title(window, "Review document EditorConfig indentation");
  gtk_window_set_default_size(window, 800, 600);
  gtk_widget_set_margin_start(state->body, 12);
  gtk_widget_set_margin_end(state->body, 12);
  gtk_widget_set_margin_top(state->body, 12);
  gtk_widget_set_margin_bottom(state->body, 12);
  gchar *label = g_strdup_printf("Captured file: %s\nProject boundary: %s", scope.relative_file,
                                 scope.project_root);
  GtkWidget *target = gtk_label_new(label);
  g_free(label);
  gtk_label_set_wrap(GTK_LABEL(target), TRUE);
  gtk_widget_set_name(target, "document.editor-config.target");
  gtk_box_append(GTK_BOX(state->body), target);
  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  GtkWidget *inspect = gtk_button_new_with_label("Inspect"),
            *apply = gtk_button_new_with_label("Apply indentation"),
            *cancel = gtk_button_new_with_label("Cancel operation"),
            *close = gtk_button_new_with_label("Close");
  gtk_widget_set_name(inspect, "document.editor-config.inspect");
  gtk_widget_set_name(apply, "document.editor-config.apply");
  gtk_widget_set_name(cancel, "document.editor-config.cancel");
  gtk_widget_set_name(close, "document.editor-config.close");
  g_object_set_data(G_OBJECT(apply), "apply", GINT_TO_POINTER(1));
  g_signal_connect_object(inspect, "clicked", G_CALLBACK(AdoptionStart), window, 0);
  g_signal_connect_object(apply, "clicked", G_CALLBACK(AdoptionStart), window, 0);
  g_signal_connect_object(cancel, "clicked", G_CALLBACK(AdoptionCancel), window, 0);
  g_signal_connect_object(close, "clicked", G_CALLBACK(AdoptionClose), window, 0);
  gtk_box_append(GTK_BOX(actions), inspect);
  gtk_box_append(GTK_BOX(actions), apply);
  gtk_box_append(GTK_BOX(actions), cancel);
  gtk_box_append(GTK_BOX(actions), close);
  gtk_box_append(GTK_BOX(state->body), actions);
  state->message = gtk_label_new("Inspect the captured file, then review the proposal before "
                                 "applying. No settings are adopted automatically.");
  gtk_label_set_wrap(GTK_LABEL(state->message), TRUE);
  gtk_widget_set_name(state->message, "document.editor-config.message");
  gtk_box_append(GTK_BOX(state->body), state->message);
  GtkWidget *report = gtk_text_view_new_with_buffer(state->buffer);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(report), FALSE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(report), TRUE);
  gtk_widget_set_name(report, "document.editor-config.report");
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_widget_set_vexpand(scroll, TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), report);
  gtk_box_append(GTK_BOX(state->body), scroll);
  if (!state->closed)
    gtk_window_set_child(window, state->body);
  if (!state->closed)
    gtk_window_set_transient_for(window, parent);
  if (!state->closed)
    gtk_window_set_destroy_with_parent(window, TRUE);
  UmiGtk4Adapter *owner = AdoptionOwner(state);
  if (state->closed || !owner || !UmiGtk4WindowIsOpen(parent))
    status = UMI_STATUS_CANCELLED;
  else
    status = UmiDocumentCoordinatorCheckIndentation(owner->edit_coordinator, &policy);
  if (status == UMI_STATUS_OK) {
    GWeakRef *reference = g_new0(GWeakRef, 1);
    g_weak_ref_init(reference, window);
    g_object_set_data_full(lifetime, "editor-config-adoption-window", reference, AdoptionWeakFree);
    state->busy = 0;
    gtk_window_present(window);
    if (state->closed || !AdoptionOwner(state))
      status = UMI_STATUS_CANCELLED;
  }
  g_object_set_data(lifetime, "editor-config-adoption-opening", NULL);
  if (status != UMI_STATUS_OK && !state->closed)
    gtk_window_destroy(window);
  g_object_unref(parent);
  g_object_unref(lifetime);
  g_object_unref(window);
  return status;
}
int UmiGtk4EditorConfigAdoptionBusy(GtkWindow *window) {
  if (!GTK_IS_WINDOW(window))
    return 0;
  AdoptionWindow *state = g_object_get_data(G_OBJECT(window), "umicom-editor-config-adoption");
  return state && state->busy;
}
UmiStatus UmiGtk4EditorConfigAdoptionStatus(GtkWindow *window) {
  if (!GTK_IS_WINDOW(window))
    return UMI_STATUS_INVALID_ARGUMENT;
  AdoptionWindow *state = g_object_get_data(G_OBJECT(window), "umicom-editor-config-adoption");
  if (!state)
    return UMI_STATUS_NOT_FOUND;
  if (state->closed)
    return UMI_STATUS_CANCELLED;
  return state->busy ? UMI_STATUS_BUSY : state->status;
}
