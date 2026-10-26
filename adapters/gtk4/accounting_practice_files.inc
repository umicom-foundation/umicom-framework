/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/accounting_practice_files.inc
 *
 * PURPOSE:
 *   Own asynchronous accounting file snapshots and require review before local replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Included by accounting_practice_gtk4.c after its shared owner helpers. This file
 * has no independent GTK ownership: window state remains in AccountingPanel. */
typedef struct AccountingFileJob {
    GWeakRef window;
    /* The original draft type name combined two domain names. Preserve it
     * disabled; captured and imported values use the public accounting owner. */
#if 0
    UmiOpenTmsFxAccountingBook *captured, *loaded;
#endif
    UmiOpenTmsAccountingBook *captured, *loaded;
    char *path;
    uint64_t generation;
    gboolean load;
    UmiStatus status;
    UmiOutputFileSnapshot receipt;
} AccountingFileJob;
static void FileJobFree(gpointer value) {
    AccountingFileJob *job = value;
    g_weak_ref_clear(&job->window);
    g_free(job->captured); g_free(job->loaded); g_free(job->path); g_free(job);
}
static void FileWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel) {
    (void)source; (void)cancel;
    AccountingFileJob *job = data;
    /* Workers never dereference a window, control or live book. Accepted writes
     * finish against this immutable capture even if the practice window closes. */
    job->status = job->load
        ? umi_open_tms_accounting_book_load(job->path, job->loaded)
        : umi_open_tms_accounting_book_save_new(job->path, job->captured, &job->receipt);
    g_task_return_boolean(task, TRUE);
}
static void AppendBook(GString *text, const char *heading, const UmiOpenTmsAccountingBook *book) {
    g_string_append_printf(text, "%s — %zu lines, revision %" G_GUINT64_FORMAT
        " | Approved %s | Posted locally %s\n",
        heading, book->count, (guint64)book->revision,
        book->approved ? "yes" : "no", book->posted ? "yes" : "no");
    for (size_t i = 0U; i < book->count; ++i) AppendLine(text, i, &book->lines[i]);
}
static gboolean BookTextForGtk(const UmiOpenTmsAccountingBook *book) {
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiOpenTmsAccountingLine *row = &book->lines[i];
        if (!g_utf8_validate(row->accountId, -1, NULL) ||
            !g_utf8_validate(row->referenceId, -1, NULL) ||
            !g_utf8_validate(row->currency, -1, NULL)) return FALSE;
    }
    return TRUE;
}
static void FileComplete(GObject *source, GAsyncResult *result, gpointer unused);
static gboolean FileCompleteRetry(gpointer task) {
    FileComplete(NULL, G_ASYNC_RESULT(task), NULL);
    return G_SOURCE_REMOVE;
}
static void FileComplete(GObject *source, GAsyncResult *result, gpointer unused) {
    (void)source; (void)unused;
    GTask *task = G_TASK(result);
    AccountingFileJob *job = g_task_get_task_data(task);
    /* Task propagation happens after the native publication guard below. */
    GtkWindow *window = g_weak_ref_get(&job->window);
    if (window == NULL) return;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired) { g_object_unref(window); return; }
    if (panel->busy) {
        /* A notification handler can run a nested main loop. Retain the task
         * and defer its completion instead of entering another publication. */
        g_timeout_add_full(G_PRIORITY_DEFAULT, 10U, FileCompleteRetry, g_object_ref(task), g_object_unref);
        g_object_unref(window); return;
    }
    (void)g_task_propagate_boolean(task, NULL);
    panel->busy = TRUE;
    panel->fileBusy = FALSE;
    /* The portable codec admits byte strings; native presentation requires
     * UTF-8. Refuse invalid text before it reaches GTK or a review display. */
    if (job->load && job->status == UMI_STATUS_OK && !BookTextForGtk(job->loaded))
        job->status = UMI_STATUS_INVALID_ARGUMENT;
    char *message = NULL;
    if (job->status != UMI_STATUS_OK) {
        message = job->load
            ? g_strdup_printf("Open refused: %s. Current records are unchanged.", umi_status_text(job->status))
            : g_strdup_printf("Save refused: %s. Any partial output is retained at %s (%" G_GUINT64_FORMAT " bytes).",
                umi_status_text(job->status), job->path, (guint64)job->receipt.bytes_written);
    } else if (!job->load) {
        message = g_strdup_printf("Captured records saved to %s. Edits after the request were not included.", job->path);
    } else if (!Live(panel, job->generation)) {
        message = g_strdup("Inputs changed during Open. Open again to review the file against current records.");
    } else {
        size_t size = 0U;
        UmiStatus status = umi_open_tms_accounting_book_archive_encode(job->loaded, NULL, 0U, &size);
        unsigned char *bytes = status == UMI_STATUS_OK ? g_try_malloc(size) : NULL;
        if (status == UMI_STATUS_OK && bytes == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK)
            status = umi_open_tms_accounting_book_archive_encode(job->loaded, bytes, size, &size);
        if (status == UMI_STATUS_OK) {
            panel->pendingBytes = bytes; panel->pendingSize = size;
            panel->expected = job->captured; job->captured = NULL;
            panel->fileGeneration = panel->generation;
            GString *review = g_string_new("Review every record before replacing this local book.\n");
            AppendBook(review, "CURRENT", &panel->book);
            AppendBook(review, "LOADED — NOT APPLIED", job->loaded);
            g_string_append(review, "Imported lifecycle labels do not authorise approval, payment or settlement.\n");
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->filePreview)), review->str, -1);
            g_string_free(review, TRUE);
            if (Live(panel, job->generation) && panel->pendingBytes != NULL)
                message = g_strdup("Review CURRENT and LOADED records, then acknowledge and replace explicitly.");
            else message = g_strdup("The file review was invalidated by an edit. Open again.");
        } else {
            g_free(bytes);
            message = g_strdup_printf("File review refused: %s. Current records are unchanged.", umi_status_text(status));
        }
    }
    if (!panel->retired) gtk_label_set_text(GTK_LABEL(panel->note), message);
    g_free(message);
    /* Retain the earlier controls-only revocation for comparison; clearing the
     * full file preview also removes text invalidated by a notification edit. */
