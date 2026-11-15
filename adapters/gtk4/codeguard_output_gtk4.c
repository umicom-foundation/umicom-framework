/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/codeguard_output_gtk4.c
 *
 * PURPOSE:
 *   Present current-context CodeGuard findings without duplicating output controls or scan ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/output_gtk4.h"
#include "umicom/codeguard/report.h"
#include "umicom/ui/gtk4/output_view.h"
#include <stdint.h>
#include <string.h>

typedef struct CodeGuardOutput {
    UmiCodeGuardReviewSnapshot last;
    uint64_t context, revision;
    gboolean seen, updating;
} CodeGuardOutput;

static gboolean same_summary(const UmiCodeGuardSummary *a, const UmiCodeGuardSummary *b)
{
    return a->total == b->total && a->critical == b->critical && a->high == b->high &&
        a->medium == b->medium && a->low == b->low && a->info == b->info &&
        a->memory == b->memory && a->buffers == b->buffers &&
        a->architecture == b->architecture && a->duplicates == b->duplicates;
}

static gboolean same_evidence(const UmiCodeGuardReviewSnapshot *a,
    const UmiCodeGuardReviewSnapshot *b)
{
    return a->request_id == b->request_id && a->context_id == b->context_id &&
        a->report_context_id == b->report_context_id && a->state == b->state &&
        a->status == b->status && a->operation_status == b->operation_status &&
        a->pending == b->pending && a->cancel_requested == b->cancel_requested &&
        a->stale == b->stale && a->has_report == b->has_report &&
        same_summary(&a->summary, &b->summary);
}

static gboolean flag_valid(int value) { return value == 0 || value == 1; }

GtkWidget *UmiCodeGuardOutputGtk4Create(void)
{
    GtkWidget *panel = UmiOutputViewGtk4Create("codeguard.output",
        "Ready. Open a workspace and choose Tools > CodeGuard to scan saved files.");
    if (panel == NULL) return NULL;
    g_object_set_data_full(G_OBJECT(panel), "umicom-codeguard-output",
        g_new0(CodeGuardOutput, 1), g_free);
    return panel;
}

UmiStatus UmiCodeGuardOutputGtk4Update(GtkWidget *panel, uint64_t current_context,
    const UmiCodeGuardReviewSnapshot *snapshot, const UmiCodeGuardResult *result)
{
    if (panel == NULL || !GTK_IS_BOX(panel) || snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    CodeGuardOutput *owner = g_object_get_data(G_OBJECT(panel), "umicom-codeguard-output");
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->updating) return UMI_STATUS_BUSY;
    if ((unsigned)snapshot->state > (unsigned)UMI_TASK_CANCELLED ||
        !flag_valid(snapshot->pending) || !flag_valid(snapshot->cancel_requested) ||
        !flag_valid(snapshot->stale) || !flag_valid(snapshot->has_report) ||
        (snapshot->pending && (snapshot->request_id == 0U || snapshot->context_id == 0U)) ||
        (snapshot->has_report && (result == NULL || current_context == 0U ||
            snapshot->report_context_id != current_context)) ||
        (!snapshot->has_report && result != NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->seen && owner->context == current_context &&
        same_evidence(&owner->last, snapshot)) return UMI_STATUS_OK;
    if (owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Complete every borrow-dependent calculation before GTK can call the
     * host. The adapter owns only copied plain values and projection bytes. */
    UmiCodeGuardReviewSnapshot captured = *snapshot;
    UmiOutputViewSnapshot *view = g_new0(UmiOutputViewSnapshot, 1);
    view->operation_id = 1U;
    view->revision = owner->revision + 1U;
    strcpy(view->context, "CodeGuard | saved source files");
    UmiStatus projection_status = UMI_STATUS_OK;
    char *projection = NULL;
    size_t bytes = 0U;
    if (captured.has_report) {
        projection_status = umi_codeguard_report_render(UMI_CODEGUARD_REPORT_TEXT,
            result, sizeof view->bytes - 1U, &projection, &bytes);
        if (projection_status == UMI_STATUS_OK) {
            memcpy(view->bytes, projection, bytes + 1U);
            view->length = bytes;
            view->total_bytes = bytes;
        }
        umi_codeguard_report_free(projection);
    }
    const char *retained = captured.has_report ? " Retained report available." : " No current report.";
    if (current_context == 0U)
        (void)g_snprintf(view->status, sizeof view->status, "Open a workspace to scan saved files.");
    else if (projection_status != UMI_STATUS_OK)
        (void)g_snprintf(view->status, sizeof view->status,
            "Report preview unavailable: %s. Full scan result remains with its service.",
            umi_status_text(projection_status));
    else if (captured.pending)
        (void)g_snprintf(view->status, sizeof view->status, "%s%s%s%s",
            captured.state == UMI_TASK_QUEUED ? "Scan queued." :
            captured.state == UMI_TASK_RUNNING ? "Scan running." : "Finishing scan publication.",
            captured.cancel_requested ? " Cancellation requested." : "",
            captured.stale ? " Workspace context changed; publication refused." : "", retained);
    else if (captured.request_id == 0U)
        (void)g_snprintf(view->status, sizeof view->status, "Ready to scan saved files.");
    else
        (void)g_snprintf(view->status, sizeof view->status,
            "Execution: %s. Publication: %s.%s Findings require review.",
            umi_status_text(captured.operation_status), umi_status_text(captured.status), retained);

    g_object_ref(panel);
    owner->updating = TRUE;
    gboolean reset = !owner->seen || owner->context != current_context ||
        (owner->last.has_report && !captured.has_report) ||
        projection_status != UMI_STATUS_OK;
    UmiStatus status = reset ? UmiOutputViewGtk4Reset(panel) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK) status = UmiOutputViewGtk4Update(panel, view);
    if (status == UMI_STATUS_OK) {
        owner->last = captured;
        owner->context = current_context;
        owner->revision = view->revision;
        owner->seen = TRUE;
    }
    owner->updating = FALSE;
    g_free(view);
    g_object_unref(panel);
    return status;
}
