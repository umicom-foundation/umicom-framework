/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/cash_plan_files_gtk4.c
 * PURPOSE: Keep file workers independent of native controls and review before replacement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "cash_plan_private.h"
typedef struct CashFileJob
{
    GWeakRef root;
    UmiCashPlan *snapshot, *loaded;
    char *path;
    uint64_t generation;
    int load, csv;
    UmiStatus status;
    UmiOutputFileSnapshot receipt;
} CashFileJob;
static void FreeJob(gpointer value)
{
    CashFileJob *job = value;
    g_weak_ref_clear(&job->root);
    UmiCashPlanDestroy(job->snapshot);
    UmiCashPlanDestroy(job->loaded);
    g_free(job->path);
    g_free(job);
}
/* Workers touch only their owned snapshot/path. Closing a window cannot free
 * a source record in use; a completed load never applies itself to a live plan. */
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    CashFileJob *job = data;
    job->status = job->load  ? UmiCashPlanLoad(job->path, &job->loaded)
                  : job->csv ? UmiCashPlanExportNew(job->path, job->snapshot, &job->receipt)
                             : UmiCashPlanSaveNew(job->path, job->snapshot, &job->receipt);
    g_task_return_boolean(task, TRUE);
}
static void Complete(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)source;
    (void)unused;
    GTask *task = G_TASK(result);
    CashFileJob *job = g_task_get_task_data(task);
    (void)g_task_propagate_boolean(task, NULL);
    GtkWidget *root = g_weak_ref_get(&job->root);
    if (root == NULL)
        return;
    CashPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-cash-plan");
    if (panel == NULL)
    {
        g_object_unref(root);
        return;
    }
    ++panel->updating;
    panel->busy = 0;
    char *message = NULL;
    if (job->status != UMI_STATUS_OK)
    {
        if (job->load)
            message = g_strdup_printf("Load refused: %s. The current plan is unchanged.",
                                      umi_status_text(job->status));
        else
            message = g_strdup_printf(
                "File request refused: %s. Any partial output is retained at %s (%" G_GUINT64_FORMAT
                " bytes).",
                umi_status_text(job->status), job->path, (guint64)job->receipt.bytes_written);
    }
    else if (!job->load)
        message = g_strdup_printf("Captured plan %s to %s. Later edits were not included.",
                                  job->csv ? "exported" : "saved", job->path);
    else if (job->generation != panel->generation)
        message = g_strdup("Inputs changed while loading. Load again to review against the current plan.");
    else
    {
        char *before = NULL, *after = NULL;
        size_t beforeSize = 0U, afterSize = 0U;
        UmiStatus status = UmiCashPlanEncode(panel->plan, &before, &beforeSize);
        if (status == UMI_STATUS_OK)
            status = UmiCashPlanEncode(job->loaded, &after, &afterSize);
        if (status == UMI_STATUS_OK)
        {
            UmiCashPlanDestroy(panel->pending);
            panel->pending = job->loaded;
            job->loaded = NULL;
            char *review = g_strdup_printf(
                "CURRENT STORED ASSUMPTIONS\n%s\nLOADED ASSUMPTIONS (not applied)\n%s", before, after);
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->output)), review, -1);
            g_free(review);
            message = g_strdup(
                "Review both plans above, then approve and apply. No payment or trade will be created.");
        }
        else
            message = g_strdup(umi_status_text(status));
        UmiCashPlanFreeBytes(before);
        UmiCashPlanFreeBytes(after);
    }
    gtk_label_set_text(GTK_LABEL(panel->note), message);
    g_free(message);
    --panel->updating;
    g_object_unref(root);
}
void UmiCashPanelFile(GtkButton *button, gpointer root)
{
    CashPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-cash-plan");
    if (panel == NULL || panel->updating || panel->busy)
        return;
    g_object_ref(root);
    ++panel->updating;
    if (panel->configDirty || panel->entryDirty || panel->generation == UINT64_MAX)
    {
        gtk_label_set_text(GTK_LABEL(panel->note), "Apply or reset edited fields before file operations.");
        --panel->updating;
        g_object_unref(root);
        return;
    }
    const char *path = gtk_editable_get_text(GTK_EDITABLE(panel->fields[CASH_PATH]));
    if (UmiOutputFileValidatePath(path) != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(panel->note),
                           "Choose an absolute file path in a directory you control.");
        --panel->updating;
        g_object_unref(root);
        return;
    }
    CashFileJob *job = g_new0(CashFileJob, 1);
    g_weak_ref_init(&job->root, root);
    job->path = g_strdup(path);
    const char *action = g_object_get_data(G_OBJECT(button), "cash-action");
    job->load = strcmp(action, "load") == 0;
    job->csv = strcmp(action, "export") == 0;
    UmiStatus status = job->load ? UMI_STATUS_OK : UmiCashPlanCopy(panel->plan, &job->snapshot);
    if (status != UMI_STATUS_OK)
    {
        FreeJob(job);
        gtk_label_set_text(GTK_LABEL(panel->note), umi_status_text(status));
        --panel->updating;
        g_object_unref(root);
        return;
    }
    UmiCashPanelInvalidate(panel);
    job->generation = panel->generation;
    panel->busy = 1;
    GTask *task = g_task_new(NULL, NULL, Complete, NULL);
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Worker);
    g_object_unref(task);
    gtk_label_set_text(GTK_LABEL(panel->note),
                       "Working with the captured file request. Loading will require review and apply.");
    --panel->updating;
    g_object_unref(root);
}
void UmiCashPanelApplyLoaded(GtkButton *button, gpointer root)
{
    (void)button;
    CashPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-cash-plan");
    if (panel == NULL || panel->updating)
        return;
    g_object_ref(root);
    ++panel->updating;
    if (panel->pending == NULL || !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)))
    {
        gtk_label_set_text(GTK_LABEL(panel->note), "Load, review and approve a plan before applying it.");
        --panel->updating;
        g_object_unref(root);
        return;
    }
    /* Transfer a complete validated owner. Retain the previous model for one
     * explicit undo; no failure-prone work remains after this publication. */
    UmiCashPlanDestroy(panel->previous);
    panel->previous = panel->plan;
    panel->plan = panel->pending;
    panel->pending = NULL;
    UmiCashPanelInvalidate(panel);
    UmiCashPanelSync(panel);
    UmiCashPanelRender(panel);
    gtk_label_set_text(GTK_LABEL(panel->note),
                       "Reviewed assumptions applied locally. Undo restores the previous "
                       "plan; save explicitly to a new file.");
    --panel->updating;
    g_object_unref(root);
}
