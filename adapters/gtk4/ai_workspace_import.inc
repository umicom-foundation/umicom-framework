/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ai_workspace_import.inc
 * PURPOSE: Review local text imports without lending workspace ownership to file-selection or I/O callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/import.h"
#include "umicom/platform/input_file.h"
#include "umicom/ui/gtk4/interaction_recording.h"
typedef struct AiImportPanel
{
    GtkWidget *root, *input, *preview, *apply;
    GtkEntry *collection, *collectionTitle, *document, *title;
    GtkCheckButton *approve;
    GtkLabel *message;
    GWeakRef host;
/* A file choice and a file read have separate cancellation lifetimes.
 * GtkFileDialog completion retains only this section root, never its host
 * workspace; late completions still pass the existing weak-owner check.
 * The superseded implementation is retained below for engineering review. */
#if 0
    GtkFileChooserNative *chooser;
#endif
    /* Keep chooser cancellation separate from the later file worker's cancellation. */
    GtkFileDialog *chooser;
    GCancellable *chooserCancel;
    GCancellable *readCancel;
    UmiAiWorkspaceImport *plan;
    uint64_t generation;
    bool closed, loading, rendering;
} AiImportPanel;
typedef struct AiImportRead
{
    char *path;
    unsigned char *bytes;
    size_t size;
    UmiStatus status;
    uint64_t generation;
} AiImportRead;
static UmiAiWorkspaceGtkPanel *AiImportOwner(AiImportPanel *import)
{
    if (import->closed)
        return NULL;
    GObject *root = g_weak_ref_get(&import->host);
    UmiAiWorkspaceGtkPanel *panel =
        root != NULL ? g_object_get_data(root, "umicom-ai-workspace-owner") : NULL;
    g_clear_object(&root);
    return Available(panel) ? panel : NULL;
}
static void AiImportReadFree(gpointer data)
{
    AiImportRead *read = data;
    g_free(read->path);
    UmiInputFileFree(read->bytes);
    g_free(read);
}
static void AiImportFree(gpointer data)
{
    AiImportPanel *import = data;
    UmiAiWorkspaceImportDestroy(import->plan);
    g_clear_object(&import->readCancel);
/* Dispose the asynchronous chooser through cancellation rather than the
 * deprecated native-dialog destroy path. Its completion owns the root until
 * result cleanup, so no callback borrows freed import state.
 * The superseded implementation is retained below for engineering review. */
#if 0
    if (import->chooser != NULL)
        gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(import->chooser));
#endif
    /* Cancel only the chooser request; asynchronous completion retains its own root reference. */
    if (import->chooserCancel != NULL)
        g_cancellable_cancel(import->chooserCancel);
    g_clear_object(&import->chooserCancel);
    g_clear_object(&import->chooser);
    g_weak_ref_clear(&import->host);
    g_free(import);
}
/* Retire before the host releases its borrowed services. File workers own only
 * a path and bytes; a late result can never access the retired workspace. */