#if 0
    if (!panel->retired && panel->pendingBytes == NULL) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->fileAck), FALSE);
        if (!panel->retired) gtk_widget_set_sensitive(panel->fileApply, FALSE);
    }
#endif
    if (!panel->retired && panel->pendingBytes == NULL) RevokeFileDisplay(panel);
    panel->busy = FALSE;
    g_object_unref(window);
}
static void FileStart(GtkButton *button, gpointer window) {
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy || panel->fileBusy) return;
    g_object_ref(window); panel->busy = TRUE;
    Invalidate(panel);
    uint64_t generation = panel->generation;
    AccountingFileJob *job = g_new0(AccountingFileJob, 1);
    g_weak_ref_init(&job->window, window);
    job->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->filePath)));
    job->load = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "accounting-file-load")) != 0;
    job->generation = generation;
    job->captured = g_memdup2(&panel->book, sizeof panel->book);
    /* Preserve the draft allocation for review; allocate the actual public
     * accounting type so worker and codec ownership agree. */
#if 0
    if (job->load) job->loaded = g_new0(UmiOpenTmsFxAccountingBook, 1);
#endif
    if (job->load) job->loaded = g_new0(UmiOpenTmsAccountingBook, 1);
    UmiStatus status = generation == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED :
        UmiOutputFileValidatePath(job->path);
    RevokeDisplay(panel);
    if (status == UMI_STATUS_OK && Live(panel, generation)) {
        gtk_label_set_text(GTK_LABEL(panel->note),
            job->load ? "Opening captured path. Review is required before replacement." :
                        "Saving captured records to a new file. Later edits are not included.");
        /* GTK notifications can edit or close the host. No worker may start if
         * that happened before dispatch; thus a refused save creates no file. */
        if (Live(panel, generation)) {
            panel->fileBusy = TRUE;
            GTask *task = g_task_new(NULL, NULL, FileComplete, NULL);
            g_task_set_task_data(task, job, FileJobFree);
            g_task_run_in_thread(task, FileWorker);
            g_object_unref(task);
            job = NULL;
        }
    } else if (!panel->retired && Live(panel, generation)) {
        gtk_label_set_text(GTK_LABEL(panel->note), status == UMI_STATUS_CAPACITY_EXCEEDED
            ? "The local review generation is exhausted. Open a new practice window."
            : "Choose an absolute ordinary file path in a directory you control.");
    }
    if (job != NULL) FileJobFree(job);
    panel->busy = FALSE; g_object_unref(window);
}
static void FileAcknowledge(GtkCheckButton *check, gpointer window) {
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy) return;
    g_object_ref(window); panel->busy = TRUE;
    uint64_t generation = panel->generation;
    gboolean active = gtk_check_button_get_active(check) && panel->pendingBytes != NULL &&
        panel->expected != NULL && panel->fileGeneration == generation && !panel->fileBusy;
    panel->fileAcknowledged = active ? generation : 0U;
    gtk_widget_set_sensitive(panel->fileApply, active);
    if (!Live(panel, generation) || panel->pendingBytes == NULL || !gtk_check_button_get_active(check)) {
        panel->fileAcknowledged = 0U;
        gtk_widget_set_sensitive(panel->fileApply, FALSE);
    }
    panel->busy = FALSE; g_object_unref(window);
}
static void FileApply(GtkButton *button, gpointer window) {
    (void)button;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy || panel->fileBusy ||
        panel->pendingBytes == NULL || panel->expected == NULL ||
        panel->fileGeneration != panel->generation || panel->fileAcknowledged != panel->generation ||
        !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->fileAck))) return;
    g_object_ref(window); panel->busy = TRUE;
    UmiStatus status = umi_open_tms_accounting_book_archive_decode_if_unchanged(
        panel->pendingBytes, panel->pendingSize, panel->expected, &panel->book);
    /* Replacement revokes every previously issued transition or file review.
     * Imported revision numbers cannot revive actions from the former book. */
    Invalidate(panel); RevokeDisplay(panel);
    if (!panel->retired) RenderRows(panel);
    if (!panel->retired) gtk_label_set_text(GTK_LABEL(panel->note), status == UMI_STATUS_OK
        ? "Reviewed archive replaced the local book. Imported flags confer no authenticated approval or ledger posting."
        : umi_status_text(status));
    panel->busy = FALSE; g_object_unref(window);
}
UmiStatus UmiGtk4AccountingPracticeFileState(GtkWindow *window, bool *outRunning, bool *outReady) {
    if (window == NULL || outRunning == NULL || outReady == NULL || outRunning == outReady ||
        !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired) return UMI_STATUS_INVALID_STATE;
    *outRunning = panel->fileBusy != FALSE;
    *outReady = panel->pendingBytes != NULL && panel->expected != NULL &&
        panel->fileGeneration == panel->generation;
    return UMI_STATUS_OK;
}
