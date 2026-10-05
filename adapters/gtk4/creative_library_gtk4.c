/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_library_gtk4.c
 * PURPOSE: Present complete asset collections with explicit storage actions and ownership-safe worker completion.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/creative_library.h"
#include "umicom/ui/gtk4/creative_assets.h"
#include "umicom/creative_workspace/asset_library_archive.h"
#include <string.h>

typedef enum Action
{
    LIBRARY_IMPORT,
    LIBRARY_SAVE,
    LIBRARY_OPEN,
    LIBRARY_EXPORT,
    LIBRARY_EDIT
} Action;
typedef struct Panel
{
    UmiCreativeAssetLibrary *library;
    UmiCreativeAsset *removed;
    char removed_id[UMI_CREATIVE_ID_CAPACITY];
    size_t removed_index;
    GWeakRef editor;
    UmiCancellationToken *cancel; /* Borrowed while the active job owns it. */
    GCancellable *chooser;
    bool busy, dirty, refreshing;
    GtkWidget *title, *rename, *source, *browse, *id, *label, *kind, *import, *selection, *details;
    GtkWidget *up, *down, *remove, *undo, *confirm, *archive, *choose_archive, *save, *open;
    GtkWidget *destination, *export, *edit, *cancel_button, *status, *summary;
} Panel;
typedef struct Job
{
    GWeakRef root;
    Action action;
    UmiCreativeAssetLibrary *library, *loaded;
    UmiCreativeAsset *capture;
    UmiCancellationToken *cancel;
    UmiCreativeAssetWriteResult written;
    char *path, *id, *label;
    UmiCreativeAssetKind kind;
    UmiStatus status;
} Job;
static Panel *State(GtkWidget *root) { return g_object_get_data(G_OBJECT(root), "umicom-creative-library"); }
static void Tag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
static void Dispose(gpointer data)
{
    Panel *panel = data;
    if (panel->cancel != NULL)
        umi_cancellation_token_request(panel->cancel);
    if (panel->chooser != NULL)
        g_cancellable_cancel(panel->chooser);
    UmiCreativeAssetLibraryDestroy(panel->library);
    UmiCreativeAssetDestroy(panel->removed);
    g_weak_ref_clear(&panel->editor);
    g_free(panel);
}
static UmiStatus Selected(Panel *panel, UmiCreativeAssetLibraryEntry *out)
{
    if (panel->busy)
        return UMI_STATUS_BUSY;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selection));
    return UmiCreativeAssetLibraryAt(panel->library, (size_t)selected, out);
}
static void Sensitivity(Panel *panel)
{
    bool blocked = panel->busy || panel->chooser != NULL;
    GtkWidget *inputs[] = {panel->title,     panel->rename,     panel->source,  panel->browse,
                           panel->id,        panel->label,      panel->kind,    panel->import,
                           panel->selection, panel->confirm,    panel->archive, panel->choose_archive,
                           panel->save,      panel->destination};
    for (size_t i = 0U; i < sizeof(inputs) / sizeof(inputs[0]); ++i)
        gtk_widget_set_sensitive(inputs[i], !blocked);
    UmiCreativeAssetLibraryEntry selected;
    bool has_selection = !blocked && Selected(panel, &selected) == UMI_STATUS_OK;
    gtk_widget_set_sensitive(panel->up, has_selection);
    gtk_widget_set_sensitive(panel->down, has_selection);
    gtk_widget_set_sensitive(panel->remove, has_selection);
    gtk_widget_set_sensitive(panel->export, has_selection);
    GObject *editor = g_weak_ref_get(&panel->editor);
    gtk_widget_set_sensitive(panel->edit, has_selection && editor != NULL);
    g_clear_object(&editor);
    gtk_widget_set_sensitive(panel->undo, !blocked && panel->removed != NULL);
    gtk_widget_set_sensitive(
        panel->open,
        !blocked && (!panel->dirty || gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->confirm))));
    gtk_widget_set_sensitive(panel->cancel_button, blocked);
}
static void Describe(Panel *panel)
{
    UmiCreativeAssetLibraryEntry entry;
    if (Selected(panel, &entry) != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(panel->details), "Select an asset to inspect or export.");
        return;
    }
    const char *kinds[] = {"Document", "Image", "Video", "Audio", "Lyrics", "Binary file"};
    char *text = g_strdup_printf(
        "ID: %s\nName: %s\nDeclared purpose: %s\nComplete bytes: %zu\n"
        "Purpose does not validate a media format. Export writes the original captured bytes.",
        entry.id, entry.asset.label, kinds[(size_t)entry.asset.declared_kind - 1U], entry.asset.byte_count);
    gtk_label_set_text(GTK_LABEL(panel->details), text);
    g_free(text);
}
/* Row positions can change after removal or reordering. Preserve selection by
 * asset ID and rebuild only the small metadata model, never captured payloads. */
