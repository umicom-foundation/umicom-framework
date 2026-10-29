/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_progress.c
 *
 * PURPOSE:
 *   Record explicit private checkpoints without sharing storage or relabelling reader state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_catalogue_private.h"
#include "umicom/base/text.h"
#include "umicom/ui/gtk4/path_picker.h"
#include <inttypes.h>
#include <string.h>

int UmiLearningCatalogueGtk4ProgressAvailable(void)
{
#ifdef UMICOM_LEARNING_PROGRESS_AVAILABLE
    return 1;
#else
    return 0;
#endif
}
UmiStatus UmiLearningCatalogueGtk4Progress(GtkWidget *widget,
    const UmiLearningProgressLocalRequest *request)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->state.busy) return UMI_STATUS_BUSY;
#ifndef UMICOM_LEARNING_PROGRESS_AVAILABLE
    return UMI_STATUS_UNAVAILABLE;
#else
    UmiStatus status = UmiLearningProgressLocalValidate(request);
    if (status != UMI_STATUS_OK) return status;
    if (!owner->state.reader.has_document || owner->reader == NULL ||
        (owner->state.reader.selection.resource.kind != UMI_LEARNING_RESOURCE_GUIDE &&
        owner->state.reader.selection.resource.kind != UMI_LEARNING_RESOURCE_LESSON))
        return UMI_STATUS_INVALID_STATE;
    if (request->action == UMI_LEARNING_PROGRESS_RECORD && owner->state.progress_write_blocked)
        return UMI_STATUS_INVALID_STATE;
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root, widget);
    job->action = LEARNING_CATALOGUE_PROGRESS;
    job->progress_receipt.request = *request;
    job->progress_receipt.selection = owner->state.reader;
    status = umi_path_copy(job->progress_receipt.request.database_path,
        sizeof(job->progress_receipt.request.database_path), request->database_path);
    if (status == UMI_STATUS_OK && !g_utf8_validate(job->progress_receipt.request.database_path, -1, NULL))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK) status = UmiLearningReaderClone(owner->reader, &job->progress_reader);
    if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    /* Keep the visible reader owned by the panel. The job owns an independent
     * verified snapshot, so neither delayed storage nor changing entry text can
     * redirect the checkpoint after its explicit acceptance. */
    g_object_ref(widget);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
