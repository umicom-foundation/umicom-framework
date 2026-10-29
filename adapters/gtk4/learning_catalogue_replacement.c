/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_replacement.c
 *
 * PURPOSE:
 *   Prepare reviewed catalogue revisions without replacing a live reading session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_catalogue_private.h"
#include <inttypes.h>
#include <string.h>

static UmiStatus ReplacementReady(LearningCatalogue *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    return owner->state.busy ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}

UmiStatus UmiLearningCatalogueGtk4PrepareReplacement(GtkWidget *widget, const char *directory)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = ReplacementReady(owner);
    if (status != UMI_STATUS_OK) return status;
    if (directory == NULL || !umi_path_is_absolute(directory)) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->review_serial == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root, widget);
    job->host = owner->host;
    job->action = LEARNING_CATALOGUE_PREPARE_REPLACEMENT;
    status = umi_path_copy(job->directory, sizeof(job->directory), directory);
    if (status == UMI_STATUS_OK && !g_utf8_validate(job->directory, -1, NULL))
        status = UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningShelfSnapshot snapshot;
    if (status == UMI_STATUS_OK) status = UmiLearningShelfCopySnapshot(owner->shelf, &snapshot);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfClone(owner->shelf, &job->selection_shelf);
    if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    job->replacement.shelf_generation = snapshot.generation;
    job->replacement.review_serial = owner->review_serial + 1U;
    g_object_ref(widget);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}

void LearningCataloguePrepareReplacement(LearningCatalogueJob *job)
{
    /* The worker owns a cloned shelf, so even successful preparation cannot
     * change the current UI, invalidate its results or destroy its reader. */
    job->status = UmiLearningPackLoad(job->directory, "learning-pack.json", job->host, &job->pack);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiLearningPackCopyInfo(job->pack, &job->replacement.proposed);
    UmiLearningPack *previous = NULL;
    if (job->status == UMI_STATUS_OK)
        job->status = UmiLearningShelfCopyPack(job->selection_shelf, job->replacement.proposed.id, &previous);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiLearningPackCopyInfo(previous, &job->replacement.previous);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiLearningPackCompare(previous, job->pack, &job->replacement_changes);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiLearningPackChangesCopySummary(job->replacement_changes, &job->replacement.changes);
    UmiLearningPackDestroy(previous);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiLearningShelfReplace(job->selection_shelf,
            job->replacement.shelf_generation, &job->pack);
}

static const char *ReplacementResourceKind(UmiLearningResourceKind kind)
{
    switch (kind) {
    case UMI_LEARNING_RESOURCE_LESSON: return "lesson";
    case UMI_LEARNING_RESOURCE_GUIDE: return "guide";
    case UMI_LEARNING_RESOURCE_EXAMPLE: return "example";
    case UMI_LEARNING_RESOURCE_IMAGE: return "image";
    }
    return "unknown";
}

static void ReplacementDetails(LearningCatalogue *owner)
{
    const UmiLearningPackChangeSummary *summary = &owner->replacement.changes;
    GString *text = g_string_new(NULL);
    g_string_append_printf(text, "%zu added, %zu removed, %zu modified, %zu unchanged. "
        "%zu modified resource hashes differ. These are manifest declarations, not verified file bytes.\n",
        summary->added, summary->removed, summary->modified, summary->unchanged, summary->changed_digests);
    for (size_t i = 0U; i < summary->change_count; ++i) {
        UmiLearningResourceChange change;
        if (UmiLearningPackChangeAt(owner->replacement_changes, i, &change) != UMI_STATUS_OK) break;
        const char *kind = change.kind == UMI_LEARNING_RESOURCE_ADDED ? "Added" :
            change.kind == UMI_LEARNING_RESOURCE_REMOVED ? "Removed" : "Modified";
        const char *id = change.kind == UMI_LEARNING_RESOURCE_ADDED ? change.proposed.id : change.previous.id;
        g_string_append_printf(text, "\n%s: %s\n", kind, id);
        if (change.kind != UMI_LEARNING_RESOURCE_ADDED)
            g_string_append_printf(text, "Previous file: %s\nPrevious SHA-256: %s\n", change.previous.path, change.previous.sha256);
        if (change.kind != UMI_LEARNING_RESOURCE_REMOVED)
            g_string_append_printf(text, "Proposed file: %s\nProposed SHA-256: %s\n", change.proposed.path, change.proposed.sha256);
        if ((change.fields & UMI_LEARNING_CHANGE_TITLE) != 0U)
            g_string_append_printf(text, "Title: %s → %s\n", change.previous.title, change.proposed.title);
        if ((change.fields & UMI_LEARNING_CHANGE_TOPIC) != 0U)
            g_string_append_printf(text, "Topic: %s → %s\n", change.previous.topic, change.proposed.topic);
        if ((change.fields & UMI_LEARNING_CHANGE_KIND) != 0U)
            g_string_append_printf(text, "Resource kind: %s → %s\n",
                ReplacementResourceKind(change.previous.kind), ReplacementResourceKind(change.proposed.kind));
        if ((change.fields & UMI_LEARNING_CHANGE_EXAMPLE) != 0U)
            g_string_append_printf(text, "Linked example: %s → %s\n", change.previous.example_id, change.proposed.example_id);
    }
    char *display = g_utf8_make_valid(text->str, (gssize)text->len);
    gtk_label_set_text(GTK_LABEL(owner->replacement_details), display);
    g_free(display);
    g_string_free(text, TRUE);
}

