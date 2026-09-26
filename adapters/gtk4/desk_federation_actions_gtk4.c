/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desk_federation_actions_gtk4.c
 *
 * PURPOSE:
 *   Translate explicit user actions into the shared workspace and launch-review contracts.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "desk_federation_internal.h"
#include <inttypes.h>

void DfGtkStatus(DfGtk *ui, UmiStatus status, const char *success)
{
    const char *message;
    switch (status) {
    case UMI_STATUS_OK: message = success; break;
    case UMI_STATUS_BUSY: message = "The saved set or an application changed. Reload if needed, then preview again. No automatic retry was made."; break;
    case UMI_STATUS_NOT_FOUND: message = "That saved entry or application was not found. Reload and check its ID."; break;
    case UMI_STATUS_INVALID_ARGUMENT: message = "Check the fields. IDs use letters, numbers, dots, underscores and hyphens; names must be valid text."; break;
    case UMI_STATUS_INVALID_STATE: message = "Open storage and select applications or a saved entry first. A used review must be previewed again."; break;
    case UMI_STATUS_CAPACITY_EXCEEDED: message = "A text, application-set or storage limit was reached. Nothing was silently truncated."; break;
    case UMI_STATUS_ALREADY_EXISTS: message = "The input contains a repeated application ID."; break;
    case UMI_STATUS_UNAVAILABLE: message = "An application or SQLite capability is unavailable. Review the per-application results."; break;
    case UMI_STATUS_PARSE_ERROR: message = "Saved workspace data is incomplete or invalid. Do not overwrite it; close this view and investigate storage."; break;
    default: message = "The operation failed. Review the result and storage diagnostics before retrying."; break;
    }
    gtk_label_set_text(ui->status, message);
}
void DfGtkCancel(DfGtk *ui)
{
    ui->previewToken = 0U;
    if (ui->service != NULL) (void)UmiDeskFederationCancelPreview(ui->service);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->confirm), FALSE);
}
void DfGtkInputChanged(GtkEditable *editable, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); (void)editable;
    if (ui != NULL && !ui->updating) DfGtkCancel(ui);
}
void DfGtkSelected(GObject *object, GParamSpec *property, gpointer root)
{
    DfGtk *ui = DfGtkGet(root);
    guint index;
    (void)object; (void)property;
    if (ui == NULL || ui->updating) return;
    DfGtkCancel(ui);
    index = gtk_drop_down_get_selected(ui->saved);
    if ((size_t)index < ui->workspaceCount) {
        ui->updating = true;
        gtk_editable_set_text(GTK_EDITABLE(ui->id), ui->workspaces[index].id);
        gtk_editable_set_text(GTK_EDITABLE(ui->title), ui->workspaces[index].title);
        ui->updating = false;
    }
}
void DfGtkRefreshWorkspaces(DfGtk *ui)
{
    UmiDeskFederationSnapshot snapshot;
    GtkStringList *names;
    if (ui->service == NULL || UmiDeskFederationSnapshotGet(ui->service, &snapshot) != UMI_STATUS_OK) return;
    names = gtk_string_list_new(NULL);
    ui->updating = true; ui->workspaceCount = 0U;
    for (size_t index = 0U; index < snapshot.workspaceCount; ++index) {
        UmiDeskWorkspace *workspace = &ui->workspaces[ui->workspaceCount];
        char *name;
        if (UmiDeskFederationWorkspaceAt(ui->service, index, workspace) != UMI_STATUS_OK) break;
        name = g_strdup_printf("%s: %s [%s] - %zu application(s)",
            workspace->kind == UMI_DESK_WORKSPACE_GROUP ? "Group" : "Session", workspace->title,
            workspace->id, workspace->memberCount);
        gtk_string_list_append(names, name); g_free(name); ++ui->workspaceCount;
    }
    if (ui->workspaceCount == 0U) gtk_string_list_append(names, "No saved workspaces");
    gtk_drop_down_set_model(ui->saved, G_LIST_MODEL(names));
    gtk_drop_down_set_selected(ui->saved, GTK_INVALID_LIST_POSITION);
    g_object_unref(names); ui->updating = false;
}
void DfGtkRefreshActivity(DfGtk *ui)
{
    UmiDeskFederationSnapshot snapshot;
    UmiDeskFederationActivity latest;
    uint64_t sequence = 0U;
    GString *text;
    char *badge;
    if (UmiDeskFederationSnapshotGet(ui->service, &snapshot) != UMI_STATUS_OK) return;
    if (UmiDeskFederationActivityAt(ui->service, 0U, &latest) == UMI_STATUS_OK) sequence = latest.sequence;
    if (sequence == ui->activitySequence && snapshot.unreadCount == ui->unreadCount &&
        snapshot.droppedActivity == ui->droppedActivity && sequence != 0U) return;
    ui->activitySequence = sequence; ui->unreadCount = snapshot.unreadCount; ui->droppedActivity = snapshot.droppedActivity;
    badge = g_strdup_printf("Desk activity: %zu unread", snapshot.unreadCount);
    gtk_label_set_text(ui->activityBadge, badge); g_free(badge);
    text = g_string_new("Local observations and request receipts. Accepted does not mean ready.\n");
    for (size_t index = 0U; index < snapshot.activityCount; ++index) {
        UmiDeskFederationActivity activity;
        const char *kind;
        if (UmiDeskFederationActivityAt(ui->service, index, &activity) != UMI_STATUS_OK) break;
        switch (activity.kind) {
        case UMI_DESK_ACTIVITY_REQUEST_ACCEPTED: kind = "Request accepted"; break;
        case UMI_DESK_ACTIVITY_REQUEST_FAILED: kind = "Request failed"; break;
        case UMI_DESK_ACTIVITY_SKIPPED: kind = "Skipped"; break;
        default: kind = "Observed state"; break;
        }
        g_string_append_printf(text, "%s#%" PRIu64 "  %s  %s  [%s; status %d]\n", activity.read ? "" : "Unread ",
            activity.sequence, activity.application.value, kind, UmiDeskFederationStateText(activity.state), (int)activity.status);
    }
    if (snapshot.droppedActivity != 0U) g_string_append_printf(text, "Older activity omitted: %" PRIu64 "\n", snapshot.droppedActivity);
    gtk_text_buffer_set_text(ui->activity, text->str, -1); g_string_free(text, TRUE);
}
static void ShowPlan(DfGtk *ui, const UmiDeskFederationPlan *plan)
{
    GString *text = g_string_new("Review these requests. No application has been started or switched yet.\n\n");
    size_t actionable = 0U;
    for (size_t index = 0U; index < plan->count; ++index) {
        const UmiDeskFederationPlanItem *item = &plan->items[index];
        g_string_append_printf(text, "%zu. %s [%s]\n   %s - %s\n", index + 1U, item->app.title,
            item->app.id.value, UmiDeskFederationActionText(item->action), UmiDeskFederationReasonText(item->reason));
        if (item->action != UMI_DESK_FEDERATION_SKIP) ++actionable;
    }
    ui->previewToken = plan->token;
    gtk_text_buffer_set_text(ui->preview, text->str, -1); g_string_free(text, TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->confirm), actionable != 0U);
}
void DfGtkOpenClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); (void)button;
    if (ui != NULL) DfGtkStatus(ui, UmiDeskFederationGtkOpenStorage(root, NULL), "Saved workspaces are open. No application was started.");
}
void DfGtkSaveClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); UmiStatus status; (void)button;
    if (ui == NULL || ui->service == NULL) return;
    DfGtkCancel(ui);
    status = UmiDeskFederationSaveSelection(ui->service, ui->runtime,
        gtk_editable_get_text(GTK_EDITABLE(ui->id)), gtk_editable_get_text(GTK_EDITABLE(ui->title)));
    if (status == UMI_STATUS_OK) DfGtkRefreshWorkspaces(ui);
    DfGtkStatus(ui, status, "Group saved from Desk's current selection. No application was started or stopped.");
}
void DfGtkCaptureClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); UmiStatus status; (void)button;
    if (ui == NULL || ui->service == NULL) return;
    DfGtkCancel(ui);
    status = UmiDeskFederationCaptureSession(ui->service,
        gtk_editable_get_text(GTK_EDITABLE(ui->id)), gtk_editable_get_text(GTK_EDITABLE(ui->title)));
    if (status == UMI_STATUS_OK) DfGtkRefreshWorkspaces(ui);
    DfGtkStatus(ui, status, "Running application IDs saved as a session. Save documents separately in their applications.");
}
void DfGtkPreviewClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); UmiStatus status; UmiDeskFederationPlan plan; guint index; (void)button;
    if (ui == NULL || ui->service == NULL) return;
    DfGtkCancel(ui); index = gtk_drop_down_get_selected(ui->saved);
    if ((size_t)index >= ui->workspaceCount) { DfGtkStatus(ui, UMI_STATUS_INVALID_STATE, ""); return; }
    status = UmiDeskFederationPrepareWorkspace(ui->service, ui->workspaces[index].kind, ui->workspaces[index].id, &plan);
    if (status == UMI_STATUS_OK) ShowPlan(ui, &plan);
    DfGtkStatus(ui, status, "Review the list, then confirm. Skipped applications are explained individually.");
}
void DfGtkLinkClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); UmiStatus status; UmiDeskFederationPlan plan; (void)button;
    if (ui == NULL || ui->service == NULL) return;
    DfGtkCancel(ui);
    status = UmiDeskFederationPrepareLink(ui->service, gtk_editable_get_text(GTK_EDITABLE(ui->link)), &plan);
    if (status == UMI_STATUS_OK) ShowPlan(ui, &plan);
    DfGtkStatus(ui, status, "Link parsed and reviewed. Nothing opens until you confirm.");
}
void DfGtkConfirmClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); UmiStatus status; UmiDeskFederationReport report; uint64_t token; GString *text; (void)button;
    if (ui == NULL || ui->service == NULL || ui->previewToken == 0U) return;
    token = ui->previewToken; ui->previewToken = 0U;
    gtk_widget_set_sensitive(GTK_WIDGET(ui->confirm), FALSE);
    status = UmiDeskFederationExecute(ui->service, token, &report);
    text = g_string_new(NULL);
    g_string_append_printf(text, "Requests accepted: %zu; failed: %zu; skipped: %zu.\nAccepted is not a readiness guarantee.\n\n",
        report.acceptedCount, report.failedCount, report.skippedCount);
    for (size_t index = 0U; index < report.count; ++index) {
        const UmiDeskFederationResult *item = &report.results[index];
        g_string_append_printf(text, "%s - %s: %s (status %d)\n", item->id.value,
            UmiDeskFederationActionText(item->action), item->status == UMI_STATUS_OK ? "accepted" : "not accepted", (int)item->status);
    }
    if (report.count == 0U) g_string_append(text, "No requests dispatched. The review expired or could not be revalidated.\n");
    gtk_text_buffer_set_text(ui->preview, text->str, -1); g_string_free(text, TRUE);
    DfGtkRefreshActivity(ui);
    DfGtkStatus(ui, status, "Requests were accepted by Desk's existing launcher. Check the taskbar and application windows for their actual state.");
}
void DfGtkCancelClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); (void)button;
    if (ui == NULL) return;
    DfGtkCancel(ui); gtk_text_buffer_set_text(ui->preview, "Preview cancelled. No application was changed.", -1);
}
void DfGtkRemoveClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); UmiStatus status; guint index; (void)button;
    if (ui == NULL || ui->service == NULL) return;
    DfGtkCancel(ui); index = gtk_drop_down_get_selected(ui->saved);
    if ((size_t)index >= ui->workspaceCount) { DfGtkStatus(ui, UMI_STATUS_INVALID_STATE, ""); return; }
    status = UmiDeskFederationRemoveWorkspace(ui->service, ui->workspaces[index].kind, ui->workspaces[index].id);
    if (status == UMI_STATUS_OK) DfGtkRefreshWorkspaces(ui);
    DfGtkStatus(ui, status, "Saved entry removed. Running applications, documents and business data were not changed.");
}
void DfGtkReadClicked(GtkButton *button, gpointer root)
{
    DfGtk *ui = DfGtkGet(root); (void)button;
    if (ui == NULL || ui->service == NULL) return;
    (void)UmiDeskFederationAcknowledgeAll(ui->service); DfGtkRefreshActivity(ui);
}