static void AiImportRetire(GtkWidget *root)
{
    if (root == NULL)
        return;
    g_object_ref(root);
    AiImportPanel *import = g_object_get_data(G_OBJECT(root), "umicom-ai-import");
    if (!import->closed)
    {
        import->closed = true;
        if (import->readCancel != NULL)
            g_cancellable_cancel(import->readCancel);
        UmiAiWorkspaceImportDestroy(import->plan);
        import->plan = NULL;
/* Retire before cancelling so a late selection cannot start file I/O
 * or access the released workspace. The pending completion owns the section
 * root and must still consume its finish result and release that reference.
 * The superseded implementation is retained below for engineering review. */
#if 0
        GtkFileChooserNative *chooser = import->chooser;
        import->chooser = NULL;
        if (chooser != NULL)
        {
            gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(chooser));
            g_object_unref(chooser);
        }
#endif
        /* The closed flag is already set, so cancellation cannot authorise a new read. */
        if (import->chooserCancel != NULL)
            g_cancellable_cancel(import->chooserCancel);
        g_clear_object(&import->chooser);
        g_clear_object(&import->chooserCancel);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(import->input)), "", 0);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(import->preview)), "", 0);
        gtk_widget_set_sensitive(root, FALSE);
    }
    g_object_unref(root);
}
static void AiImportInvalidate(AiImportPanel *import)
{
    if (import->generation != UINT64_MAX)
        ++import->generation;
    UmiAiWorkspaceImportDestroy(import->plan);
    import->plan = NULL;
    gtk_check_button_set_active(import->approve, FALSE);
    gtk_widget_set_sensitive(import->apply, FALSE);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(import->preview)), "", 0);
}
static void AiImportChanged(GObject *object, gpointer data)
{
    (void)object;
    GtkWidget *root = g_object_ref(data);
    AiImportPanel *import = g_object_get_data(G_OBJECT(root), "umicom-ai-import");
    if (!import->closed)
    {
        AiImportInvalidate(import);
        gtk_label_set_text(import->message, "Input changed. Preview every passage again before saving.");
    }
    g_object_unref(root);
}
static void AiImportPrepare(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiImportPanel *import = g_object_get_data(G_OBJECT(root), "umicom-ai-import");
    if (AiImportOwner(import) == NULL || import->loading || import->rendering)
    {
        g_object_unref(root);
        return;
    }
    import->rendering = true;
    AiImportInvalidate(import);
    UmiAiWorkspaceGtkPanel *host = AiImportOwner(import);
    uint64_t generation = import->generation;
    UmiStatus status = host == NULL               ? UMI_STATUS_CANCELLED
                       : generation == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED
                                                  : UMI_STATUS_OK;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(import->input));
    if (status == UMI_STATUS_OK &&
        (size_t)gtk_text_buffer_get_char_count(buffer) > UMI_AI_WORKSPACE_IMPORT_MAX_BYTES)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
        status = UmiAiWorkspaceImportCreate(host->workspace, Value(import->collection),
                                            Value(import->collectionTitle), Value(import->document),
                                            Value(import->title), text, strlen(text), &import->plan);
        g_free(text);
    }
    GString *preview = g_string_new(NULL);
    if (status == UMI_STATUS_OK)
    {
        UmiAiWorkspaceImportSummary summary;
        (void)UmiAiWorkspaceImportInspect(import->plan, &summary);
        g_string_append_printf(preview,
                               "%s / %s\n%zu input bytes; %zu normalized bytes; %zu passages.\n"
                               "UTF-8 BOM removed, CRLF and lone CR normalized to LF in these copies. The "
                               "original file is unchanged.\n\n",
                               summary.collectionId, summary.title, summary.inputBytes, summary.textBytes,
                               summary.passageCount);
        for (size_t i = 0U; i < summary.passageCount; ++i)
        {
            UmiAiWorkspaceImportPassage passage;
            (void)UmiAiWorkspaceImportPassageAt(import->plan, i, &passage);
            g_string_append_printf(preview, "%s | lines %u–%u | normalized byte %zu, %zu bytes\n%s\n\n",
                                   passage.source.id, passage.source.firstLine, passage.source.lastLine,
                                   passage.textOffset, passage.byteCount, passage.source.text);
        }
    }
    /* All rendering bytes are owned before GTK can notify a host teardown or
     * another edit. Such an edit revokes the plan and cannot enable Save. */
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(import->preview)), preview->str, -1);
    if (!import->closed && generation == import->generation && AiImportOwner(import) != NULL)
    {
        gtk_widget_set_sensitive(import->apply, status == UMI_STATUS_OK && import->plan != NULL);
        gtk_label_set_text(import->message, status == UMI_STATUS_OK
                                                ? "Review all passages and their destination, then approve "
                                                  "and save together. No model is contacted."
                                                : Explain(status));
    }
    g_string_free(preview, TRUE);
    import->rendering = false;
    g_object_unref(root);
}
static void AiImportApply(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiImportPanel *import = g_object_get_data(G_OBJECT(root), "umicom-ai-import");
    UmiAiWorkspaceGtkPanel *host = AiImportOwner(import);
    if (host != NULL && !import->loading && !import->rendering)
    {
        UmiStatus status = import->plan == NULL
                               ? UMI_STATUS_INVALID_STATE
                               : UmiAiWorkspaceImportApply(host->workspace, import->plan,
                                                           gtk_check_button_get_active(import->approve));
        if (status == UMI_STATUS_OK)
        {
            UmiAiWorkspaceImportDestroy(import->plan);
            import->plan = NULL;
            gtk_check_button_set_active(import->approve, FALSE);
            gtk_widget_set_sensitive(import->apply, FALSE);
            gtk_label_set_text(
                import->message,
                "Saved together. On Sources, enter this collection ID and search. Reload shared workspace "
                "refreshes the complete catalogue. Existing job evidence is retained.");
        }
        else
        {
            AiImportInvalidate(import);
            gtk_label_set_text(import->message, Explain(status));
        }
    }
    g_object_unref(root);
}
static void AiImportReadWorker(GTask *task, gpointer source, gpointer taskData, GCancellable *cancellation)
{
    (void)source;
    AiImportRead *read = taskData;
    read->status =
        g_cancellable_is_cancelled(cancellation)
            ? UMI_STATUS_CANCELLED
            : UmiInputFileRead(read->path, UMI_AI_WORKSPACE_IMPORT_MAX_BYTES, &read->bytes, &read->size);
    if (g_cancellable_is_cancelled(cancellation))
        read->status = UMI_STATUS_CANCELLED;
    g_task_return_boolean(task, TRUE);
}
static void AiImportReadDone(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)data;
    AiImportPanel *import = g_object_get_data(source, "umicom-ai-import");
    GError *error = NULL;
    gboolean done = g_task_propagate_boolean(G_TASK(result), &error);
    AiImportRead *read = g_task_get_task_data(G_TASK(result));
    import->loading = false;
    g_clear_object(&import->readCancel);
    if (!import->closed && done && import->generation == read->generation)
    {
        UmiStatus status = read->status;
        if (status == UMI_STATUS_OK &&
            (read->size == 0U || memchr(read->bytes, 0, read->size) != NULL ||
             !g_utf8_validate((const char *)read->bytes, (gssize)read->size, NULL)))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
        {
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(import->input)),
                                     (const char *)read->bytes, (gint)read->size);
            if (!import->closed)
                gtk_label_set_text(import->message,
                                   "Local text loaded. Check the collection, document ID and title, then "
                                   "preview. Nothing has been saved or sent.");
        }
        else
            gtk_label_set_text(import->message, Explain(status));
    }
    else if (!import->closed)
        gtk_label_set_text(import->message, "The input changed or loading was cancelled. The file result was "
                                            "discarded; your current text is retained.");
    g_clear_error(&error);
}
/* File loading is separate from import preparation: the worker owns only
 * its absolute path and bytes, while the GTK token decides whether to show them. */