static void Refresh(Panel *panel, const char *selected_id)
{
    UmiCreativeAssetLibraryInfo info;
    if (UmiCreativeAssetLibraryInspect(panel->library, &info) != UMI_STATUS_OK)
        return;
    panel->refreshing = true;
    GtkStringList *rows = gtk_string_list_new(NULL);
    guint selection = GTK_INVALID_LIST_POSITION;
    for (size_t i = 0U; i < info.asset_count; ++i)
    {
        UmiCreativeAssetLibraryEntry entry;
        if (UmiCreativeAssetLibraryAt(panel->library, i, &entry) != UMI_STATUS_OK)
            break;
        char *text =
            g_strdup_printf("%s — %s (%zu bytes)", entry.id, entry.asset.label, entry.asset.byte_count);
        gtk_string_list_append(rows, text);
        g_free(text);
        if (selected_id != NULL && strcmp(selected_id, entry.id) == 0)
            selection = (guint)i;
    }
    /* set_model borrows its argument; the dropdown retains its own reference.
     * The former model may be released only after GTK has accepted this one. */
    gtk_drop_down_set_model(GTK_DROP_DOWN(panel->selection), G_LIST_MODEL(rows));
    g_object_unref(rows);
    if (selection == GTK_INVALID_LIST_POSITION && info.asset_count != 0U)
        selection = 0U;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->selection), selection);
    char *summary =
        g_strdup_printf("%s — %zu / %u assets, %zu / %u bytes. %s", info.title, info.asset_count,
                        UMI_CREATIVE_LIBRARY_MAX_ASSETS, info.byte_count, UMI_CREATIVE_LIBRARY_MAX_BYTES,
                        panel->dirty ? "Unsaved library edits." : "No unsaved library edits.");
    gtk_label_set_text(GTK_LABEL(panel->summary), summary);
    g_free(summary);
    panel->refreshing = false;
    Describe(panel);
    Sensitivity(panel);
}
static void SelectedChanged(GObject *object, GParamSpec *spec, gpointer root)
{
    (void)object;
    (void)spec;
    Panel *panel = State(GTK_WIDGET(root));
    if (panel->refreshing || panel->busy)
        return;
    Describe(panel);
    Sensitivity(panel);
}
static void ConfirmChanged(GtkCheckButton *button, gpointer root)
{
    (void)button;
    Sensitivity(State(GTK_WIDGET(root)));
}
static void Changed(Panel *panel)
{
    panel->dirty = true;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm), FALSE);
}
static void JobFree(gpointer data)
{
    Job *job = data;
    g_weak_ref_clear(&job->root);
    UmiCreativeAssetLibraryDestroy(job->library);
    UmiCreativeAssetLibraryDestroy(job->loaded);
    UmiCreativeAssetDestroy(job->capture);
    umi_cancellation_token_destroy(job->cancel);
    g_free(job->path);
    g_free(job->id);
    g_free(job->label);
    g_free(job);
}
static void Work(GTask *task, gpointer source, gpointer data, GCancellable *unused)
{
    (void)source;
    (void)unused;
    Job *job = data;
    if (job->action == LIBRARY_IMPORT)
    {
        UmiCreativeAssetLibraryInfo info;
        job->status = UmiCreativeAssetLibraryInspect(job->library, &info);
        if (job->status == UMI_STATUS_OK)
            job->status = UmiCreativeAssetLoadFile(job->path, job->label, job->kind,
                                                   UMI_CREATIVE_LIBRARY_MAX_BYTES - info.byte_count,
                                                   job->cancel, &job->capture);
    }
    else if (job->action == LIBRARY_SAVE)
        job->status = UmiCreativeAssetLibrarySaveNew(job->library, job->path, job->cancel, &job->written);
    else if (job->action == LIBRARY_OPEN)
        job->status =
            UmiCreativeAssetLibraryLoad(job->path, UMI_CREATIVE_LIBRARY_MAX_BYTES, job->cancel, &job->loaded);
    else
    {
        const UmiCreativeAsset *asset = NULL;
        job->status = UmiCreativeAssetLibraryBorrow(job->library, job->id, &asset);
        if (job->status == UMI_STATUS_OK && job->action == LIBRARY_EXPORT)
            job->status = UmiCreativeAssetWriteNew(asset, job->path, job->cancel, &job->written);
        else if (job->status == UMI_STATUS_OK)
        {
            UmiCreativeAssetInfo info;
            const void *bytes = NULL;
            size_t size = 0U;
            job->status = UmiCreativeAssetInspect(asset, &info);
            if (job->status == UMI_STATUS_OK)
                job->status = UmiCreativeAssetBytes(asset, &bytes, &size);
            if (job->status == UMI_STATUS_OK)
                job->status = UmiCreativeAssetCapture(info.label, info.declared_kind, bytes, size,
                                                      job->cancel, &job->capture);
        }
    }
    g_task_return_boolean(task, TRUE);
}
static void Done(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)source;
    (void)unused;
    Job *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWidget *root = g_weak_ref_get(&job->root);
    if (root == NULL)
        return;
    Panel *panel = State(root);
    panel->busy = false;
    panel->cancel = NULL;
    panel->library = job->library;
    job->library = NULL;
    /* Completion is the publication boundary. Even a successful read/copy is
     * refused when the user cancelled before GTK could handle its result. */
    if (umi_cancellation_token_is_requested(job->cancel) && job->action != LIBRARY_SAVE &&
        job->action != LIBRARY_EXPORT)
        job->status = UMI_STATUS_CANCELLED;
    if (job->status == UMI_STATUS_OK && job->action == LIBRARY_IMPORT)
    {
        job->status = UmiCreativeAssetLibraryInsert(panel->library, job->id, &job->capture);
        if (job->status == UMI_STATUS_OK)
            Changed(panel);
    }
    else if (job->status == UMI_STATUS_OK && job->action == LIBRARY_OPEN)
    {
        UmiCreativeAssetLibraryDestroy(panel->library);
        panel->library = job->loaded;
        job->loaded = NULL;
        UmiCreativeAssetDestroy(panel->removed);
        panel->removed = NULL;
        panel->dirty = false;
        UmiCreativeAssetLibraryInfo info;
        if (UmiCreativeAssetLibraryInspect(panel->library, &info) == UMI_STATUS_OK)
            gtk_editable_set_text(GTK_EDITABLE(panel->title), info.title);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm), FALSE);
    }
    else if (job->status == UMI_STATUS_OK && job->action == LIBRARY_SAVE)
    {
        panel->dirty = false;
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm), FALSE);
    }
    else if (job->status == UMI_STATUS_OK && job->action == LIBRARY_EDIT)
    {
        GtkWidget *editor = g_weak_ref_get(&panel->editor);
        job->status =
            editor != NULL ? UmiCreativeAssetsGtkAdopt(editor, &job->capture) : UMI_STATUS_UNAVAILABLE;
        g_clear_object(&editor);
    }
    Refresh(panel, job->id);
    const char *names[] = {"Import", "Save library", "Open library", "Export asset", "Copy to Assets editor"};
    char *message;
    if (job->written.created)
        message = g_strdup_printf("%s: %s. New file retained: %s (%" G_GUINT64_FORMAT " bytes written).",
                                  names[job->action], umi_status_text(job->status), job->written.file.path,
                                  (guint64)job->written.file.bytes_written);
    else
        message = g_strdup_printf("%s: %s.%s", names[job->action], umi_status_text(job->status),
                                  job->status == UMI_STATUS_OK && job->action == LIBRARY_EDIT
                                      ? " Open the Assets page to preview or edit this independent copy."
                                  : job->status != UMI_STATUS_OK ? " The current library is retained."
                                                                 : "");
    gtk_label_set_text(GTK_LABEL(panel->status), message);
    g_free(message);
    g_object_unref(root);
}
static void Start(GtkWidget *root, Action action)
{
    Panel *panel = State(root);
    if (panel->busy || panel->chooser != NULL)
        return;
    if (action == LIBRARY_OPEN && panel->dirty &&
        !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->confirm)))
    {
        gtk_label_set_text(GTK_LABEL(panel->status),
                           "Save your library first, or explicitly allow replacing its unsaved edits.");
        return;
    }
    UmiCreativeAssetLibraryEntry selected;
    if ((action == LIBRARY_EXPORT || action == LIBRARY_EDIT) && Selected(panel, &selected) != UMI_STATUS_OK)
        return;
    Job *job = g_new0(Job, 1);
    g_weak_ref_init(&job->root, G_OBJECT(root));
    job->action = action;
    job->id = g_strdup(action == LIBRARY_IMPORT ? gtk_editable_get_text(GTK_EDITABLE(panel->id))
                       : action == LIBRARY_EXPORT || action == LIBRARY_EDIT ? selected.id
                                                                            : "");
    job->label = g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->label)));
    job->kind = (UmiCreativeAssetKind)(gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->kind)) + 1U);
    job->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(action == LIBRARY_IMPORT   ? panel->source
                                                            : action == LIBRARY_EXPORT ? panel->destination
                                                                                       : panel->archive)));
    UmiStatus status = umi_cancellation_token_create(&job->cancel);
    if (status != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(panel->status), umi_status_text(status));
        JobFree(job);
        return;
    }
    /* Transfer the whole library for every worker operation. GTK cannot mutate
     * or borrow it until completion returns ownership, including failed opens. */
    job->library = panel->library;
    panel->library = NULL;
    panel->busy = true;
    panel->cancel = job->cancel;
    Sensitivity(panel);
    gtk_label_set_text(
        GTK_LABEL(panel->status),
        "Working. Cancellation retains the current library; a started write may leave a new file.");
    GTask *task = g_task_new(NULL, NULL, Done, NULL);
    g_task_set_task_data(task, job, JobFree);
    g_task_run_in_thread(task, Work);
    g_object_unref(task);
}
static void Import(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), LIBRARY_IMPORT);
}
static void Save(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), LIBRARY_SAVE);
}
static void Open(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), LIBRARY_OPEN);
}
static void Export(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), LIBRARY_EXPORT);
}
static void Edit(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), LIBRARY_EDIT);
}
static void Cancel(GtkButton *button, gpointer root)
{
    (void)button;
    Panel *panel = State(GTK_WIDGET(root));
    if (panel->cancel != NULL)
        umi_cancellation_token_request(panel->cancel);
    if (panel->chooser != NULL)
        g_cancellable_cancel(panel->chooser);
}
static void Rename(GtkButton *button, gpointer root)
{
    (void)button;
    Panel *panel = State(GTK_WIDGET(root));
    if (panel->busy || panel->chooser != NULL)
        return;
    UmiCreativeAssetLibraryInfo before;
    if (UmiCreativeAssetLibraryInspect(panel->library, &before) != UMI_STATUS_OK)
        return;
    const char *title = gtk_editable_get_text(GTK_EDITABLE(panel->title));
    UmiStatus status = UmiCreativeAssetLibraryTitle(panel->library, title);
    if (status == UMI_STATUS_OK && strcmp(title, before.title) != 0)
        Changed(panel);
    Refresh(panel, NULL);
    gtk_label_set_text(GTK_LABEL(panel->status),
                       status == UMI_STATUS_OK
                           ? "Library title applied."
                           : "Use a nonempty single-line title of at most 191 UTF-8 bytes.");
}
static void Move(GtkWidget *root, bool up)
{
    Panel *panel = State(root);
    UmiCreativeAssetLibraryEntry entry;
    UmiCreativeAssetLibraryInfo info;
    if (panel->busy || panel->chooser != NULL || Selected(panel, &entry) != UMI_STATUS_OK ||
        UmiCreativeAssetLibraryInspect(panel->library, &info) != UMI_STATUS_OK)
        return;
    size_t index = (size_t)gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selection));
    if ((up && index == 0U) || (!up && index + 1U == info.asset_count))
        return;
    if (UmiCreativeAssetLibraryMove(panel->library, entry.id, up ? index - 1U : index + 1U) == UMI_STATUS_OK)
    {
        Changed(panel);
        Refresh(panel, entry.id);
        gtk_label_set_text(GTK_LABEL(panel->status),
                           "Asset order changed. Save a new library file to keep it.");
    }
}
static void Up(GtkButton *button, gpointer root)
{
    (void)button;
    Move(GTK_WIDGET(root), true);
}
static void Down(GtkButton *button, gpointer root)
{
    (void)button;
    Move(GTK_WIDGET(root), false);
}
static void Remove(GtkButton *button, gpointer root)
{
    (void)button;
    Panel *panel = State(GTK_WIDGET(root));
    UmiCreativeAssetLibraryEntry entry;
    if (panel->busy || panel->chooser != NULL || Selected(panel, &entry) != UMI_STATUS_OK)
        return;
    UmiCreativeAsset *removed = NULL;
    size_t index = (size_t)gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selection));
    if (UmiCreativeAssetLibraryDetach(panel->library, entry.id, &removed) != UMI_STATUS_OK)
        return;
    UmiCreativeAssetDestroy(panel->removed);
    panel->removed = removed;
    panel->removed_index = index;
    memcpy(panel->removed_id, entry.id, strlen(entry.id) + 1U);
    Changed(panel);
    Refresh(panel, NULL);
    gtk_label_set_text(GTK_LABEL(panel->status),
                       "Removed from this library only. Undo last removal restores it until another removal "
                       "or successful Open. Disk files remain unchanged.");
}
static void Undo(GtkButton *button, gpointer root)
{
    (void)button;
    Panel *panel = State(GTK_WIDGET(root));
    if (panel->busy || panel->chooser != NULL || panel->removed == NULL)
        return;
    UmiStatus status = UmiCreativeAssetLibraryInsert(panel->library, panel->removed_id, &panel->removed);
    if (status == UMI_STATUS_OK)
    {
        UmiCreativeAssetLibraryInfo info;
        if (UmiCreativeAssetLibraryInspect(panel->library, &info) == UMI_STATUS_OK)
            (void)UmiCreativeAssetLibraryMove(
                panel->library, panel->removed_id,
                panel->removed_index < info.asset_count ? panel->removed_index : info.asset_count - 1U);
        Changed(panel);
        Refresh(panel, panel->removed_id);
    }
    gtk_label_set_text(GTK_LABEL(panel->status),
                       status == UMI_STATUS_OK ? "Last removal undone."
                                               : "Cannot restore yet: the ID or library capacity is "
                                                 "occupied. The removed capture remains available for Undo.");
}
typedef struct Choice
{
    GWeakRef root;
    GCancellable *cancel;
    bool archive;
    char *previous;
} Choice;
static void Chosen(GObject *source, GAsyncResult *result, gpointer data)
{
    Choice *choice = data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    char *path = file != NULL ? g_file_get_path(file) : NULL;
    GtkWidget *root = g_weak_ref_get(&choice->root);
    if (root != NULL)
    {
        Panel *panel = State(root);
        GtkWidget *field = choice->archive ? panel->archive : panel->source;
        if (!g_cancellable_is_cancelled(choice->cancel) && path != NULL &&
            UmiOutputFileValidatePath(path) == UMI_STATUS_OK &&
            strcmp(choice->previous, gtk_editable_get_text(GTK_EDITABLE(field))) == 0)
        {
            gtk_editable_set_text(GTK_EDITABLE(field), path);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm), FALSE);
            if (!choice->archive && gtk_editable_get_text(GTK_EDITABLE(panel->label))[0] == '\0')
            {
                char *name = g_filename_display_basename(path);
                gtk_editable_set_text(GTK_EDITABLE(panel->label), name);
                g_free(name);
            }
            gtk_label_set_text(GTK_LABEL(panel->status), "File selected. Review the fields, then choose "
                                                         "Import or Open. Nothing was read by selection.");
        }
        panel->chooser = NULL;
        Sensitivity(panel);
        g_object_unref(root);
    }
    g_free(path);
    g_clear_object(&file);
    g_clear_error(&error);
    g_object_unref(choice->cancel);
    g_weak_ref_clear(&choice->root);
    g_free(choice->previous);
    g_free(choice);
}
static void Choose(GtkWidget *root, bool archive)
{
    Panel *panel = State(root);
    if (panel->busy || panel->chooser != NULL)
        return;
    Choice *choice = g_new0(Choice, 1);
    g_weak_ref_init(&choice->root, G_OBJECT(root));
    choice->archive = archive;
    choice->previous =
        g_strdup(gtk_editable_get_text(GTK_EDITABLE(archive ? panel->archive : panel->source)));
    choice->cancel = g_cancellable_new();
    panel->chooser = choice->cancel;
    Sensitivity(panel);
    GtkRoot *parent = gtk_widget_get_root(root);
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, archive ? "Choose an asset library" : "Choose an asset to import");
    gtk_file_dialog_set_modal(dialog, TRUE);
    gtk_file_dialog_open(dialog, GTK_IS_WINDOW(parent) ? GTK_WINDOW(parent) : NULL, choice->cancel, Chosen,
                         choice);
    g_object_unref(dialog);
}
static void Browse(GtkButton *button, gpointer root)
{
    (void)button;
    Choose(GTK_WIDGET(root), false);
}
static void BrowseArchive(GtkButton *button, gpointer root)
{
    (void)button;
    Choose(GTK_WIDGET(root), true);
}
static void ArchiveChanged(GtkEditable *field, gpointer root)
{
    (void)field;
    Panel *panel = State(GTK_WIDGET(root));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm), FALSE);
}
static GtkWidget *Text(GtkWidget *root, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(root), label);
    return label;
}
static GtkWidget *Entry(GtkWidget *root, const char *label, const char *id)
{
    Text(root, label);
    GtkWidget *field = gtk_entry_new();
    Tag(field, id);
    gtk_box_append(GTK_BOX(root), field);
    return field;
}
static GtkWidget *Button(GtkWidget *root, const char *label, const char *id, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    Tag(button, id);
    g_signal_connect_object(button, "clicked", callback, root, 0);
    gtk_box_append(GTK_BOX(root), button);
    return button;
}
GtkWidget *UmiCreativeAssetLibraryGtkCreate(GtkWidget *asset_editor)
{
    if (asset_editor != NULL && !GTK_IS_WIDGET(asset_editor))
        return NULL;
    Panel *panel = g_new0(Panel, 1);
    if (UmiCreativeAssetLibraryCreate("Project assets", &panel->library) != UMI_STATUS_OK)
    {
        g_free(panel);
        return NULL;
    }
    g_weak_ref_init(&panel->editor, asset_editor != NULL ? G_OBJECT(asset_editor) : NULL);
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(root, "creative.library");
    g_object_set_data_full(G_OBJECT(root), "umicom-creative-library", panel, Dispose);
    Text(root, "Keep up to 32 complete assets together, with a combined 64 MiB payload. Save a new library "
               "file to reopen this collection in another creative workbench. Libraries are plaintext and "
               "separate from scene projects; nothing is uploaded.");
    panel->summary = Text(root, "");
    Tag(panel->summary, "creative.library.summary");
    panel->title = Entry(root, "Library title", "creative.library.title");
    gtk_editable_set_text(GTK_EDITABLE(panel->title), "Project assets");
    panel->rename = Button(root, "Apply library title", "creative.library.rename", G_CALLBACK(Rename));
    panel->source = Entry(root, "Absolute source file path", "creative.library.source");
    panel->browse = Button(root, "Choose source file...", "creative.library.browse", G_CALLBACK(Browse));
    panel->id =
        Entry(root, "Unique asset ID (letters, digits, dots, underscores or hyphens)", "creative.library.id");
    panel->label = Entry(root, "Name for this asset", "creative.library.label");
    Text(root, "Declared purpose (content still needs format validation)");
    const char *kinds[] = {"Document", "Image", "Video", "Audio", "Lyrics", "Binary file", NULL};
    panel->kind = gtk_drop_down_new_from_strings(kinds);
    Tag(panel->kind, "creative.library.kind");
    gtk_box_append(GTK_BOX(root), panel->kind);
    panel->import = Button(root, "Import complete file", "creative.library.import", G_CALLBACK(Import));
    Text(root, "Library assets");
    panel->selection = gtk_drop_down_new(NULL, NULL);
    Tag(panel->selection, "creative.library.selection");
    gtk_box_append(GTK_BOX(root), panel->selection);
    g_signal_connect_object(panel->selection, "notify::selected", G_CALLBACK(SelectedChanged), root, 0);
    panel->details = Text(root, "");
    Tag(panel->details, "creative.library.details");
    gtk_label_set_selectable(GTK_LABEL(panel->details), TRUE);
    panel->up = Button(root, "Move selected asset up", "creative.library.up", G_CALLBACK(Up));
    panel->down = Button(root, "Move selected asset down", "creative.library.down", G_CALLBACK(Down));
    panel->remove =
        Button(root, "Remove selected asset from library", "creative.library.remove", G_CALLBACK(Remove));
    panel->undo = Button(root, "Undo last removal", "creative.library.undo", G_CALLBACK(Undo));
    panel->edit =
        Button(root, "Copy selected asset to Assets editor", "creative.library.edit", G_CALLBACK(Edit));
    panel->destination =
        Entry(root, "New absolute file path for selected asset export", "creative.library.destination");
    panel->export =
        Button(root, "Export selected asset to new file", "creative.library.export", G_CALLBACK(Export));
    panel->archive = Entry(root, "Absolute library file path (for example, project.umilibrary)",
                           "creative.library.archive");
    panel->choose_archive =
        Button(root, "Choose saved library...", "creative.library.browse-archive", G_CALLBACK(BrowseArchive));
    panel->save = Button(root, "Save library to new file", "creative.library.save", G_CALLBACK(Save));
    panel->confirm = gtk_check_button_new_with_label("Allow Open to replace my unsaved library edits");
    Tag(panel->confirm, "creative.library.confirm");
    gtk_box_append(GTK_BOX(root), panel->confirm);
    g_signal_connect_object(panel->confirm, "toggled", G_CALLBACK(ConfirmChanged), root, 0);
    g_signal_connect_object(panel->archive, "changed", G_CALLBACK(ArchiveChanged), root, 0);
    panel->open = Button(root, "Open complete library", "creative.library.open", G_CALLBACK(Open));
    panel->cancel_button =
        Button(root, "Cancel pending operation", "creative.library.cancel", G_CALLBACK(Cancel));
    panel->status = Text(
        root, "The library is in memory until saved. Source files and existing saves are never overwritten.");
    Tag(panel->status, "creative.library.status");
    Refresh(panel, NULL);
    return root;
}
UmiStatus UmiCreativeAssetLibraryGtkInspect(GtkWidget *root, UmiCreativeAssetLibraryInfo *out)
{
    if (!GTK_IS_WIDGET(root) || State(root) == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    Panel *panel = State(root);
    if (panel->busy || panel->chooser != NULL)
        return UMI_STATUS_BUSY;
    return UmiCreativeAssetLibraryInspect(panel->library, out);
}
UmiStatus UmiCreativeAssetLibraryGtkAt(GtkWidget *root, size_t index, UmiCreativeAssetLibraryEntry *out)
{
    if (!GTK_IS_WIDGET(root) || State(root) == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    Panel *panel = State(root);
    if (panel->busy || panel->chooser != NULL)
        return UMI_STATUS_BUSY;
    return UmiCreativeAssetLibraryAt(panel->library, index, out);
}
