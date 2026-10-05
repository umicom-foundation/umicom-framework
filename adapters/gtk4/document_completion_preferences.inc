/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_completion_preferences.inc
 * PURPOSE: Keep local server settings I/O off the UI thread and separate from process launch.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void CompletionPreferenceFree(CompletionPreferenceWork *work)
{
    g_weak_ref_clear(&work->window);
    g_free(work);
}
static void CompletionPreferenceWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    CompletionPreferenceWork *work = data;
    work->status = UMI_STATUS_OK;
    if (work->save)
    {
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
        work->status = work->save ? UmiLanguageServerPreferencesSave(work->path, &work->preferences)
                                  : UmiLanguageServerPreferencesLoad(
                                        work->path, work->preferences.language_id, &work->preferences);
    g_task_return_boolean(task, TRUE);
}
/* Keep the worker attached during publication. Draft-change callbacks can then
 * mark intervening user edits stale instead of letting a load overwrite them. */
static void CompletionPreferenceDone(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    (void)data;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    CompletionPreferenceWork *work = g_task_get_task_data(task);
    g_task_set_task_data(task, NULL, NULL);
    GtkWidget *window = g_weak_ref_get(&work->window);
    CompletionReview *review = window == NULL ? NULL : CompletionReviewState(window);
    if (review != NULL && review->preference_pending == work)
    {
        review->rendering = 1;
        UmiStatus status = work->stale ? UMI_STATUS_CANCELLED : work->status;
        if (CompletionReviewOwner(window, review) != NULL && status == UMI_STATUS_OK && !work->save)
        {
            review->preference_write = review->program;
            review->preference_value = work->preferences.executable;
            gtk_editable_set_text(GTK_EDITABLE(review->program), work->preferences.executable);
            if (CompletionReviewOwner(window, review) != NULL && !work->stale)
            {
                review->preference_write = review->arguments;
                review->preference_value = work->preferences.arguments;
                gtk_editable_set_text(GTK_EDITABLE(review->arguments), work->preferences.arguments);
            }
            review->preference_write = NULL;
            review->preference_value = NULL;
            if (work->stale)
                status = UMI_STATUS_CANCELLED;
        }
        review->preference_pending = NULL;
        g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", NULL);
        if (CompletionReviewOwner(window, review) != NULL)
        {
            char *message;
            if (work->save && work->status == UMI_STATUS_OK)
                message = g_strdup_printf(
                    "Saved the selected server to %s.%s", work->path,
                    work->stale ? " The form has newer edits; save again to keep those edits." : "");
            else if (status == UMI_STATUS_OK)
                message = g_strdup("Saved server loaded into this form. Review the path and arguments, then "
                                   "use the request button to start the selected server.");
            else
                message =
                    g_strdup_printf("Server settings: %s. Location: %s. Review the fields before continuing.",
                                    umi_status_text(status), work->path);
            gtk_label_set_text(GTK_LABEL(review->message), message);
            g_free(message);
        }
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->save_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, TRUE);
        review->rendering = 0;
    }
    g_clear_object(&window);
    CompletionPreferenceFree(work);
}
/* Loading or saving server preferences retires rename notes with their source proposal; saved preferences never include a replacement name.
 * The former implementation is retained for engineering review. */
