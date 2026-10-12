/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workspace_library_history_gtk4.inc
 * PURPOSE: Remember chosen library locations without opening their databases.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#define LIBRARY_HISTORY_LIMIT 12U
#define LIBRARY_HISTORY_QUEUE_LIMIT 32U
typedef enum LibraryHistoryAction { HISTORY_LOAD, HISTORY_REMEMBER, HISTORY_FORGET, HISTORY_CLEAR } LibraryHistoryAction;
typedef struct LibraryHistoryJob {
    LibraryHistoryAction action;
    char *settingsPath, *libraryPath;
    uint64_t openedAt;
    UmiStatus status;
    UmiRecentItemSnapshot items[LIBRARY_HISTORY_LIMIT];
    size_t count;
} LibraryHistoryJob;

/* Serialize read/modify/write within this process, including separate windows.
 * Other application processes remain last-writer-wins; this is a convenience
 * list, never the authority for opening a database or identifying its owner. */
G_LOCK_DEFINE_STATIC(library_history_disk);
static void HistoryStart(UmiWorkspaceLibraryGtk *library);
static void HistoryJobFree(gpointer data)
{
    LibraryHistoryJob *job = data;
    g_free(job->settingsPath); g_free(job->libraryPath); g_free(job);
}
static UmiStatus HistoryRemember(UmiRecentItemRegistry *registry, const LibraryHistoryJob *job)
{
    UmiRecentItemSnapshot item = {0}, existing = {0};
    if (strlen(job->libraryPath) >= sizeof(item.uri)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_platform_recent_item_id_from_uri("workspace-library", job->libraryPath, item.id, sizeof(item.id));
    if (status != UMI_STATUS_OK) return status;
    status = umi_platform_recent_items_registry_find(registry, item.id, &existing);
    if (status == UMI_STATUS_OK) {
        /* Hashes locate a record; exact path equality prevents a rare collision
         * from borrowing another library's counters or replacing its identity. */
        if (strcmp(existing.uri, job->libraryPath) != 0) return UMI_STATUS_ALREADY_EXISTS;
        return umi_platform_recent_items_registry_touch(registry, item.id, job->openedAt);
    }
    if (status != UMI_STATUS_NOT_FOUND) return status;
    item.struct_size = (uint32_t)sizeof(item); item.api_version = 1U;
    memcpy(item.uri, job->libraryPath, strlen(job->libraryPath) + 1U);
    memcpy(item.label, "Library", sizeof("Library"));
    memcpy(item.kind, "workspace-library", sizeof("workspace-library"));
    item.last_opened = job->openedAt; item.open_count = 1U;
    /* Evict before insertion so a full preferences file remains usable. Pinned
     * entries retain their established meaning and can still exhaust capacity. */
    status = UmiRecentItemsTrimKind(registry, "workspace-library", LIBRARY_HISTORY_LIMIT - 1U);
    if (status == UMI_STATUS_OK) status = umi_platform_recent_items_registry_upsert(registry, &item);
    return status;
}
static UmiStatus HistoryForget(UmiRecentItemRegistry *registry, const char *path)
{
    /* Compare the full path; a selected row need not use an identifier generated
     * by this UI. Forgetting removes metadata only, never the library file. */
    UmiRecentItemSnapshot item;
    for (size_t index = 0U; index < umi_platform_recent_items_registry_count(registry); ++index) {
        UmiStatus status = umi_platform_recent_items_registry_at(registry, index, &item);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(item.kind, "workspace-library") == 0 && strcmp(item.uri, path) == 0)
            return umi_platform_recent_items_registry_remove(registry, item.id);
    }
    return UMI_STATUS_OK;
}
static void HistoryWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    LibraryHistoryJob *job = data;
    UmiRecentItemRegistry *registry = NULL;
    G_LOCK(library_history_disk);
    job->status = UmiRecentItemsLoadLocal(job->settingsPath, &registry, NULL);
    if (job->status == UMI_STATUS_OK && job->action == HISTORY_REMEMBER)
        job->status = HistoryRemember(registry, job);
    if (job->status == UMI_STATUS_OK && job->action == HISTORY_FORGET)
        job->status = HistoryForget(registry, job->libraryPath);
    if (job->status == UMI_STATUS_OK && job->action == HISTORY_CLEAR) {
        /* An explicitly shared preferences file may also contain other recent
         * item families. Clear only the rows this selector owns. */
        size_t index = 0U;
        while (job->status == UMI_STATUS_OK && index < umi_platform_recent_items_registry_count(registry)) {
            UmiRecentItemSnapshot item;
            job->status = umi_platform_recent_items_registry_at(registry, index, &item);
            if (job->status != UMI_STATUS_OK) break;
            if (strcmp(item.kind, "workspace-library") == 0)
                job->status = umi_platform_recent_items_registry_remove(registry, item.id);
            else ++index;
        }
    }
    if (job->status == UMI_STATUS_OK && job->action != HISTORY_LOAD) {
        /* Only an explicit successful open or history edit creates the small
         * preferences directory. A library's parent is never created here. */
        char *parent = g_path_get_dirname(job->settingsPath);
        if (g_mkdir_with_parents(parent, 0700) != 0) job->status = UMI_STATUS_IO_ERROR;
        g_free(parent);
        if (job->status == UMI_STATUS_OK) job->status = UmiRecentItemsSaveLocal(registry, job->settingsPath);
    }
    if (job->status == UMI_STATUS_OK) {
        UmiRecentItemQuery query = {NULL, "workspace-library", 0, LIBRARY_HISTORY_LIMIT};
        job->status = umi_platform_recent_items_registry_query(registry, &query, job->items, LIBRARY_HISTORY_LIMIT, &job->count);
    }
    umi_platform_recent_items_registry_destroy(registry);
    G_UNLOCK(library_history_disk);
    /* The main thread reads the immutable result only after GTask publishes it.
     * This worker never reads a widget, host callback or borrowed data server. */
    g_task_return_boolean(task, TRUE);
}
static void HistoryFinished(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    GtkWidget *root = data;
    UmiWorkspaceLibraryGtk *library = g_object_get_data(G_OBJECT(root), "umicom-library-owner");
    LibraryHistoryJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    library->historyBusy = false;
    if (library->closed) { g_object_unref(root); return; }
    if (job->status == UMI_STATUS_OK) {
        GtkStringList *model = gtk_string_list_new(NULL);
        for (size_t index = 0U; index < job->count; ++index) {
            const char *uri = job->items[index].uri;
            /* Preferences are untrusted text. Offer only absolute local UTF-8
             * paths; selecting a row still requires a separate Open action. */
            if (g_utf8_validate(uri, -1, NULL) && g_path_is_absolute(uri)) gtk_string_list_append(model, uri);
        }
        gtk_drop_down_set_model(GTK_DROP_DOWN(library->recent), G_LIST_MODEL(model));
        g_object_unref(model);
        gtk_label_set_text(GTK_LABEL(library->historyMessage),
            job->action == HISTORY_LOAD ? "Recent locations loaded. Use selected fills the path; Open existing opens it." :
            job->action == HISTORY_REMEMBER ? "Library location remembered on this computer." :
            "Recent locations updated. Library files were not changed.");
    } else {
        char *message = g_strdup_printf("Recent locations could not be %s (status %d). Your open library is unchanged. Check the preferences location below.",
            job->action == HISTORY_LOAD ? "loaded" : "saved", (int)job->status);
        gtk_label_set_text(GTK_LABEL(library->historyMessage), message); g_free(message);
    }
    HistoryStart(library);
    g_object_unref(root);
}
static void HistoryStart(UmiWorkspaceLibraryGtk *library)
{
    if (library->closed || library->historyBusy || g_queue_is_empty(&library->historyQueue)) return;
    LibraryHistoryJob *job = g_queue_pop_head(&library->historyQueue);
    library->historyBusy = true;
    /* The completion, not GTask finalization, releases this GTK reference on
     * the main thread. Worker/task cleanup owns only plain metadata. */
    GTask *task = g_task_new(NULL, NULL, HistoryFinished, g_object_ref(library->root));
    g_task_set_task_data(task, job, HistoryJobFree);
    g_task_run_in_thread(task, HistoryWorker); g_object_unref(task);
}
static UmiStatus HistoryQueue(UmiWorkspaceLibraryGtk *library, LibraryHistoryAction action, const char *path)
{
    if (library == NULL || library->closed || library->historyPath == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (g_queue_get_length(&library->historyQueue) >= LIBRARY_HISTORY_QUEUE_LIMIT) {
        gtk_label_set_text(GTK_LABEL(library->historyMessage), "Recent locations are busy. The library remains usable; try Remember current later.");
        return UMI_STATUS_BUSY;
    }
    LibraryHistoryJob *job = g_new0(LibraryHistoryJob, 1);
    job->action = action; job->settingsPath = g_strdup(library->historyPath); job->libraryPath = g_strdup(path);
    /* Microseconds keep consecutive opens ordered without trusting the clock
     * as an identity or authorization token. A clock rollback affects order only. */
    gint64 now = g_get_real_time(); job->openedAt = now > 0 ? (uint64_t)now : 0U;
    g_queue_push_tail(&library->historyQueue, job); HistoryStart(library);
    return UMI_STATUS_OK;
}
static const char *HistorySelected(UmiWorkspaceLibraryGtk *library)
{
    gpointer item = gtk_drop_down_get_selected_item(GTK_DROP_DOWN(library->recent));
    return GTK_IS_STRING_OBJECT(item) ? gtk_string_object_get_string(GTK_STRING_OBJECT(item)) : NULL;
}
static void HistoryUse(GtkButton *button, gpointer data)
{
    (void)button; UmiWorkspaceLibraryGtk *library = data;
    if (library->closed) return;
    const char *path = HistorySelected(library);
    if (path != NULL) UmiWorkspaceLibraryGtkSetPath(library, path);
}
static void HistoryEdit(GtkButton *button, gpointer data)
{
    UmiWorkspaceLibraryGtk *library = data;
    if (library->closed) return;
    const char *action = g_object_get_data(G_OBJECT(button), "umicom-automation-id");
    if (strcmp(action, "workspace.library.history.clear") == 0) (void)HistoryQueue(library, HISTORY_CLEAR, NULL);
    else if (strcmp(action, "workspace.library.history.remember") == 0) (void)UmiWorkspaceLibraryGtkRememberCurrent(library);
    else { const char *path = HistorySelected(library); if (path != NULL) (void)HistoryQueue(library, HISTORY_FORGET, path); }
}
UmiStatus UmiWorkspaceLibraryGtkEnableHistory(UmiWorkspaceLibraryGtk *library, const char *settingsPath)
{
    if (library == NULL || library->closed || settingsPath == NULL ||
        !g_utf8_validate(settingsPath, -1, NULL) || !g_path_is_absolute(settingsPath)) return UMI_STATUS_INVALID_ARGUMENT;
    if (library->historyPath != NULL) return UMI_STATUS_ALREADY_EXISTS;
    library->historyPath = g_canonicalize_filename(settingsPath, NULL);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(library->root), box);
    Label(box, "Recent libraries on this computer");
    library->recent = gtk_drop_down_new(NULL, NULL); Tag(library->recent, "workspace.library.history.list");
    gtk_widget_set_hexpand(library->recent, TRUE); gtk_box_append(GTK_BOX(box), library->recent);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(box), row);
    Button(library, row, "Use selected", "workspace.library.history.use", false, G_CALLBACK(HistoryUse));
    Button(library, row, "Forget selected", "workspace.library.history.forget", false, G_CALLBACK(HistoryEdit));
    Button(library, row, "Clear recent list", "workspace.library.history.clear", false, G_CALLBACK(HistoryEdit));
    Button(library, row, "Remember current", "workspace.library.history.remember", false, G_CALLBACK(HistoryEdit));
    library->historyMessage = Label(box, "Loading recent locations..."); Tag(library->historyMessage, "workspace.library.history.status");
    Label(box, "Only file locations are remembered. This list contains no passwords or API keys.");
    GtkWidget *location = Label(box, library->historyPath); gtk_label_set_selectable(GTK_LABEL(location), TRUE);
    Tag(location, "workspace.library.history.location");
    return HistoryQueue(library, HISTORY_LOAD, NULL);
}
UmiStatus UmiWorkspaceLibraryGtkRememberCurrent(UmiWorkspaceLibraryGtk *library)
{
    if (library == NULL || library->closed || library->current == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return HistoryQueue(library, HISTORY_REMEMBER, library->current);
}
bool UmiWorkspaceLibraryGtkHistoryBusy(const UmiWorkspaceLibraryGtk *library)
{
    return library != NULL && !library->closed && (library->historyBusy || library->historyQueue.length != 0U);
}
