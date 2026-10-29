/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_jobs.c
 *
 * PURPOSE:
 *   Read learning packs off the GTK thread and publish only to a live owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_catalogue_private.h"
#include "umicom/browser/document_preview.h"
#include <string.h>

void LearningCatalogueJobFree(gpointer data)
{
    LearningCatalogueJob *job = data;
    if (job == NULL) return;
    g_weak_ref_clear(&job->root);
    UmiLearningShelfDestroy(job->selection_shelf);
    UmiLearningPackDestroy(job->pack);
    UmiLearningReaderDestroy(job->reader);
    g_free(job->display);
    g_free(job);
}

static int IsHtml(const char *path)
{
    size_t size = strlen(path);
    return (size >= 5U && g_ascii_strcasecmp(path + size - 5U, ".html") == 0) ||
        (size >= 4U && g_ascii_strcasecmp(path + size - 4U, ".htm") == 0);
}

/* Render from the verified buffer, never by reopening a filename. This first
 * adapter is deliberately a text presentation; it cannot execute markup,
 * fetch a remote image or grant permission to run a source example. */
static void PrepareDisplay(LearningCatalogueJob *job)
{
    size_t size = 0U;
    job->status = UmiLearningReaderCopySnapshot(job->reader, &job->snapshot);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiLearningReaderCopyContent(job->reader, NULL, 0U, &size);
    if (job->status != UMI_STATUS_OK) return;
    unsigned char *bytes = g_try_malloc(size + 1U);
    if (bytes == NULL) { job->status = UMI_STATUS_OUT_OF_MEMORY; return; }
    job->status = UmiLearningReaderCopyContent(job->reader, bytes, size + 1U, &size);
    if (job->status != UMI_STATUS_OK) { g_free(bytes); return; }
    if (job->snapshot.selection.resource.kind == UMI_LEARNING_RESOURCE_IMAGE) {
        job->display = g_strdup("The image bytes were verified. Image rendering is not available in this text reader.");
        g_strlcpy(job->notice, "Verified image; text-reader limitation.", sizeof(job->notice));
    } else if (!g_utf8_validate((const char *)bytes, (gssize)size, NULL)) {
        job->display = g_strdup("The bytes were verified, but this resource is not valid UTF-8 text. It has not been executed.");
        g_strlcpy(job->notice, "Verified content cannot be shown as UTF-8 text.", sizeof(job->notice));
    } else if (IsHtml(job->snapshot.selection.resource.path)) {
        UmiBrowserDocumentPreview *preview = g_try_new(UmiBrowserDocumentPreview, 1U);
        if (preview == NULL) job->status = UMI_STATUS_OUT_OF_MEMORY;
        else {
            job->status = umi_browser_document_preview_from_html((const char *)bytes, size, preview);
            if (job->status == UMI_STATUS_OK) {
                /* A bounded preview may stop inside a multi-byte character.
                 * Repair display bytes explicitly; the owned original remains
                 * unchanged in the reader for later copying or another host. */
                job->display = g_utf8_make_valid(preview->text, (gssize)preview->text_bytes);
                g_strlcpy(job->notice, preview->truncated ?
                    "Verified HTML; text preview shortened to its display limit." :
                    "Verified HTML shown as plain text; layout and images are not rendered.", sizeof(job->notice));
            }
            g_free(preview);
        }
    } else {
        job->display = (char *)bytes;
        bytes = NULL;
        g_strlcpy(job->notice, "Verified UTF-8 text. Source examples have not been executed.", sizeof(job->notice));
    }
    g_free(bytes);
}

