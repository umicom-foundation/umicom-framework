/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ai_workspace_evidence_export.inc
 * PURPOSE: Save copied AI evidence on a file worker without lending it workspace or widget pointers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/evidence_export.h"
#include "umicom/ui/gtk4/path_picker.h"

typedef struct AiEvidenceExport {
    GWeakRef host;
    GtkWidget *root, *path, *save, *status;
    bool closed, busy;
} AiEvidenceExport;

typedef struct AiEvidenceWrite {
    GWeakRef root;
    UmiAiEvidenceReview *review;
    UmiOutputFileSnapshot file;
    char *path;
    char jobId[UMI_AI_WORKSPACE_ID_CAPACITY];
    UmiStatus status;
} AiEvidenceWrite;

static void AiEvidenceExportFree(gpointer data)
{
    AiEvidenceExport *export = data;
    g_weak_ref_clear(&export->host);
    g_free(export);
}
static void AiEvidenceExportRetire(GtkWidget *root)
{
    if (root == NULL) return;
    g_object_ref(root);
    AiEvidenceExport *export = g_object_get_data(G_OBJECT(root), "umicom-ai-evidence-export");
    /* An accepted write owns a capture and can finish after closure. Retiring
     * the view prevents its completion from touching a replacement workspace. */
    export->closed = true;
    g_weak_ref_set(&export->host, NULL);
    gtk_widget_set_sensitive(root, FALSE);
    g_object_unref(root);
}
static UmiAiWorkspaceGtkPanel *AiEvidenceExportOwner(AiEvidenceExport *export)
{
    if (export->closed) return NULL;
    GObject *host = g_weak_ref_get(&export->host);
    UmiAiWorkspaceGtkPanel *panel = host != NULL ?
        g_object_get_data(host, "umicom-ai-workspace-owner") : NULL;
    g_clear_object(&host);
    return Available(panel) ? panel : NULL;
}
static void AiEvidenceWriteFree(gpointer data)
{
    AiEvidenceWrite *write = data;
    g_weak_ref_clear(&write->root);
    UmiAiEvidenceDestroy(write->review);
    g_free(write->path);
    g_free(write);
}
static void AiEvidenceWriteWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    AiEvidenceWrite *write = data;
    write->status = UmiAiEvidenceWriteNew(write->review, write->path, &write->file);
    g_task_return_boolean(task, TRUE);
}
static void AiEvidenceWriteDone(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    AiEvidenceWrite *write = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWidget *root = g_weak_ref_get(&write->root);
    if (root == NULL) return;
    AiEvidenceExport *export = g_object_get_data(G_OBJECT(root), "umicom-ai-evidence-export");
    if (!export->closed) {
        export->busy = false;
        char *message = g_strdup_printf("Job %s: %s\n%s\n%s",
            write->jobId, umi_status_text(write->status), write->path,
            write->status == UMI_STATUS_OK ?
                "Saved the captured inspection. Review its private content before sharing." :
                "Existing files were not replaced. Inspect any partial new file and choose another name to retry.");
        gtk_label_set_text(GTK_LABEL(export->status), message);
        g_free(message);
        if (!export->closed) gtk_widget_set_sensitive(export->save, TRUE);
    }
    g_object_unref(root);
}
static void AiEvidenceExportClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiEvidenceExport *export = g_object_get_data(G_OBJECT(root), "umicom-ai-evidence-export");
    UmiAiWorkspaceGtkPanel *panel = AiEvidenceExportOwner(export);
    if (export->closed || export->busy) { g_object_unref(root); return; }
    if (panel == NULL) {
        gtk_label_set_text(GTK_LABEL(export->status), "Finish the current model request before capturing a report.");
        g_object_unref(root); return;
    }
    AiEvidenceWrite *write = g_new0(AiEvidenceWrite, 1);
    g_weak_ref_init(&write->root, root);
    write->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(export->path)));
    write->status = UmiOutputFileValidatePath(write->path);
    if (write->status == UMI_STATUS_OK)
        write->status = UmiAiEvidenceCapture(panel->workspace, panel->selectedId, &write->review);
    if (write->status == UMI_STATUS_OK)
        (void)g_strlcpy(write->jobId, panel->selectedId, sizeof(write->jobId));
    /* All workspace reads finish before any GTK notification can close or
     * rebuild the host. Only the owned capture crosses the worker boundary. */
    if (write->status != UMI_STATUS_OK) {
        char *message = g_strdup_printf("Report not started: %s. Load a saved job and choose a new absolute path.",
            umi_status_text(write->status));
        AiEvidenceWriteFree(write);
        gtk_label_set_text(GTK_LABEL(export->status), message);
        g_free(message);
        g_object_unref(root); return;
    }
    export->busy = true;
    GTask *task = g_task_new(NULL, NULL, AiEvidenceWriteDone, NULL);
    g_task_set_task_data(task, write, AiEvidenceWriteFree);
    g_task_run_in_thread(task, AiEvidenceWriteWorker);
    g_object_unref(task);
    gtk_widget_set_sensitive(export->save, FALSE);
    if (!export->closed)
        gtk_label_set_text(GTK_LABEL(export->status),
            "Writing the captured job inspection. Closing the window does not undo this requested write.");
    g_object_unref(root);
}
static GtkWidget *AiEvidenceExportCreate(UmiAiWorkspaceGtkPanel *panel)
{
    AiEvidenceExport *export = g_new0(AiEvidenceExport, 1);
    g_weak_ref_init(&export->host, panel->root);
    export->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_ref_sink(export->root);
    g_object_set_data_full(G_OBJECT(export->root), "umicom-ai-evidence-export", export, AiEvidenceExportFree);
    GtkWidget *notice = gtk_label_new(
        "Save the currently selected job's prompt, returned draft and source inspection as plain text. "
        "The report can contain private material. Select a new file you control.");
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_box_append(GTK_BOX(export->root), notice);
    export->path = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(export->path), (int)UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(export->path), "New absolute .txt filename");
    Tag(export->path, "ai.evidence.export.path");
    gtk_box_append(GTK_BOX(export->root), export->path);
    GtkWidget *browse = UmiPathPickerGtk4Create(GTK_ENTRY(export->path),
        UMI_PATH_PICKER_NEW_FILE, "Choose an AI inspection report location", "ai-inspection.txt");
    if (browse != NULL) gtk_box_append(GTK_BOX(export->root), browse);
    export->save = gtk_button_new_with_label("Save current job inspection");
    Tag(export->save, "ai.evidence.export.save");
    gtk_box_append(GTK_BOX(export->root), export->save);
    export->status = gtk_label_new("No inspection report has been written.");
    gtk_label_set_wrap(GTK_LABEL(export->status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(export->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(export->status), 0.0F);
    Tag(export->status, "ai.evidence.export.status");
    gtk_box_append(GTK_BOX(export->root), export->status);
    g_signal_connect_object(export->save, "clicked", G_CALLBACK(AiEvidenceExportClicked), export->root, 0);
    return export->root;
}