#endif
}
void LearningCatalogueProgressWorker(LearningCatalogueJob *job)
{
#ifdef UMICOM_LEARNING_PROGRESS_AVAILABLE
    job->status = UmiLearningProgressLocalExecute(job->progress_reader,
        &job->progress_receipt.request, &job->progress_receipt);
#else
    job->status = UMI_STATUS_UNAVAILABLE;
#endif
    job->progress_receipt.status = job->status;
}
void LearningCatalogueProgressComplete(LearningCatalogue *owner, LearningCatalogueJob *job)
{
    owner->progress_receipt = job->progress_receipt;
    owner->state.has_progress_result = 1;
    const UmiLearningProgressLocalReceipt *receipt = &owner->progress_receipt;
    if (receipt->journal_loaded && receipt->journal.recovery_required) {
        owner->state.progress_write_blocked = 1;
        memcpy(owner->progress_recovery_path, receipt->request.database_path, sizeof(owner->progress_recovery_path));
        memcpy(owner->progress_recovery_learner, receipt->request.learner_id, sizeof(owner->progress_recovery_learner));
    } else if (owner->state.progress_write_blocked && receipt->request.action == UMI_LEARNING_PROGRESS_INSPECT &&
        receipt->journal_loaded && (receipt->status == UMI_STATUS_OK || receipt->status == UMI_STATUS_NOT_FOUND) &&
        strcmp(receipt->request.database_path, owner->progress_recovery_path) == 0 &&
        strcmp(receipt->request.learner_id, owner->progress_recovery_learner) == 0) {
        owner->state.progress_write_blocked = 0;
        owner->progress_recovery_path[0] = '\0';
        owner->progress_recovery_learner[0] = '\0';
    }
    char *text = g_strdup_printf(
        "%s — %s\nLearner: %s; checkpoint: %s\nDocument: %s::%s, revision %" PRIu64
        "\nDatabase: %s\nJournal loaded: %s; events: %zu; matching event: %s; sequence: %" PRIu64
        "%s",
        receipt->request.action == UMI_LEARNING_PROGRESS_RECORD ? "Record checkpoint" : "Inspect checkpoint",
        umi_status_text(receipt->status), receipt->request.learner_id, receipt->request.checkpoint,
        receipt->selection.pack.id, receipt->selection.selection.resource.id, receipt->selection.pack.revision,
        receipt->request.database_path, receipt->journal_loaded ? "yes" : "no", receipt->journal.event_count,
        receipt->has_event ? "yes" : "no", receipt->has_event ? receipt->event.sequence : UINT64_C(0),
        owner->state.progress_write_blocked ? "\nWrites are blocked: inspect the same database and learner to review uncertain storage." : "");
    gtk_label_set_text(GTK_LABEL(owner->progress_status), text);
    g_free(text);
    if (!owner->state.retired)
        LearningCatalogueMessage(owner, receipt->status, receipt->status == UMI_STATUS_OK ?
            "Explicit checkpoint request completed. The reading session was kept." :
            receipt->status == UMI_STATUS_NOT_FOUND && receipt->journal_loaded ?
                "No checkpoint matches this lesson revision and content hash." :
            receipt->status == UMI_STATUS_ALREADY_EXISTS ?
                "That exact checkpoint already exists. No new event was appended." :
                "Checkpoint request did not complete successfully. Inspect its captured receipt before retrying.");
}
UmiStatus UmiLearningCatalogueGtk4CopyProgressReceipt(GtkWidget *widget,
    UmiLearningProgressLocalReceipt *out)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!owner->state.has_progress_result) return UMI_STATUS_NOT_FOUND;
    *out = owner->progress_receipt;
    return UMI_STATUS_OK;
}
#ifdef UMICOM_LEARNING_PROGRESS_AVAILABLE
static void ProgressAction(GtkButton *button, gpointer data)
{
    GtkWidget *widget = data;
    g_object_ref(widget);
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner != NULL && !owner->state.retired && !owner->state.busy) {
        UmiLearningProgressLocalRequest request = {0};
        request.action = (UmiLearningProgressLocalAction)GPOINTER_TO_UINT(
            g_object_get_data(G_OBJECT(button), "learning-progress-action"));
        UmiStatus status = umi_path_copy(request.database_path, sizeof(request.database_path),
            gtk_editable_get_text(GTK_EDITABLE(owner->progress_path)));
        if (status == UMI_STATUS_OK) status = umi_text_copy(request.learner_id, sizeof(request.learner_id),
            gtk_editable_get_text(GTK_EDITABLE(owner->progress_learner)));
        if (status == UMI_STATUS_OK) status = umi_text_copy(request.checkpoint, sizeof(request.checkpoint),
            gtk_editable_get_text(GTK_EDITABLE(owner->progress_checkpoint)));
        if (status == UMI_STATUS_OK && request.action == UMI_LEARNING_PROGRESS_RECORD)
            status = umi_text_copy(request.evidence_reference, sizeof(request.evidence_reference),
                gtk_editable_get_text(GTK_EDITABLE(owner->progress_evidence)));
        if (status == UMI_STATUS_OK) status = UmiLearningCatalogueGtk4Progress(widget, &request);
        if (status != UMI_STATUS_OK && !owner->state.retired)
            LearningCatalogueMessage(owner, status, "Choose a private database, learner ID and checkpoint for the displayed lesson.");
    }
    g_object_unref(widget);
}
static GtkWidget *ProgressLabel(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    return label;
}
static GtkWidget *ProgressEntry(GtkWidget *box, const char *caption, const char *id, int maximum)
{
    ProgressLabel(box, caption);
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry), maximum);
    g_object_set_data_full(G_OBJECT(entry), "umicom-automation-id", g_strdup(id), g_free);
    gtk_box_append(GTK_BOX(box), entry);
    return entry;
}
static void ProgressButton(LearningCatalogue *owner, GtkWidget *box, const char *text,
    const char *id, UmiLearningProgressLocalAction action)
{
    GtkWidget *button = gtk_button_new_with_label(text);
    g_object_set_data_full(G_OBJECT(button), "umicom-automation-id", g_strdup(id), g_free);
    g_object_set_data(G_OBJECT(button), "learning-progress-action", GUINT_TO_POINTER((guint)action));
    g_signal_connect_object(button, "clicked", G_CALLBACK(ProgressAction), owner->root, 0);
    gtk_box_append(GTK_BOX(box), button);
}
#endif
void LearningCatalogueProgressControls(LearningCatalogue *owner)
{
#ifdef UMICOM_LEARNING_PROGRESS_AVAILABLE
    GtkWidget *expander = gtk_expander_new("Private learning checkpoints");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    ProgressLabel(box, "Record a checkpoint only when you choose to. This is local plaintext history, "
        "not an assessment or proof of completion. Keep the database outside course folders and public repositories.");
    owner->progress_path = ProgressEntry(box, "Private database filename (absolute path)",
        "learning.catalogue.progress-path", (int)UMI_PATH_CAPACITY - 1);
    GtkWidget *picker = UmiPathPickerGtk4Create(GTK_ENTRY(owner->progress_path),
        UMI_PATH_PICKER_NEW_FILE, "Choose a private checkpoint database", NULL);
    if (picker != NULL) gtk_box_append(GTK_BOX(box), picker);
    ProgressLabel(box, "Continue an existing private database, or type its path above. The parent folder must exist.");
    GtkWidget *existing = UmiPathPickerGtk4Create(GTK_ENTRY(owner->progress_path),
        UMI_PATH_PICKER_EXISTING_FILE, "Choose an existing checkpoint database", NULL);
    if (existing != NULL) gtk_box_append(GTK_BOX(box), existing);
    owner->progress_learner = ProgressEntry(box, "Local learner ID",
        "learning.catalogue.progress-learner", (int)UMI_TEACHER_ID_CAPACITY - 1);
    owner->progress_checkpoint = ProgressEntry(box, "Checkpoint ID (for example, reviewed)",
        "learning.catalogue.progress-checkpoint", (int)UMI_TEACHER_ID_CAPACITY - 1);
    owner->progress_evidence = ProgressEntry(box, "Evidence reference for recording (for example, practice-1)",
        "learning.catalogue.progress-evidence", (int)UMI_TEACHER_ID_CAPACITY - 1);
    ProgressLabel(box, "IDs start with a letter or digit and use letters, digits, dots, hyphens or underscores. "
        "Use short references, not personal notes, secrets or file paths.");
    ProgressButton(owner, box, "Inspect checkpoint", "learning.catalogue.progress-inspect", UMI_LEARNING_PROGRESS_INSPECT);
    ProgressButton(owner, box, "Record checkpoint", "learning.catalogue.progress-record", UMI_LEARNING_PROGRESS_RECORD);
    ProgressLabel(box, "Inspect never creates a missing database. Record may create it and append one event. "
        "An accepted write may finish after this window closes; it is not undone by closing.");
    owner->progress_status = ProgressLabel(box, "No checkpoint operation has completed.");
    gtk_label_set_selectable(GTK_LABEL(owner->progress_status), TRUE);
    g_object_set_data_full(G_OBJECT(owner->progress_status), "umicom-automation-id",
        g_strdup("learning.catalogue.progress-result"), g_free);
    gtk_expander_set_child(GTK_EXPANDER(expander), box);
    gtk_box_append(GTK_BOX(owner->controls), expander);
#else
    (void)owner;
#endif
}
