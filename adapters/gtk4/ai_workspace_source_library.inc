/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ai_workspace_source_library.inc
 * PURPOSE: Present explicit source selections and complete reviewed changes without borrowing services after teardown.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/source_change.h"

typedef struct AiSourceLibrary
{
    GtkWidget *root, *apply;
    GtkListBox *sources;
    GtkDropDown *operation;
    GtkEntry *collection, *collectionTitle, *document, *title;
    GtkTextBuffer *input, *preview;
    GtkCheckButton *approve;
    GtkLabel *message;
    GWeakRef host;
    UmiAiWorkspaceSourceChange *change;
    uint64_t generation, catalogueRevision;
    bool closed, rendering, refreshing, loaded;
} AiSourceLibrary;
static UmiAiWorkspaceGtkPanel *AiLibraryOwner(AiSourceLibrary *library)
{
    if (library->closed)
        return NULL;
    GObject *root = g_weak_ref_get(&library->host);
    UmiAiWorkspaceGtkPanel *host = root != NULL ? g_object_get_data(root, "umicom-ai-workspace-owner") : NULL;
    g_clear_object(&root);
    return Available(host) ? host : NULL;
}
static void AiLibraryFree(gpointer data)
{
    AiSourceLibrary *library = data;
    UmiAiWorkspaceSourceChangeDestroy(library->change);
    g_weak_ref_clear(&library->host);
    g_free(library);
}
/* The root owns the controller and all controls. Callbacks retain the root,
 * resolve the host before each service access and finish all service reads
 * before GTK notifications, which can close the application synchronously. */
