/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_completion_bridge.inc
 *
 * PURPOSE:
 *   Bind selected-card completion review to the live board through a weak native owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


static UmiStatus CaptureCompletion(gpointer context, const UmiKanbanCompletionReview *review,
    UmiKanbanCompletionPlan **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!review) return UMI_STATUS_INVALID_ARGUMENT;
    GtkWidget *root = NULL; Panel *panel = Acquire(context, &root);
    if (!panel) return UMI_STATUS_UNAVAILABLE;
    UmiStatus status = UMI_STATUS_UNAVAILABLE;
    if (gtk_widget_get_mapped(root)) {
        UmiKanbanCompletionRecord record = {0};
        g_strlcpy(record.card_id, panel->selected, sizeof record.card_id);
        record.decision = review->decision;
        memcpy(record.evidence_sha256, review->evidence_sha256, sizeof record.evidence_sha256);
        memcpy(record.rationale, review->rationale, sizeof record.rationale);
        const char *done = NULL;
        for (size_t i = 0U; i < panel->config.column_count; ++i)
            if (panel->config.columns[i].stage == UMI_KANBAN_DONE) {
                done = panel->config.columns[i].id; break;
            }
        status = done ? UmiKanbanCompletionPlanCreate(panel->board, panel->selection_revision,
            done, review->reviewer, review->source_identity, review->configuration, &record, 1U, out)
            : UMI_STATUS_NOT_FOUND;
    }
    g_object_unref(root); return status;
}
static UmiStatus ApplyCompletion(gpointer context, const UmiKanbanCompletionPlan *plan)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(context, &root);
    if (!panel) return UMI_STATUS_UNAVAILABLE;
    UmiKanbanCompletionPlanInfo info;
    UmiStatus status = UmiKanbanCompletionPlanRead(plan, &info);
    if (!gtk_widget_get_mapped(root)) status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK && panel->revision != info.reviewed_revision)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        /* The plan service owns the checked atomic edit. Native callbacks cannot
         * enter another panel edit during the subsequent observable refresh.
         * Refresh interruption cannot undo or misreport an accepted model edit. */
        panel->applying = true;
        status = UmiKanbanCompletionPlanApply(plan, panel->board, info.reviewed_revision);
        if (status == UMI_STATUS_OK) {
            if (panel->selection_revision == info.reviewed_revision)
                panel->selection_revision = umi_kanban_board_revision(panel->board);
            (void)UmiKanbanGtk4Refresh(root);
        }
        panel->applying = false;
    }
    g_object_unref(root); return status;
}
static GtkWidget *CreateCompletionReview(Panel *panel)
{
    Binding *binding = g_new0(Binding, 1);
    g_weak_ref_init(&binding->root, G_OBJECT(panel->root));
    GtkWidget *widget = UmiKanbanCompletionGtk4Create(CaptureCompletion, ApplyCompletion,
        binding, FreePlanContext);
    if (!widget) FreePlanContext(binding);
    return widget;
}