#if 0
static void CompletionPreferenceStart(GtkWidget *window, int saving)
{
    g_object_ref(window);
    CompletionReview *review = CompletionReviewState(window);
    if (CompletionReviewOwner(window, review) == NULL || review->rendering || review->pending != NULL ||
        review->preference_pending != NULL || review->settings_path[0] == '\0')
    {
        g_object_unref(window);
        return;
    }
    review->rendering = 1;
    CompletionPreferenceWork *work = g_try_new0(CompletionPreferenceWork, 1);
    UmiStatus status = work == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    UmiDocumentSourceRequestSummary summary;
    if (work != NULL)
    {
        g_weak_ref_init(&work->window, window);
        work->save = saving;
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestInspect(review->source, &summary);
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(work->preferences.language_id, sizeof(work->preferences.language_id),
                               summary.language_id);
    if (status == UMI_STATUS_OK && saving)
        status = umi_text_copy(work->preferences.executable, sizeof(work->preferences.executable),
                               gtk_editable_get_text(GTK_EDITABLE(review->program)));
    if (status == UMI_STATUS_OK && saving)
        status = umi_text_copy(work->preferences.arguments, sizeof(work->preferences.arguments),
                               gtk_editable_get_text(GTK_EDITABLE(review->arguments)));
    if (status == UMI_STATUS_OK && saving)
        status = UmiLanguageServerPreferencesValidate(&work->preferences);
    if (status == UMI_STATUS_OK)
    {
        memcpy(work->path, review->settings_path, sizeof(work->path));
        review->preference_pending = work;
        CompletionReviewInvalidate(review);
        UmiLanguageCompletionCatalogueDestroy(review->catalogue);
        review->catalogue = NULL;
        UmiLanguageTextEditPreviewDestroy(review->formatted);
        UmiLanguageLocationCatalogueDestroy(review->locations);
        UmiLanguageSymbolCatalogueDestroy(review->symbols);
        UmiLanguageSignatureCatalogueDestroy(review->signatures);
        review->signatures = NULL;
        review->symbols = NULL;
        review->locations = NULL;
        review->formatted = NULL;
        g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", GINT_TO_POINTER(1));
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->preview, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->save_settings, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_label_set_text(GTK_LABEL(review->message),
                               "Working with local server settings. No server is being started.");
        if (CompletionReviewOwner(window, review) != NULL && !work->stale)
        {
            GTask *task = g_task_new(NULL, NULL, CompletionPreferenceDone, NULL);
            g_task_set_task_data(task, work, NULL);
            g_task_run_in_thread(task, CompletionPreferenceWorker);
            g_object_unref(task);
            work = NULL;
        }
        else
        {
            review->preference_pending = NULL;
            g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", NULL);
        }
    }
    else if (CompletionReviewOwner(window, review) != NULL)
        gtk_label_set_text(
            GTK_LABEL(review->message),
            "Settings were not saved. Enter a valid absolute server path and valid argument text.");
    if (work != NULL)
        CompletionPreferenceFree(work);
    if (review->preference_pending == NULL && CompletionReviewOwner(window, review) != NULL)
    {
        gtk_widget_set_sensitive(review->save_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, TRUE);
    }
    review->rendering = 0;
    g_object_unref(window);
}
#endif
/* Retire all code-action choices when server preferences change; a loaded setting cannot inherit an earlier approval.
 * The former implementation is retained for engineering review. */
#if 0
static void CompletionPreferenceStart(GtkWidget *window, int saving)
{
    g_object_ref(window);
    CompletionReview *review = CompletionReviewState(window);
    if (CompletionReviewOwner(window, review) == NULL || review->rendering || review->pending != NULL ||
        review->preference_pending != NULL || review->settings_path[0] == '\0')
    {
        g_object_unref(window);
        return;
    }
    review->rendering = 1;
    CompletionPreferenceWork *work = g_try_new0(CompletionPreferenceWork, 1);
    UmiStatus status = work == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    UmiDocumentSourceRequestSummary summary;
    if (work != NULL)
    {
        g_weak_ref_init(&work->window, window);
        work->save = saving;
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestInspect(review->source, &summary);
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(work->preferences.language_id, sizeof(work->preferences.language_id),
                               summary.language_id);
    if (status == UMI_STATUS_OK && saving)
        status = umi_text_copy(work->preferences.executable, sizeof(work->preferences.executable),
                               gtk_editable_get_text(GTK_EDITABLE(review->program)));
    if (status == UMI_STATUS_OK && saving)
        status = umi_text_copy(work->preferences.arguments, sizeof(work->preferences.arguments),
                               gtk_editable_get_text(GTK_EDITABLE(review->arguments)));
    if (status == UMI_STATUS_OK && saving)
        status = UmiLanguageServerPreferencesValidate(&work->preferences);
    if (status == UMI_STATUS_OK)
    {
        memcpy(work->path, review->settings_path, sizeof(work->path));
        review->preference_pending = work;
        CompletionReviewInvalidate(review);
        UmiLanguageCompletionCatalogueDestroy(review->catalogue);
        review->catalogue = NULL;
        UmiLanguageTextEditPreviewDestroy(review->formatted);
        UmiLanguageLocationCatalogueDestroy(review->locations);
        UmiLanguageSymbolCatalogueDestroy(review->symbols);
        UmiLanguageSignatureCatalogueDestroy(review->signatures);
        g_clear_pointer(&review->rename_details, g_free);
        review->signatures = NULL;
        review->symbols = NULL;
        review->locations = NULL;
        review->formatted = NULL;
        g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", GINT_TO_POINTER(1));
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->preview, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->save_settings, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_label_set_text(GTK_LABEL(review->message),
                               "Working with local server settings. No server is being started.");
        if (CompletionReviewOwner(window, review) != NULL && !work->stale)
        {
            GTask *task = g_task_new(NULL, NULL, CompletionPreferenceDone, NULL);
            g_task_set_task_data(task, work, NULL);
            g_task_run_in_thread(task, CompletionPreferenceWorker);
            g_object_unref(task);
            work = NULL;
        }
        else
        {
            review->preference_pending = NULL;
            g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", NULL);
        }
    }
    else if (CompletionReviewOwner(window, review) != NULL)
        gtk_label_set_text(
            GTK_LABEL(review->message),
            "Settings were not saved. Enter a valid absolute server path and valid argument text.");
    if (work != NULL)
        CompletionPreferenceFree(work);
    if (review->preference_pending == NULL && CompletionReviewOwner(window, review) != NULL)
    {
        gtk_widget_set_sensitive(review->save_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, TRUE);
    }
    review->rendering = 0;
    g_object_unref(window);
}
#endif
/* Changing server preferences retires diagnostic publications from the former session.
 * The former implementation is retained for engineering review. */
#if 0
static void CompletionPreferenceStart(GtkWidget *window, int saving)
{
    g_object_ref(window);
    CompletionReview *review = CompletionReviewState(window);
    if (CompletionReviewOwner(window, review) == NULL || review->rendering || review->pending != NULL ||
        review->preference_pending != NULL || review->settings_path[0] == '\0')
    {
        g_object_unref(window);
        return;
    }
    review->rendering = 1;
    CompletionPreferenceWork *work = g_try_new0(CompletionPreferenceWork, 1);
    UmiStatus status = work == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    UmiDocumentSourceRequestSummary summary;
    if (work != NULL)
    {
        g_weak_ref_init(&work->window, window);
        work->save = saving;
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestInspect(review->source, &summary);
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(work->preferences.language_id, sizeof(work->preferences.language_id),
                               summary.language_id);
    if (status == UMI_STATUS_OK && saving)
        status = umi_text_copy(work->preferences.executable, sizeof(work->preferences.executable),
                               gtk_editable_get_text(GTK_EDITABLE(review->program)));
    if (status == UMI_STATUS_OK && saving)
        status = umi_text_copy(work->preferences.arguments, sizeof(work->preferences.arguments),
                               gtk_editable_get_text(GTK_EDITABLE(review->arguments)));
    if (status == UMI_STATUS_OK && saving)
        status = UmiLanguageServerPreferencesValidate(&work->preferences);
    if (status == UMI_STATUS_OK)
    {
        memcpy(work->path, review->settings_path, sizeof(work->path));
        review->preference_pending = work;
        CompletionReviewInvalidate(review);
        UmiLanguageCompletionCatalogueDestroy(review->catalogue);
        review->catalogue = NULL;
        UmiLanguageTextEditPreviewDestroy(review->formatted);
        UmiLanguageLocationCatalogueDestroy(review->locations);
        UmiLanguageSymbolCatalogueDestroy(review->symbols);
        UmiLanguageSignatureCatalogueDestroy(review->signatures);
        UmiLanguageCodeActionCatalogueDestroy(review->actions);
        review->actions = NULL;
        g_clear_pointer(&review->rename_details, g_free);
        review->signatures = NULL;
        review->symbols = NULL;
        review->locations = NULL;
        review->formatted = NULL;
        g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", GINT_TO_POINTER(1));
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->preview, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->save_settings, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_label_set_text(GTK_LABEL(review->message),
                               "Working with local server settings. No server is being started.");
        if (CompletionReviewOwner(window, review) != NULL && !work->stale)
        {
            GTask *task = g_task_new(NULL, NULL, CompletionPreferenceDone, NULL);
            g_task_set_task_data(task, work, NULL);
            g_task_run_in_thread(task, CompletionPreferenceWorker);
            g_object_unref(task);
            work = NULL;
        }
        else
        {
            review->preference_pending = NULL;
            g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", NULL);
        }
    }
    else if (CompletionReviewOwner(window, review) != NULL)
        gtk_label_set_text(
            GTK_LABEL(review->message),
            "Settings were not saved. Enter a valid absolute server path and valid argument text.");
    if (work != NULL)
        CompletionPreferenceFree(work);
    if (review->preference_pending == NULL && CompletionReviewOwner(window, review) != NULL)
    {
        gtk_widget_set_sensitive(review->save_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, TRUE);
    }
    review->rendering = 0;
    g_object_unref(window);
}
#endif
static void CompletionPreferenceStart(GtkWidget *window, int saving)
{
    g_object_ref(window);
    CompletionReview *review = CompletionReviewState(window);
    if (CompletionReviewOwner(window, review) == NULL || review->rendering || review->pending != NULL ||
        review->preference_pending != NULL || review->settings_path[0] == '\0')
    {
        g_object_unref(window);
        return;
    }
    review->rendering = 1;
    CompletionPreferenceWork *work = g_try_new0(CompletionPreferenceWork, 1);
    UmiStatus status = work == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    UmiDocumentSourceRequestSummary summary;
    if (work != NULL)
    {
        g_weak_ref_init(&work->window, window);
        work->save = saving;
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestInspect(review->source, &summary);
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(work->preferences.language_id, sizeof(work->preferences.language_id),
                               summary.language_id);
    if (status == UMI_STATUS_OK && saving)
        status = umi_text_copy(work->preferences.executable, sizeof(work->preferences.executable),
                               gtk_editable_get_text(GTK_EDITABLE(review->program)));
    if (status == UMI_STATUS_OK && saving)
        status = umi_text_copy(work->preferences.arguments, sizeof(work->preferences.arguments),
                               gtk_editable_get_text(GTK_EDITABLE(review->arguments)));
    if (status == UMI_STATUS_OK && saving)
        status = UmiLanguageServerPreferencesValidate(&work->preferences);
    if (status == UMI_STATUS_OK)
    {
        memcpy(work->path, review->settings_path, sizeof(work->path));
        review->preference_pending = work;
        CompletionReviewInvalidate(review);
        UmiLanguageCompletionCatalogueDestroy(review->catalogue);
        review->catalogue = NULL;
        UmiLanguageTextEditPreviewDestroy(review->formatted);
        UmiLanguageLocationCatalogueDestroy(review->locations);
        UmiLanguageSymbolCatalogueDestroy(review->symbols);
        UmiLanguageSignatureCatalogueDestroy(review->signatures);
        UmiLanguageCodeActionCatalogueDestroy(review->actions);
        UmiLanguageDiagnosticCatalogueDestroy(review->diagnostics);
        review->diagnostics = NULL;
        review->actions = NULL;
        g_clear_pointer(&review->rename_details, g_free);
        review->signatures = NULL;
        review->symbols = NULL;
        review->locations = NULL;
        review->formatted = NULL;
        g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", GINT_TO_POINTER(1));
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->preview, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->save_settings, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, FALSE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_label_set_text(GTK_LABEL(review->message),
                               "Working with local server settings. No server is being started.");
        if (CompletionReviewOwner(window, review) != NULL && !work->stale)
        {
            GTask *task = g_task_new(NULL, NULL, CompletionPreferenceDone, NULL);
            g_task_set_task_data(task, work, NULL);
            g_task_run_in_thread(task, CompletionPreferenceWorker);
            g_object_unref(task);
            work = NULL;
        }
        else
        {
            review->preference_pending = NULL;
            g_object_set_data(G_OBJECT(window), "umicom-completion-settings-pending", NULL);
        }
    }
    else if (CompletionReviewOwner(window, review) != NULL)
        gtk_label_set_text(
            GTK_LABEL(review->message),
            "Settings were not saved. Enter a valid absolute server path and valid argument text.");
    if (work != NULL)
        CompletionPreferenceFree(work);
    if (review->preference_pending == NULL && CompletionReviewOwner(window, review) != NULL)
    {
        gtk_widget_set_sensitive(review->save_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->load_settings, TRUE);
        if (CompletionReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->request, TRUE);
    }
    review->rendering = 0;
    g_object_unref(window);
}
static void CompletionPreferenceSave(GtkButton *button, gpointer data)
{
    (void)button;
    CompletionPreferenceStart(GTK_WIDGET(data), 1);
}
static void CompletionPreferenceLoad(GtkButton *button, gpointer data)
{
    (void)button;
    CompletionPreferenceStart(GTK_WIDGET(data), 0);
}