static void AiLibraryRetire(GtkWidget *root)
{
    if (root == NULL)
        return;
    g_object_ref(root);
    AiSourceLibrary *library = g_object_get_data(G_OBJECT(root), "umicom-ai-library");
    if (!library->closed)
    {
        library->closed = true;
        UmiAiWorkspaceSourceChangeDestroy(library->change);
        library->change = NULL;
        gtk_text_buffer_set_text(library->input, "", 0);
        gtk_text_buffer_set_text(library->preview, "", 0);
        GtkWidget *child;
        while ((child = gtk_widget_get_first_child(GTK_WIDGET(library->sources))) != NULL)
            gtk_list_box_remove(library->sources, child);
        gtk_widget_set_sensitive(root, FALSE);
    }
    g_object_unref(root);
}
static void AiLibraryInvalidate(AiSourceLibrary *library)
{
    if (library->generation != UINT64_MAX)
        ++library->generation;
    UmiAiWorkspaceSourceChangeDestroy(library->change);
    library->change = NULL;
    gtk_check_button_set_active(library->approve, FALSE);
    gtk_widget_set_sensitive(library->apply, FALSE);
    if (gtk_text_buffer_get_char_count(library->preview) != 0)
        gtk_text_buffer_set_text(library->preview, "", 0);
}
static void AiLibraryChanged(GObject *object, gpointer data)
{
    (void)object;
    GtkWidget *root = g_object_ref(data);
    AiSourceLibrary *library = g_object_get_data(G_OBJECT(root), "umicom-ai-library");
    if (!library->closed)
    {
        AiLibraryInvalidate(library);
        if (!library->closed)
            gtk_label_set_text(library->message, "Selection or input changed. Preview and approve again.");
    }
    g_object_unref(root);
}
static void AiLibrarySelectionChanged(GtkListBox *box, gpointer data)
{
    AiSourceLibrary *library = g_object_get_data(G_OBJECT(data), "umicom-ai-library");
    if (!library->refreshing)
        AiLibraryChanged(G_OBJECT(box), data);
}
static void AiLibraryOperationChanged(GObject *object, GParamSpec *parameter, gpointer data)
{
    (void)parameter;
    AiLibraryChanged(object, data);
}
static const char *AiLibraryExplain(UmiStatus status)
{
    if (status == UMI_STATUS_ALREADY_EXISTS)
        return "A replacement ID belongs to an unselected passage, a selection is duplicated, or the "
               "collection name differs. No sources were changed.";
    if (status == UMI_STATUS_PERMISSION_DENIED)
        return "Read the full preview and select the approval checkbox before applying this change.";
    if (status == UMI_STATUS_BUSY)
        return "The workspace changed or is busy. Reload if needed, refresh the passage list, then preview "
               "again.";
    return Explain(status);
}
static void AiLibraryRefresh(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiSourceLibrary *library = g_object_get_data(G_OBJECT(root), "umicom-ai-library");
    if (library->rendering || AiLibraryOwner(library) == NULL)
    {
        g_object_unref(root);
        return;
    }
    library->rendering = true;
    library->refreshing = true;
    library->loaded = false;
    AiLibraryInvalidate(library);
    UmiAiWorkspaceGtkPanel *host = AiLibraryOwner(library);
    UmiAiWorkspaceSnapshot snapshot = {0};
    UmiStatus status =
        host != NULL ? UmiAiWorkspaceSnapshotRead(host->workspace, &snapshot) : UMI_STATUS_CANCELLED;
    UmiAiWorkspaceSource *sources = g_new0(UmiAiWorkspaceSource, UMI_AI_WORKSPACE_MAX_SOURCES);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot.sourceCount; ++i)
        status = UmiAiWorkspaceSourceAt(host->workspace, i, &sources[i]);
    /* GTK receives only copied records. Each row stores its exact source ID
     * and revision, so refresh and multi-selection never guess ownership from
     * a title, visible row number or the import naming convention. */
    GtkWidget *child;
    while (!library->closed && (child = gtk_widget_get_first_child(GTK_WIDGET(library->sources))) != NULL)
        gtk_list_box_remove(library->sources, child);
    for (size_t i = 0U; status == UMI_STATUS_OK && !library->closed && i < snapshot.sourceCount; ++i)
    {
        GtkWidget *row = gtk_list_box_row_new();
        UmiAiWorkspaceSource *source = g_memdup2(&sources[i], sizeof(sources[i]));
        g_object_set_data_full(G_OBJECT(row), "umicom-ai-source", source, g_free);
        char *text = g_strdup_printf("%s / %s — %s | lines %u–%u", source->collectionId, source->id,
                                     source->title, source->firstLine, source->lastLine);
        GtkWidget *label = gtk_label_new(text);
        g_free(text);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(library->sources, row);
    }
    g_free(sources);
    library->refreshing = false;
    library->rendering = false;
    if (!library->closed)
    {
        library->loaded = status == UMI_STATUS_OK;
        library->catalogueRevision = snapshot.revision;
        gtk_label_set_text(library->message, status == UMI_STATUS_OK
                                                 ? "Select the exact passages to change. Use Ctrl or Shift "
                                                   "for multiple rows; unselected passages stay as they are."
                                                 : AiLibraryExplain(status));
    }
    g_object_unref(root);
}
/* Copy the selected rows into an editable draft, never into storage. Users can
 * revise a prior import without locating and copying its original file again. */
