/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_snippet_storage.inc
 * PURPOSE: Perform explicit named-template I/O on workers and publish only current draft fields.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void SnippetStorageFree(SnippetStorageWork *work)
{
    g_weak_ref_clear(&work->window);
    g_free(work);
}
static void SnippetStorageWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    SnippetStorageWork *work = data;
    work->status = UMI_STATUS_OK;
    if (work->save)
    {
        /* Save is an explicit user action. Only its resolved local settings
         * parent is created; opening or loading a review creates no directory. */
        GFile *file = g_file_new_for_path(work->path), *parent = g_file_get_parent(file);
        GError *error = NULL;
        if (parent == NULL || (!g_file_make_directory_with_parents(parent, NULL, &error) &&
                               !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_EXISTS)))
            work->status = UMI_STATUS_IO_ERROR;
        g_clear_error(&error);
        g_clear_object(&parent);
        g_object_unref(file);
    }
    if (work->status == UMI_STATUS_OK)
        work->status = work->save ? UmiSnippetTemplateSave(work->path, &work->snippet)
                                  : UmiSnippetTemplateLoad(work->path, work->snippet.language_id,
                                                           work->snippet.id, &work->snippet);
    g_task_return_boolean(task, TRUE);
}
static void SnippetStorageDone(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    (void)data;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    SnippetStorageWork *work = g_task_get_task_data(task);
    g_task_set_task_data(task, NULL, NULL);
    GtkWidget *window = g_weak_ref_get(&work->window);
    SnippetPanel *panel = window == NULL ? NULL : SnippetPanelState(window);
    if (panel != NULL && panel->storage_pending == work)
    {
        UmiStatus status = work->stale ? UMI_STATUS_CANCELLED : work->status;
        if (status == UMI_STATUS_OK && !work->save && SnippetPanelOwner(window, panel) != NULL)
        {
            /* Even an identical saved body must be expanded again: values in
             * the old session may differ from the saved template defaults. */
            g_clear_pointer(&panel->template_body, g_free);
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->body));
            /* This single field is a draft publication, never a source edit.
             * Suppress only our own change handler; external observers still
             * run. Re-read the final value so their edits are not mistaken for
             * a successful load. No expansion or insertion starts here. */
            g_signal_handler_block(buffer, panel->body_changed_handler);
            gtk_text_buffer_set_text(buffer, work->snippet.body, -1);
            g_signal_handler_unblock(buffer, panel->body_changed_handler);
            gchar *actual = SnippetPanelText(panel->body);
            if (strcmp(actual, work->snippet.body) != 0 || work->stale)
                status = UMI_STATUS_CANCELLED;
            g_free(actual);
        }
        panel->storage_pending = NULL;
        g_object_set_data(G_OBJECT(window), "umicom-snippet-storage-pending", NULL);
        if (SnippetPanelOwner(window, panel) != NULL)
        {
            gchar *message;
            if (work->save && work->status == UMI_STATUS_OK)
                message =
                    g_strdup_printf("Saved the captured template to %s.%s", work->path,
                                    work->stale ? " The form has newer edits; save again to keep them." : "");
            else if (status == UMI_STATUS_OK)
                message = g_strdup("Named template loaded into this form. Choose Expand template, edit its "
                                   "values and review the complete insertion.");
            else
                message = g_strdup_printf("Template storage: %s. Location: %s. The document was not changed.",
                                          umi_status_text(status), work->path);
            gtk_label_set_text(GTK_LABEL(panel->message), message);
            g_free(message);
        }
        if (SnippetPanelOwner(window, panel) != NULL)
            gtk_widget_set_sensitive(panel->save_template, TRUE);
        if (SnippetPanelOwner(window, panel) != NULL)
            gtk_widget_set_sensitive(panel->load_template, TRUE);
        panel->busy = 0;
    }
    g_clear_object(&window);
    SnippetStorageFree(work);
}
static void SnippetStorageNameChanged(GtkEditable *editable, gpointer data)
{
    (void)editable;
    GtkWidget *window = g_object_ref(data);
    SnippetPanel *panel = SnippetPanelState(window);
    if (SnippetPanelOwner(window, panel) != NULL)
    {
        if (panel->storage_pending != NULL)
            panel->storage_pending->stale = 1;
        SnippetPanelExpire(panel);
    }
    g_object_unref(window);
}
static void SnippetStorageStart(GtkWidget *window, int saving)
{
    g_object_ref(window);
    SnippetPanel *panel = SnippetPanelState(window);
    if (SnippetPanelOwner(window, panel) == NULL || panel->busy || panel->storage_pending != NULL ||
        panel->application_directory == NULL)
    {
        g_object_unref(window);
        return;
    }
    panel->busy = 1;
    SnippetPanelExpire(panel);
    SnippetStorageWork *work = g_try_new0(SnippetStorageWork, 1);
    UmiStatus status = work == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    if (work != NULL)
    {
        g_weak_ref_init(&work->window, window);
        work->save = saving;
        work->snippet.struct_size = (uint32_t)sizeof(work->snippet);
        work->snippet.api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
        status = umi_text_copy(work->snippet.language_id, sizeof(work->snippet.language_id),
                               panel->storage_language);
        if (status == UMI_STATUS_OK)
            status = umi_text_copy(work->snippet.id, sizeof(work->snippet.id),
                                   gtk_editable_get_text(GTK_EDITABLE(panel->template_name)));
        if (status == UMI_STATUS_OK)
            status = umi_text_copy(work->snippet.name, sizeof(work->snippet.name), work->snippet.id);
        gchar *body = SnippetPanelText(panel->body);
        if (status == UMI_STATUS_OK && saving)
            status = umi_text_copy(work->snippet.body, sizeof(work->snippet.body), body);
        g_free(body);
        if (status == UMI_STATUS_OK)
            status = UmiSnippetTemplateValidate(&work->snippet);
        if (status == UMI_STATUS_OK)
            status = UmiSnippetTemplatePath(panel->application_directory, panel->base_override,
                                            work->snippet.language_id, work->snippet.id, work->path,
                                            sizeof(work->path));
    }
    if (status == UMI_STATUS_OK && SnippetPanelOwner(window, panel) != NULL)
    {
        panel->storage_pending = work;
        g_object_set_data(G_OBJECT(window), "umicom-snippet-storage-pending", work);
        gtk_widget_set_sensitive(panel->save_template, FALSE);
        if (SnippetPanelOwner(window, panel) != NULL)
            gtk_widget_set_sensitive(panel->load_template, FALSE);
        if (SnippetPanelOwner(window, panel) != NULL && !work->stale)
        {
            /* The task owns copied inputs only. It never dereferences a widget,
             * document coordinator or Studio runtime from the worker thread. */
            GTask *task = g_task_new(NULL, NULL, SnippetStorageDone, NULL);
            g_task_set_task_data(task, work, NULL);
            g_task_run_in_thread(task, SnippetStorageWorker);
            g_object_unref(task);
            work = NULL;
        }
        else
        {
            panel->storage_pending = NULL;
            status = UMI_STATUS_CANCELLED;
            g_object_set_data(G_OBJECT(window), "umicom-snippet-storage-pending", NULL);
        }
    }
    else
    {
        if (status == UMI_STATUS_OK)
            status = UMI_STATUS_CANCELLED;
    }
    if (work != NULL)
        SnippetStorageFree(work);
    if (status != UMI_STATUS_OK)
    {
        SnippetPanelStatus(window, panel, status, "");
        if (SnippetPanelOwner(window, panel) != NULL)
            gtk_widget_set_sensitive(panel->save_template, TRUE);
        if (SnippetPanelOwner(window, panel) != NULL)
            gtk_widget_set_sensitive(panel->load_template, TRUE);
        panel->busy = 0;
    }
    g_object_unref(window);
}
static void SnippetStorageSave(GtkButton *button, gpointer data)
{
    (void)button;
    SnippetStorageStart(GTK_WIDGET(data), 1);
}
static void SnippetStorageLoad(GtkButton *button, gpointer data)
{
    (void)button;
    SnippetStorageStart(GTK_WIDGET(data), 0);
}