void LearningCatalogueCompleteReplacement(LearningCatalogue *owner, LearningCatalogueJob *job)
{
    if (job->status == UMI_STATUS_OK) {
        UmiLearningShelfDestroy(owner->replacement_shelf);
        UmiLearningPackChangesDestroy(owner->replacement_changes);
        owner->replacement_changes = job->replacement_changes;
        job->replacement_changes = NULL;
        owner->replacement_shelf = job->selection_shelf;
        job->selection_shelf = NULL;
        owner->replacement = job->replacement;
        owner->review_serial = job->replacement.review_serial;
        char *text = g_strdup_printf("Review %s (%s, %s): revision %" PRIu64
            " to %" PRIu64 ". Accept explicitly to replace the catalogue entry. "
            "The current reading session will keep its earlier revision.",
            owner->replacement.proposed.id, owner->replacement.proposed.product,
            owner->replacement.proposed.locale, owner->replacement.previous.revision,
            owner->replacement.proposed.revision);
        gtk_label_set_text(GTK_LABEL(owner->replacement_status), text);
        g_free(text);
        if (!owner->state.retired) ReplacementDetails(owner);
    }
    if (!owner->state.retired)
        LearningCatalogueMessage(owner, job->status, job->status == UMI_STATUS_OK ?
            "Replacement prepared for review. The current catalogue is unchanged." :
            "Replacement was refused. The current catalogue and any earlier review were kept.");
}

UmiStatus UmiLearningCatalogueGtk4CopyReplacement(GtkWidget *widget,
    UmiLearningCatalogueReplacement *out)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->replacement_shelf == NULL) return UMI_STATUS_NOT_FOUND;
    *out = owner->replacement;
    return UMI_STATUS_OK;
}

UmiStatus UmiLearningCatalogueGtk4ReplacementChangeAt(GtkWidget *widget,
    uint64_t serial, size_t index, UmiLearningResourceChange *out)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->replacement_shelf == NULL) return UMI_STATUS_NOT_FOUND;
    if (owner->replacement.review_serial != serial) return UMI_STATUS_BUSY;
    return UmiLearningPackChangeAt(owner->replacement_changes, index, out);
}

UmiStatus UmiLearningCatalogueGtk4AcceptReplacement(GtkWidget *widget, uint64_t serial)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = ReplacementReady(owner);
    if (status != UMI_STATUS_OK) return status;
    if (owner->replacement_shelf == NULL) return UMI_STATUS_NOT_FOUND;
    if (owner->replacement.review_serial != serial) return UMI_STATUS_BUSY;
    UmiLearningShelfSnapshot snapshot;
    status = UmiLearningShelfCopySnapshot(owner->shelf, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.generation != owner->replacement.shelf_generation) return UMI_STATUS_BUSY;
    g_object_ref(widget);
    owner->state.busy = 1;
    status = LearningCataloguePublishShelf(owner, &owner->replacement_shelf,
        gtk_editable_get_text(GTK_EDITABLE(owner->query)));
    if (status == UMI_STATUS_OK) {
        UmiLearningPackChangesDestroy(owner->replacement_changes);
        owner->replacement_changes = NULL;
        memset(&owner->replacement, 0, sizeof(owner->replacement));
        if (!owner->state.retired)
            gtk_label_set_text(GTK_LABEL(owner->replacement_details), "No pending resource comparison.");
        if (!owner->state.retired)
            gtk_label_set_text(GTK_LABEL(owner->replacement_status), "Replacement accepted. No pending review.");
    }
    if (!owner->state.retired)
        LearningCatalogueMessage(owner, status, status == UMI_STATUS_OK ?
            "Catalogue updated. Open a result to read the new revision; the existing reader was kept." :
            "Catalogue was not changed. The pending review was kept for another attempt.");
    owner->state.busy = 0;
    g_object_unref(widget);
    return status;
}

UmiStatus UmiLearningCatalogueGtk4DiscardReplacement(GtkWidget *widget, uint64_t serial)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = ReplacementReady(owner);
    if (status != UMI_STATUS_OK) return status;
    if (owner->replacement_shelf == NULL) return UMI_STATUS_NOT_FOUND;
    if (owner->replacement.review_serial != serial) return UMI_STATUS_BUSY;
    g_object_ref(widget);
    owner->state.busy = 1;
    UmiLearningShelfDestroy(owner->replacement_shelf);
    owner->replacement_shelf = NULL;
    UmiLearningPackChangesDestroy(owner->replacement_changes);
    owner->replacement_changes = NULL;
    gtk_label_set_text(GTK_LABEL(owner->replacement_details), "No pending resource comparison.");
    memset(&owner->replacement, 0, sizeof(owner->replacement));
    if (!owner->state.retired)
        gtk_label_set_text(GTK_LABEL(owner->replacement_status), "Replacement discarded. No pending review.");
    if (!owner->state.retired)
        LearningCatalogueMessage(owner, UMI_STATUS_OK, "Pending replacement discarded. The catalogue was kept.");
    owner->state.busy = 0;
    g_object_unref(widget);
    return UMI_STATUS_OK;
}