static void CatalogueWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    LearningCatalogueJob *job = data;
    switch (job->action) {
    case LEARNING_CATALOGUE_LOAD:
        job->status = UmiLearningPackLoad(job->directory, "learning-pack.json", job->host, &job->pack);
        break;
    case LEARNING_CATALOGUE_OPEN:
        job->status = UmiLearningReaderCreateFromShelf(job->selection_shelf,
            job->selected.pack.id, job->selected.pack.revision, job->selected.resource.id,
            job->selected.resource.sha256, UMI_LEARNING_CATALOGUE_BYTES, &job->reader);
        break;
    case LEARNING_CATALOGUE_BACK:
        job->status = UmiLearningReaderBack(job->reader);
        break;
    case LEARNING_CATALOGUE_FORWARD:
        job->status = UmiLearningReaderForward(job->reader);
        break;
    case LEARNING_CATALOGUE_RELOAD:
        job->status = UmiLearningReaderReload(job->reader);
        break;
    }
    if (job->action != LEARNING_CATALOGUE_LOAD && job->status == UMI_STATUS_OK) PrepareDisplay(job);
    g_task_return_boolean(task, job->status == UMI_STATUS_OK);
}

static void CatalogueComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    (void)data;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    LearningCatalogueJob *job = g_task_get_task_data(task);
    GObject *root = g_weak_ref_get(&job->root);
    if (root == NULL) return;
    LearningCatalogue *owner = LearningCatalogueOwner(GTK_WIDGET(root));
    if (owner == NULL || owner->state.retired) { g_object_unref(root); return; }

    /* The weak root resolves only on the GTK thread. The job owns every C
     * object it used; no worker has borrowed a widget, shelf or application. */
    if (job->action == LEARNING_CATALOGUE_LOAD) {
        if (job->status == UMI_STATUS_OK)
            job->status = UmiLearningShelfAdopt(owner->shelf, &job->pack);
        if (job->status == UMI_STATUS_OK)
            job->status = LearningCatalogueRefresh(owner,
                gtk_editable_get_text(GTK_EDITABLE(owner->query)));
        if (!owner->state.retired)
            LearningCatalogueMessage(owner, job->status,
                "Pack added. Select a result and choose Open selected.");
    } else {
        /* Navigation temporarily transfers the existing reader to the worker.
         * Return it even when navigation failed: its prior bytes/history were
         * preserved by the core service. A new-open failure keeps the old reader. */
        if (job->reader != NULL) {
            UmiLearningReaderDestroy(owner->reader);
            owner->reader = job->reader;
            job->reader = NULL;
        }
        if (job->snapshot.has_document) {
            owner->state.reader = job->snapshot;
            char *title = g_utf8_make_valid(job->snapshot.selection.resource.title, -1);
            gtk_label_set_text(GTK_LABEL(owner->heading), title);
            g_free(title);
            if (!owner->state.retired) {
                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(owner->content));
                gtk_text_buffer_set_text(buffer, job->display != NULL ? job->display : "", -1);
            }
        }
        if (!owner->state.retired)
            LearningCatalogueMessage(owner, job->status,
                job->notice[0] != '\0' ? job->notice : "The previous reading selection was kept.");
    }
    /* Notifications may retire the owner. busy stays true until publication is
     * finished, so callbacks cannot dispatch a second job halfway through it. */
    owner->state.busy = 0;
    if (!owner->state.retired) gtk_widget_set_sensitive(owner->controls, TRUE);
    g_object_unref(root);
}

UmiStatus LearningCatalogueDispatch(LearningCatalogue *owner, LearningCatalogueJob *job)
{
    owner->state.busy = 1;
    owner->state.last_status = UMI_STATUS_OK;
    gtk_widget_set_sensitive(owner->controls, FALSE);
    if (!owner->state.retired)
        gtk_label_set_text(GTK_LABEL(owner->status), "Reading local learning content…");
    if (owner->state.retired) {
        owner->state.busy = 0;
        /* A transferred navigation reader belongs back to its closed owner
         * until final disposal; do not leak it if a notification retired us. */
        if (job->action >= LEARNING_CATALOGUE_BACK) {
            owner->reader = job->reader;
            job->reader = NULL;
        }
        LearningCatalogueJobFree(job);
        return UMI_STATUS_INVALID_STATE;
    }
    GTask *task = g_task_new(NULL, NULL, CatalogueComplete, NULL);
    g_task_set_task_data(task, job, LearningCatalogueJobFree);
    g_task_run_in_thread(task, CatalogueWorker);
    g_object_unref(task);
    return UMI_STATUS_OK;
}