static void AiLibraryCopy(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiSourceLibrary *library = g_object_get_data(G_OBJECT(root), "umicom-ai-library");
    if (library->rendering || AiLibraryOwner(library) == NULL)
    {
        g_object_unref(root);
        return;
    }
    library->rendering = true;
    GList *rows = gtk_list_box_get_selected_rows(library->sources);
    GString *text = g_string_new(NULL);
    for (GList *item = rows; item != NULL; item = item->next)
    {
        const UmiAiWorkspaceSource *source = g_object_get_data(G_OBJECT(item->data), "umicom-ai-source");
        if (source != NULL)
            g_string_append(text, source->text);
    }
    bool selected = rows != NULL;
    g_list_free(rows);
    if (selected)
        gtk_text_buffer_set_text(library->input, text->str, -1);
    g_string_free(text, TRUE);
    library->rendering = false;
    if (!library->closed)
        gtk_label_set_text(library->message,
                           selected ? "Selected text copied in displayed order, without added separators. "
                                      "Check its order, destination and document ID before previewing."
                                    : "Select at least one passage to copy.");
    g_object_unref(root);
}
static void AiLibraryDescribe(GString *text, const char *heading, const UmiAiWorkspaceSource *source)
{
    g_string_append_printf(text, "%s: %s / %s — %s\nLines %u–%u | source revision %" PRIu64 "\n%s\n\n",
                           heading, source->collectionId, source->id, source->title, source->firstLine,
                           source->lastLine, source->revision, source->text);
}
static void AiLibraryPrepare(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiSourceLibrary *library = g_object_get_data(G_OBJECT(root), "umicom-ai-library");
    if (library->rendering || AiLibraryOwner(library) == NULL)
    {
        g_object_unref(root);
        return;
    }
    library->rendering = true;
    AiLibraryInvalidate(library);
    uint64_t generation = library->generation;
    UmiAiWorkspaceGtkPanel *host = AiLibraryOwner(library);
    UmiAiWorkspaceSnapshot snapshot;
    UmiStatus status =
        host != NULL ? UmiAiWorkspaceSnapshotRead(host->workspace, &snapshot) : UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK && (!library->loaded || snapshot.revision != library->catalogueRevision))
        status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK && generation == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    const char *ids[UMI_AI_WORKSPACE_MAX_SOURCES];
    size_t count = 0U;
    GList *rows = gtk_list_box_get_selected_rows(library->sources);
    for (GList *item = rows; status == UMI_STATUS_OK && item != NULL; item = item->next)
    {
        const UmiAiWorkspaceSource *source = g_object_get_data(G_OBJECT(item->data), "umicom-ai-source");
        if (source == NULL || count == UMI_AI_WORKSPACE_MAX_SOURCES)
            status = UMI_STATUS_INVALID_STATE;
        else
            ids[count++] = source->id;
    }
    guint operation = gtk_drop_down_get_selected(library->operation);
    if (status == UMI_STATUS_OK && operation == 1U)
        status = UmiAiWorkspaceSourceRemovalCreate(host->workspace, ids, count, &library->change);
    else if (status == UMI_STATUS_OK && operation == 0U)
    {
        if ((size_t)gtk_text_buffer_get_char_count(library->input) > UMI_AI_WORKSPACE_IMPORT_MAX_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
        {
            char *text = BufferText(library->input);
            status = UmiAiWorkspaceSourceReplacementCreate(
                host->workspace, ids, count, Value(library->collection), Value(library->collectionTitle),
                Value(library->document), Value(library->title), text, strlen(text), &library->change);
            g_free(text);
        }
    }
    else if (status == UMI_STATUS_OK)
        status = UMI_STATUS_INVALID_ARGUMENT;
    g_list_free(rows);
    GString *text = g_string_new(NULL);
    if (status == UMI_STATUS_OK)
    {
        UmiAiWorkspaceSourceChangeSummary summary;
        (void)UmiAiWorkspaceSourceChangeInspect(library->change, &summary);
        g_string_append_printf(text,
                               "%s: %zu selected passages; %zu replacement passages.\n"
                               "Only the BEFORE IDs listed below are affected. Unselected passages and saved "
                               "job evidence are retained.\n"
                               "Removing a source from search does not erase copies in jobs or backups.\n\n",
                               summary.kind == UMI_AI_WORKSPACE_SOURCE_REMOVE ? "Remove from search"
                                                                              : "Replace in search",
                               summary.beforeCount, summary.afterCount);
        if (summary.kind == UMI_AI_WORKSPACE_SOURCE_REPLACE)
            g_string_append(text, "Replacement line numbers start at 1. UTF-8 BOM and line endings follow "
                                  "the document import rules.\n\n");
        if (!summary.hasChanges)
            g_string_append(text, "The saved content is identical. No new revision will be created.\n\n");
        for (size_t i = 0U; i < summary.beforeCount; ++i)
        {
            UmiAiWorkspaceSource source;
            (void)UmiAiWorkspaceSourceChangeBeforeAt(library->change, i, &source);
            AiLibraryDescribe(text, "BEFORE", &source);
        }
        for (size_t i = 0U; i < summary.afterCount; ++i)
        {
            UmiAiWorkspaceSource source;
            (void)UmiAiWorkspaceSourceChangeAfterAt(library->change, i, &source);
            AiLibraryDescribe(text, "AFTER", &source);
        }
    }
    /* Format everything before exposing text to GTK. A notification can revoke
     * the capture or destroy its owner; never dereference it after rendering. */
    if (!library->closed)
        gtk_text_buffer_set_text(library->preview, text->str, -1);
    g_string_free(text, TRUE);
    library->rendering = false;
    if (!library->closed && library->generation == generation && library->change != NULL &&
        AiLibraryOwner(library) != NULL)
    {
        gtk_widget_set_sensitive(library->apply, status == UMI_STATUS_OK);
        if (!library->closed)
            gtk_label_set_text(
                library->message,
                status == UMI_STATUS_OK
                    ? "Review every BEFORE and AFTER passage, then explicitly approve this change."
                    : AiLibraryExplain(status));
    }
    else if (!library->closed && status != UMI_STATUS_OK)
        gtk_label_set_text(library->message, AiLibraryExplain(status));
    g_object_unref(root);
}
static void AiLibraryApply(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    AiSourceLibrary *library = g_object_get_data(G_OBJECT(root), "umicom-ai-library");
    UmiAiWorkspaceGtkPanel *host = AiLibraryOwner(library);
    if (library->rendering || host == NULL || library->change == NULL)
    {
        g_object_unref(root);
        return;
    }
    library->rendering = true;
    UmiStatus status = UmiAiWorkspaceSourceChangeApply(host->workspace, library->change,
                                                       gtk_check_button_get_active(library->approve));
    /* A failed approval may be corrected without losing the visible preview.
     * Other outcomes retire this capture before any notification is delivered. */
    if (status != UMI_STATUS_PERMISSION_DENIED)
        AiLibraryInvalidate(library);
    if (status == UMI_STATUS_OK)
        library->loaded = false;
    library->rendering = false;
    if (!library->closed)
        gtk_label_set_text(library->message,
                           status == UMI_STATUS_OK
                               ? "Reviewed change completed. Refresh the passage list before another edit. "
                                 "Search now uses the saved sources; earlier jobs keep their evidence."
                               : AiLibraryExplain(status));
    g_object_unref(root);
}
static GtkWidget *AiLibraryCreate(UmiAiWorkspaceGtkPanel *host)
{
    AiSourceLibrary *library = g_new0(AiSourceLibrary, 1);
    library->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    g_object_ref_sink(library->root);
    library->generation = 1U;
    g_weak_ref_init(&library->host, host->root);
    g_object_set_data_full(G_OBJECT(library->root), "umicom-ai-library", library, AiLibraryFree);
    (void)UmiGtk4RecordingSetPrivate(library->root, 1);
    GtkWidget *notice = gtk_label_new(
        "Maintain saved text without changing existing job evidence. Select exact passages, review the full "
        "change, then approve it. Import document remains available for new sources.");
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_box_append(GTK_BOX(library->root), notice);
    GtkWidget *refresh = gtk_button_new_with_label("Refresh saved passages");
    Tag(refresh, "ai.library.refresh");
    gtk_box_append(GTK_BOX(library->root), refresh);
    library->sources = GTK_LIST_BOX(gtk_list_box_new());
    Tag(GTK_WIDGET(library->sources), "ai.library.sources");
    gtk_list_box_set_selection_mode(library->sources, GTK_SELECTION_MULTIPLE);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, -1, 160);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), GTK_WIDGET(library->sources));
    gtk_box_append(GTK_BOX(library->root), scroll);
    GtkWidget *copy = gtk_button_new_with_label("Copy selected text to editor");
    Tag(copy, "ai.library.copy");
    gtk_box_append(GTK_BOX(library->root), copy);
    static const char *const operations[] = {"Replace selected passages",
                                             "Remove selected passages from search", NULL};
    library->operation = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(operations));
    Tag(GTK_WIDGET(library->operation), "ai.library.operation");
    gtk_box_append(GTK_BOX(library->root), GTK_WIDGET(library->operation));
    library->collection =
        Entry(library->root, "Replacement collection ID", "workshop", "ai.library.collection",
              "Ignored for removal. Existing collection names must match exactly.");
    library->collectionTitle =
        Entry(library->root, "Replacement collection name", "Community workshop",
              "ai.library.collection-title", "A new collection is created with the complete change.");
    library->document = Entry(library->root, "Replacement document ID", "replacement", "ai.library.document",
                              "New passage IDs end in .part.N. Reusing selected IDs is allowed; unselected "
                              "collisions are rejected.");
    library->title = Entry(library->root, "Replacement source title", "Revised source", "ai.library.title",
                           "The title appears in search results and future citations.");
    library->input = TextArea(library->root, "Complete replacement text (ignored for removal)",
                              "ai.library.text", 140, true);
    GtkWidget *prepare = gtk_button_new_with_label("Preview complete source change");
    Tag(prepare, "ai.library.prepare");
    gtk_box_append(GTK_BOX(library->root), prepare);
    library->preview =
        TextArea(library->root, "Complete BEFORE and AFTER passages", "ai.library.preview", 240, false);
    library->approve =
        GTK_CHECK_BUTTON(gtk_check_button_new_with_label("I approve exactly the source changes shown above"));
    Tag(GTK_WIDGET(library->approve), "ai.library.approve");
    gtk_box_append(GTK_BOX(library->root), GTK_WIDGET(library->approve));
    library->apply = gtk_button_new_with_label("Apply reviewed source change");
    Tag(library->apply, "ai.library.apply");
    gtk_widget_set_sensitive(library->apply, FALSE);
    gtk_box_append(GTK_BOX(library->root), library->apply);
    library->message = GTK_LABEL(gtk_label_new("Refresh the saved passages to begin."));
    Tag(GTK_WIDGET(library->message), "ai.library.message");
    gtk_label_set_wrap(library->message, TRUE);
    gtk_box_append(GTK_BOX(library->root), GTK_WIDGET(library->message));
    GtkEntry *entries[] = {library->collection, library->collectionTitle, library->document, library->title};
    for (size_t i = 0U; i < sizeof(entries) / sizeof(entries[0]); ++i)
        g_signal_connect_object(entries[i], "changed", G_CALLBACK(AiLibraryChanged), library->root, 0);
    g_signal_connect_object(library->input, "changed", G_CALLBACK(AiLibraryChanged), library->root, 0);
    g_signal_connect_object(library->sources, "selected-rows-changed", G_CALLBACK(AiLibrarySelectionChanged),
                            library->root, 0);
    g_signal_connect_object(library->operation, "notify::selected", G_CALLBACK(AiLibraryOperationChanged),
                            library->root, 0);
    g_signal_connect_object(refresh, "clicked", G_CALLBACK(AiLibraryRefresh), library->root, 0);
    g_signal_connect_object(copy, "clicked", G_CALLBACK(AiLibraryCopy), library->root, 0);
    g_signal_connect_object(prepare, "clicked", G_CALLBACK(AiLibraryPrepare), library->root, 0);
    g_signal_connect_object(library->apply, "clicked", G_CALLBACK(AiLibraryApply), library->root, 0);
    return library->root;
}