static UmiStatus AiImportReadStart(AiImportPanel *import, const char *path)
{
    if (AiImportOwner(import) == NULL)
        return UMI_STATUS_UNAVAILABLE;
    if (import->loading || import->rendering)
        return UMI_STATUS_BUSY;
    AiImportInvalidate(import);
    if (import->closed)
        return UMI_STATUS_CANCELLED;
    if (import->generation == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    import->loading = true;
    import->readCancel = g_cancellable_new();
    AiImportRead *read = g_new0(AiImportRead, 1);
    read->path = g_strdup(path);
    read->generation = import->generation;
    GTask *task = g_task_new(import->root, import->readCancel, AiImportReadDone, NULL);
    g_task_set_task_data(task, read, AiImportReadFree);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_run_in_thread(task, AiImportReadWorker);
    g_object_unref(task);
    gtk_label_set_text(import->message, "Reading the selected local file. Editing the form discards the "
                                        "pending result; the database is unchanged.");
    return UMI_STATUS_OK;
}
/* The supported GTK 4.10 async/finish API replaces deprecated chooser
 * signals. Keep native-file-only input, original bounded worker reads, weak
 * host ownership, generation checks, preview and explicit save approval.
 * Cancelling a dialog or closing a host never starts an import.
 * The superseded implementation is retained below for engineering review. */
#if 0
static void AiImportFileSelected(GtkNativeDialog *dialog, int response, gpointer data)
{
    GtkWidget *root = g_object_ref(data);
    AiImportPanel *import = g_object_get_data(G_OBJECT(root), "umicom-ai-import");
    if (import->chooser != GTK_FILE_CHOOSER_NATIVE(dialog))
    {
        g_object_unref(root);
        return;
    }
    GFile *file =
        response == GTK_RESPONSE_ACCEPT ? gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog)) : NULL;
    if (import->chooser == GTK_FILE_CHOOSER_NATIVE(dialog))
    {
        import->chooser = NULL;
        gtk_native_dialog_destroy(dialog);
        g_object_unref(dialog);
    }
    if (file != NULL && AiImportOwner(import) != NULL && !import->loading)
    {
        char *path = g_file_is_native(file) ? g_file_get_path(file) : NULL;
        if (path == NULL)
            gtk_label_set_text(import->message,
                               "Choose an ordinary local text file. Remote URIs cannot be imported here.");
        else
        {
            UmiStatus status = AiImportReadStart(import, path);
            if (status != UMI_STATUS_OK && !import->closed)
                gtk_label_set_text(import->message, Explain(status));
            g_free(path);
        }
    }
    g_clear_object(&file);
    g_object_unref(root);
}
static void AiImportOpenFile(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiImportPanel *import = g_object_get_data(G_OBJECT(root), "umicom-ai-import");
    if (AiImportOwner(import) != NULL && !import->loading && import->chooser == NULL)
    {
        GtkRoot *host = gtk_widget_get_root(root);
        GtkFileChooserNative *chooser = gtk_file_chooser_native_new(
            "Choose a local UTF-8 text file", GTK_IS_WINDOW(host) ? GTK_WINDOW(host) : NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN, "Load text", "Cancel");
        if (!import->closed)
        {
            import->chooser = chooser;
            g_signal_connect_object(chooser, "response", G_CALLBACK(AiImportFileSelected), root, 0);
            gtk_native_dialog_show(GTK_NATIVE_DIALOG(chooser));
        }
        else
        {
            gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(chooser));
            g_object_unref(chooser);
        }
    }
    g_object_unref(root);
}
#endif
/* Always finish and release the selected file; a retired host receives no new file read. */
static void AiImportFileSelected(GObject *source, GAsyncResult *result, gpointer data)
{
    /* This is the launch-held root reference, not borrowed callback data. */
    GtkWidget *root = data;
    AiImportPanel *import = g_object_get_data(G_OBJECT(root), "umicom-ai-import");
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    const bool current = import->chooser == GTK_FILE_DIALOG(source);
    if (current)
    {
        g_clear_object(&import->chooser);
        g_clear_object(&import->chooserCancel);
    }
    if (current && !import->closed && AiImportOwner(import) != NULL && !import->loading)
    {
        if (file != NULL)
        {
            char *path = g_file_is_native(file) ? g_file_get_path(file) : NULL;
            if (path == NULL)
                gtk_label_set_text(import->message,
                                   "Choose an ordinary local text file. Remote URIs cannot be imported here.");
            else
            {
                UmiStatus status = AiImportReadStart(import, path);
                if (status != UMI_STATUS_OK && !import->closed)
                    gtk_label_set_text(import->message, Explain(status));
                g_free(path);
            }
        }
        else if (error != NULL &&
                 !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
                 !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) &&
                 !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
            gtk_label_set_text(import->message, "The local file chooser could not be opened. Current text is unchanged.");
    }
    g_clear_error(&error);
    g_clear_object(&file);
    g_object_unref(root);
}
/* Start one asynchronous selection while retaining the section, not the borrowed workspace. */
static void AiImportOpenFile(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiImportPanel *import = g_object_get_data(G_OBJECT(root), "umicom-ai-import");
    if (AiImportOwner(import) != NULL && !import->loading && import->chooser == NULL)
    {
        GtkRoot *host = gtk_widget_get_root(root);
        GtkFileDialog *chooser = gtk_file_dialog_new();
        gtk_file_dialog_set_title(chooser, "Choose a local UTF-8 text file");
        gtk_file_dialog_set_accept_label(chooser, "Load text");
        if (!import->closed)
        {
            import->chooser = chooser;
            import->chooserCancel = g_cancellable_new();
            /* Exactly one root reference belongs to the pending completion.
             * It keeps UI state alive but grants no ownership of the host. */
            gtk_file_dialog_open(chooser, GTK_IS_WINDOW(host) ? GTK_WINDOW(host) : NULL,
                import->chooserCancel, AiImportFileSelected, g_object_ref(root));
        }
        else
            g_object_unref(chooser);
    }
    g_object_unref(root);
}
static GtkWidget *AiImportCreate(UmiAiWorkspaceGtkPanel *host)
{
    AiImportPanel *import = g_new0(AiImportPanel, 1);
    import->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    g_object_ref_sink(import->root);
    import->generation = 1U;
    g_weak_ref_init(&import->host, host->root);
    g_object_set_data_full(G_OBJECT(import->root), "umicom-ai-import", import, AiImportFree);
    (void)UmiGtk4RecordingSetPrivate(import->root, 1);
    GtkWidget *notice = gtk_label_new("Import local UTF-8 text into searchable passages. PDF, Word and image "
                                      "extraction are not supported here. At most 98,240 input bytes and 64 "
                                      "total saved passages are supported; no tail is silently omitted.");
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_box_append(GTK_BOX(import->root), notice);
    import->collection = Entry(import->root, "Collection ID", "workshop", "ai.import.collection",
                               "Existing collection names must match exactly.");
    import->collectionTitle =
        Entry(import->root, "Collection name", "Community workshop", "ai.import.collection-title",
              "A new collection is created only when the complete import commits.");
    import->document =
        Entry(import->root, "New document ID", "notice", "ai.import.document",
              "Up to 55 identifier bytes. Existing passage IDs are never replaced by an import.");
    import->title = Entry(import->root, "Source title", "Workshop notice", "ai.import.title",
                          "A readable source label for citations; no local path is stored automatically.");
    GtkWidget *open = gtk_button_new_with_label("Load local text file…");
    Tag(open, "ai.import.open");
    gtk_box_append(GTK_BOX(import->root), open);
    GtkTextBuffer *input = TextArea(import->root, "Complete document text", "ai.import.text", 150, true);
    /* The view is the scroll child's widget; keeping the section alive keeps
     * all these controls alive even if the application host is retired. */
    GtkWidget *scroll = gtk_widget_get_last_child(import->root);
    import->input = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scroll));
    GtkWidget *preview = gtk_button_new_with_label("Preview all passages");
    Tag(preview, "ai.import.prepare");
    gtk_box_append(GTK_BOX(import->root), preview);
    (void)TextArea(import->root, "Destination and complete passage preview", "ai.import.preview", 220, false);
    scroll = gtk_widget_get_last_child(import->root);
    import->preview = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scroll));
    import->approve = GTK_CHECK_BUTTON(
        gtk_check_button_new_with_label("I approve saving these passages to this local collection"));
    Tag(GTK_WIDGET(import->approve), "ai.import.approve");
    gtk_box_append(GTK_BOX(import->root), GTK_WIDGET(import->approve));
    import->apply = gtk_button_new_with_label("Save reviewed passages together");
    Tag(import->apply, "ai.import.apply");
    gtk_widget_set_sensitive(import->apply, FALSE);
    gtk_box_append(GTK_BOX(import->root), import->apply);
    import->message =
        GTK_LABEL(gtk_label_new("Choose a local file or paste text, then preview the complete import."));
    gtk_label_set_wrap(import->message, TRUE);
    Tag(GTK_WIDGET(import->message), "ai.import.message");
    gtk_box_append(GTK_BOX(import->root), GTK_WIDGET(import->message));
    GtkEntry *entries[] = {import->collection, import->collectionTitle, import->document, import->title};
    for (size_t i = 0U; i < sizeof(entries) / sizeof(entries[0]); ++i)
        g_signal_connect_object(entries[i], "changed", G_CALLBACK(AiImportChanged), import->root, 0);
    g_signal_connect_object(input, "changed", G_CALLBACK(AiImportChanged), import->root, 0);
    g_signal_connect_object(open, "clicked", G_CALLBACK(AiImportOpenFile), import->root, 0);
    g_signal_connect_object(preview, "clicked", G_CALLBACK(AiImportPrepare), import->root, 0);
    g_signal_connect_object(import->apply, "clicked", G_CALLBACK(AiImportApply), import->root, 0);
    return import->root;
}
