/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/layout_library_gtk4.c
 *
 * PURPOSE:
 *   Render the existing workspace owner's named layouts and submit copied,
 *   revision-checked requests without owning a second layout registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/layout_library.h"
#include "umicom/ui/gtk4/automation.h"

#include <stdio.h>
#include <string.h>

/* Strings in the portable request are borrowed; this idle owns their copies. */
typedef struct PendingLibraryRequest {
    UmiGtk4WorkspaceLayoutLibrary *library;
    UmiUiWorkspaceLibraryAction action;
    char target[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char new_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    uint64_t expected_revision;
    bool confirmed;
} PendingLibraryRequest;

/* Storage requests contain only value evidence captured by an explicit click. */
typedef struct PendingLibraryStorageRequest {
    UmiGtk4WorkspaceLayoutLibrary *library;
    UmiGtk4WorkspaceLayoutLibraryStorageRequest request;
} PendingLibraryStorageRequest;

struct UmiGtk4WorkspaceLayoutLibrary {
    GtkWidget *popover;
    GtkWidget *list;
    GtkWidget *search;
    GtkWidget *selected_label;
    GtkWidget *new_id;
    GtkWidget *name;
    GtkWidget *confirm;
    GtkWidget *open;
    GtkWidget *duplicate;
    GtkWidget *rename;
    GtkWidget *remove;
    GtkWidget *refresh;
    GtkWidget *status;
    GtkWidget *notice;
    GtkWidget *storage_status;
    GtkWidget *save_library;
    GtkWidget *restore_library;
    GtkWidget *confirm_restore;
    UmiGtk4WorkspaceLayoutLibraryReadHandler read_handler;
    UmiGtk4WorkspaceLayoutLibraryApplyHandler apply_handler;
    void *context;
    UmiGtk4WorkspaceLayoutLibraryStorageReadHandler storage_read_handler;
    UmiGtk4WorkspaceLayoutLibraryStorageOperationHandler storage_operation_handler;
    void *storage_context;
    UmiGtk4WorkspaceLayoutLibraryStorageState storage_state;
    bool storage_valid;
    UmiUiWorkspaceLibrarySnapshot snapshot;
    char selected_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char *query;
    guint pending_id;
    unsigned operation_depth;
    bool valid;
    bool syncing;
    bool in_callback;
    bool destroy_requested;
};

/* Forward declarations keep lifecycle and UI projection in one C component. */
static void update_controls(UmiGtk4WorkspaceLayoutLibrary *library);
static UmiStatus refresh_view(UmiGtk4WorkspaceLayoutLibrary *library);
static UmiStatus refresh_storage_state(UmiGtk4WorkspaceLayoutLibrary *library);

/* Retained native descendants must not borrow a destroyed controller. */
static void disconnect_widgets(GtkWidget *widget, UmiGtk4WorkspaceLayoutLibrary *library)
{
    if (widget == NULL) return;
    g_signal_handlers_disconnect_by_data(widget, library);
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        disconnect_widgets(child, library);
}

/* Final release occurs only after all synchronous owner callbacks return. */
static void release_library(UmiGtk4WorkspaceLayoutLibrary *library)
{
    if (library->popover != NULL) g_object_unref(library->popover);
    g_free(library->query);
    g_free(library);
}

/* Return false when finishing this operation also releases the controller. */
static bool finish_operation(UmiGtk4WorkspaceLayoutLibrary *library)
{
    --library->operation_depth;
    if (library->destroy_requested && library->operation_depth == 0U) {
        release_library(library);
        return false;
    }
    return true;
}

/* Cancel work and remove borrowed callbacks before native ownership changes. */
void umi_gtk4_ws_layout_library_destroy(UmiGtk4WorkspaceLayoutLibrary *library)
{
    if (library == NULL || library->destroy_requested) return;
    library->destroy_requested = true;
    if (library->pending_id != 0U) {
        guint source = library->pending_id;
        library->pending_id = 0U;
        (void)g_source_remove(source);
    }
    disconnect_widgets(library->popover, library);
    /* GtkListBox retains its filter callback separately from GObject signals. */
    if (library->list != NULL)
        gtk_list_box_set_filter_func(GTK_LIST_BOX(library->list), NULL, NULL, NULL);
    if (library->operation_depth == 0U) release_library(library);
}

/* Use the native theme's error role and keep the full explanation selectable. */
static void show_status(UmiGtk4WorkspaceLayoutLibrary *library, const char *text, bool error)
{
    gtk_label_set_text(GTK_LABEL(library->status), text != NULL ? text : "");
    if (error) gtk_widget_add_css_class(library->status, "error");
    else gtk_widget_remove_css_class(library->status, "error");
}

/* Describe only the scope and durability actually supplied by the owner. */
static void update_storage_notice(UmiGtk4WorkspaceLayoutLibrary *library)
{
    const UmiGtk4WorkspaceLayoutLibraryStorageState *state = &library->storage_state;
    const char *notice;
    if (!library->storage_valid || !state->supported)
        notice = "Named layouts are kept for this session. Whole-library storage is unavailable. Active-layout Save is separate.";
    else if (!state->durable)
        notice = "Library storage is memory-only and does not survive restart. It covers named layouts and their active selection, not documents, global context groups or application settings.";
    else
        notice = "Save library stores named layouts, their arrangement and order, and the active selection for restart. It does not save documents, global context groups or application settings. Active-layout Save is separate.";
    gtk_label_set_text(GTK_LABEL(library->notice), notice);
    gtk_label_set_text(GTK_LABEL(library->storage_status), library->storage_valid
        ? (state->message[0] != '\0' ? state->message
            : (!state->supported ? "Library storage is not available."
                : (state->has_saved ? "A saved library is available."
                    : "No saved library has been confirmed. Confirm Restore library to check storage.")))
        : "Library storage is not available.");
}

/* Read cached owner evidence only; refreshing this view must not load storage.
 * A backend generation prevents a queued request crossing a server rebind. */
static UmiStatus refresh_storage_state(UmiGtk4WorkspaceLayoutLibrary *library)
{
    UmiGtk4WorkspaceLayoutLibraryStorageState state = {0};
    UmiStatus status = UMI_STATUS_OK;
    if (library->storage_read_handler == NULL) {
        library->storage_valid = false;
        (void)memset(&library->storage_state, 0, sizeof(library->storage_state));
        update_storage_notice(library);
        return UMI_STATUS_OK;
    }
    library->in_callback = true;
    status = library->storage_read_handler(&state, library->storage_context);
    library->in_callback = false;
    if (library->destroy_requested) return UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK &&
        (memchr(state.message, '\0', sizeof(state.message)) == NULL ||
         !g_utf8_validate(state.message, -1, NULL))) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) {
        library->storage_valid = false;
        gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm_restore), FALSE);
        update_storage_notice(library);
        return status;
    }
    if (!library->storage_valid || state.storage_revision != library->storage_state.storage_revision ||
        state.storage_generation != library->storage_state.storage_generation ||
        state.has_saved != library->storage_state.has_saved || state.supported != library->storage_state.supported)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm_restore), FALSE);
    library->storage_state = state;
    library->storage_valid = true;
    update_storage_notice(library);
    return UMI_STATUS_OK;
}

/* Resolve an identity only inside a validated copied snapshot. */
static const UmiUiWorkspaceLibraryRow *selected_row(const UmiGtk4WorkspaceLayoutLibrary *library)
{
    if (!library->valid) return NULL;
    for (size_t index = 0U; index < library->snapshot.layout_count; ++index)
        if (strcmp(library->snapshot.rows[index].layout_id, library->selected_id) == 0)
            return &library->snapshot.rows[index];
    return NULL;
}

/* Validate extension-facing copied text before any GTK or libc string access. */
static bool snapshot_safe(const UmiUiWorkspaceLibrarySnapshot *snapshot)
{
    size_t active_count = 0U;
    if (snapshot->layout_count > UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS) return false;
    for (size_t index = 0U; index < snapshot->layout_count; ++index) {
        const UmiUiWorkspaceLibraryRow *row = &snapshot->rows[index];
        if (memchr(row->layout_id, '\0', sizeof(row->layout_id)) == NULL || row->layout_id[0] == '\0' ||
            memchr(row->name, '\0', sizeof(row->name)) == NULL ||
            !g_utf8_validate(row->layout_id, -1, NULL) || !g_utf8_validate(row->name, -1, NULL) ||
            row->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) return false;
        if (row->active) ++active_count;
        for (size_t previous = 0U; previous < index; ++previous)
            if (strcmp(row->layout_id, snapshot->rows[previous].layout_id) == 0) return false;
    }
    return active_count <= 1U;
}

/* Compare row content, not struct padding or refresh-only owner revisions. */
static bool rows_equal(const UmiUiWorkspaceLibrarySnapshot *left, const UmiUiWorkspaceLibrarySnapshot *right)
{
    if (left->layout_count != right->layout_count) return false;
    for (size_t index = 0U; index < left->layout_count; ++index) {
        const UmiUiWorkspaceLibraryRow *a = &left->rows[index];
        const UmiUiWorkspaceLibraryRow *b = &right->rows[index];
        if (strcmp(a->layout_id, b->layout_id) != 0 || strcmp(a->name, b->name) != 0 ||
            a->window_count != b->window_count || a->active != b->active || a->locked != b->locked)
            return false;
    }
    return true;
}

/* Searching affects only this copied presentation, never active layout state. */
static gboolean filter_row(GtkListBoxRow *row, gpointer data)
{
    UmiGtk4WorkspaceLayoutLibrary *library = data;
    const char *search = g_object_get_data(G_OBJECT(row), "umicom-layout-library-search");
    return library->query == NULL || library->query[0] == '\0' ||
        (search != NULL && strstr(search, library->query) != NULL);
}

/* Selection remains explicit, including when a search temporarily hides it. */
static void update_controls(UmiGtk4WorkspaceLayoutLibrary *library)
{
    const UmiUiWorkspaceLibraryRow *row = selected_row(library);
    const bool ready = row != NULL && !library->snapshot.editing &&
        library->pending_id == 0U && !library->in_callback && !library->destroy_requested;
    const bool storage_ready = library->valid && !library->snapshot.editing &&
        library->pending_id == 0U && !library->in_callback && !library->destroy_requested &&
        library->storage_valid && library->storage_state.supported &&
        library->storage_operation_handler != NULL;
    char *message = row != NULL ? g_strdup_printf("Selected: %s\n%s", row->name, row->layout_id)
        : g_strdup("Select a layout from the list.");
    gtk_label_set_text(GTK_LABEL(library->selected_label), message);
    g_free(message);
    gtk_widget_set_sensitive(library->open, ready);
    gtk_widget_set_sensitive(library->duplicate, ready &&
        library->snapshot.layout_count < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS &&
        gtk_editable_get_text(GTK_EDITABLE(library->new_id))[0] != '\0' &&
        gtk_editable_get_text(GTK_EDITABLE(library->name))[0] != '\0');
    gtk_widget_set_sensitive(library->rename, ready &&
        gtk_editable_get_text(GTK_EDITABLE(library->name))[0] != '\0');
    gtk_widget_set_sensitive(library->remove, ready && library->snapshot.layout_count > 1U &&
        gtk_check_button_get_active(GTK_CHECK_BUTTON(library->confirm)));
    gtk_widget_set_sensitive(library->confirm, ready && library->snapshot.layout_count > 1U);
    gtk_widget_set_sensitive(library->refresh, library->pending_id == 0U && !library->in_callback);
    gtk_widget_set_sensitive(library->save_library, storage_ready &&
        library->storage_state.revision_known && library->storage_state.save_enabled);
    gtk_widget_set_sensitive(library->confirm_restore, storage_ready &&
        library->storage_state.restore_enabled);
    gtk_widget_set_sensitive(library->restore_library, storage_ready &&
        library->storage_state.restore_enabled &&
        gtk_check_button_get_active(GTK_CHECK_BUTTON(library->confirm_restore)));
}

/* Read the selected native row's copied ID and populate the rename draft only
 * for a deliberate selection change, not for an ordinary polling refresh. */
static void on_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data)
{
    UmiGtk4WorkspaceLayoutLibrary *library = data;
    const char *id;
    const UmiUiWorkspaceLibraryRow *selected;
    (void)list;
    if (library->syncing || row == NULL) return;
    id = g_object_get_data(G_OBJECT(row), "umicom-layout-library-id");
    if (id == NULL || strcmp(id, library->selected_id) == 0) return;
    (void)g_strlcpy(library->selected_id, id, sizeof(library->selected_id));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm), FALSE);
    selected = selected_row(library);
    if (selected != NULL) gtk_editable_set_text(GTK_EDITABLE(library->name), selected->name);
    update_controls(library);
}

/* Preserve input widgets and selection while narrowing visible native rows. */
static void on_search_changed(GtkSearchEntry *entry, gpointer data)
{
    UmiGtk4WorkspaceLayoutLibrary *library = data;
    char *query = g_utf8_casefold(gtk_editable_get_text(GTK_EDITABLE(entry)), -1);
    if (query == NULL) return;
    g_strstrip(query);
    g_free(library->query); library->query = query;
    library->syncing = true;
    gtk_list_box_invalidate_filter(GTK_LIST_BOX(library->list));
    /* A selected layout may have been duplicated while hidden by this query.
     * Restore its native highlight once it is visible, without changing ID. */
    for (GtkWidget *row = gtk_widget_get_first_child(library->list); row != NULL;
         row = gtk_widget_get_next_sibling(row)) {
        const char *id = g_object_get_data(G_OBJECT(row), "umicom-layout-library-id");
        if (GTK_IS_LIST_BOX_ROW(row) && gtk_widget_get_child_visible(row) &&
            g_strcmp0(id, library->selected_id) == 0)
            gtk_list_box_select_row(GTK_LIST_BOX(library->list), GTK_LIST_BOX_ROW(row));
    }
    library->syncing = false;
    update_controls(library);
}

/* Text and confirmation changes affect affordances without applying a model. */
static void on_input_changed(GObject *widget, gpointer data)
{
    (void)widget;
    update_controls(data);
}

/* Rebuild only changed copied rows; old native row references become inert. */
static void rebuild_rows(UmiGtk4WorkspaceLayoutLibrary *library)
{
    GtkWidget *child = gtk_widget_get_first_child(library->list);
    library->syncing = true;
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        /* The native empty-search placeholder is not a layout row. */
        if (GTK_IS_LIST_BOX_ROW(child)) {
            disconnect_widgets(child, library);
            gtk_list_box_remove(GTK_LIST_BOX(library->list), child);
        }
        child = next;
    }
    for (size_t index = 0U; index < library->snapshot.layout_count; ++index) {
        const UmiUiWorkspaceLibraryRow *item = &library->snapshot.rows[index];
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
        GtkWidget *name = gtk_label_new(item->name);
        char *description = g_strdup_printf("%s · %zu panels%s%s", item->layout_id, item->window_count,
            item->active ? " · Active" : "", item->locked ? " · Layout locked" : "");
        GtkWidget *detail = gtk_label_new(description);
        char *search = g_strconcat(item->name, " ", item->layout_id, NULL);
        char tag[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 64U];
        gtk_label_set_xalign(GTK_LABEL(name), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(detail), 0.0F);
        gtk_label_set_wrap(GTK_LABEL(name), TRUE);
        gtk_label_set_wrap(GTK_LABEL(detail), TRUE);
        gtk_widget_add_css_class(detail, "dim-label");
        gtk_widget_set_margin_start(box, 8); gtk_widget_set_margin_end(box, 8);
        gtk_widget_set_margin_top(box, 7); gtk_widget_set_margin_bottom(box, 7);
        gtk_box_append(GTK_BOX(box), name); gtk_box_append(GTK_BOX(box), detail);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);
        g_object_set_data_full(G_OBJECT(row), "umicom-layout-library-id", g_strdup(item->layout_id), g_free);
        g_object_set_data_full(G_OBJECT(row), "umicom-layout-library-search", g_utf8_casefold(search, -1), g_free);
        (void)snprintf(tag, sizeof(tag), "workstation.layout-library.row.%s", item->layout_id);
        (void)umi_gtk4_automation_tag_widget(row, tag);
        gtk_list_box_append(GTK_LIST_BOX(library->list), row);
        if (strcmp(item->layout_id, library->selected_id) == 0)
            gtk_list_box_select_row(GTK_LIST_BOX(library->list), GTK_LIST_BOX_ROW(row));
        g_free(search); g_free(description);
    }
    library->syncing = false;
}

/* Read through the existing owner and publish the copied view only after its
 * complete count/text validation. A failed read never replaces good rows. */
static UmiStatus refresh_view(UmiGtk4WorkspaceLayoutLibrary *library)
{
    UmiUiWorkspaceLibrarySnapshot *candidate = g_try_new0(UmiUiWorkspaceLibrarySnapshot, 1);
    UmiStatus status;
    bool changed;
    if (candidate == NULL) {
        library->valid = false;
        show_status(library, umi_status_text(UMI_STATUS_OUT_OF_MEMORY), true);
        update_controls(library);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    library->in_callback = true;
    status = library->read_handler(candidate, library->context);
    library->in_callback = false;
    if (library->destroy_requested) { g_free(candidate); return UMI_STATUS_CANCELLED; }
    if (status == UMI_STATUS_OK && !snapshot_safe(candidate)) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) {
        library->valid = false;
        show_status(library, umi_status_text(status), true);
        update_controls(library);
        g_free(candidate);
        return status;
    }
    changed = !library->valid || !rows_equal(&library->snapshot, candidate);
    if (!library->valid || candidate->customisation_revision != library->snapshot.customisation_revision) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm), FALSE);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm_restore), FALSE);
    }
    library->snapshot = *candidate;
    library->valid = true;
    /* Keep an existing selection, otherwise select the active row (or first).
     * A removed selection never silently targets a stale record. */
    if (selected_row(library) == NULL) {
        library->selected_id[0] = '\0';
        for (size_t index = 0U; index < candidate->layout_count; ++index)
            if (index == 0U || candidate->rows[index].active)
                (void)g_strlcpy(library->selected_id, candidate->rows[index].layout_id, sizeof(library->selected_id));
        if (gtk_editable_get_text(GTK_EDITABLE(library->name))[0] == '\0' && selected_row(library) != NULL)
            gtk_editable_set_text(GTK_EDITABLE(library->name), selected_row(library)->name);
    }
    if (changed) rebuild_rows(library);
    status = refresh_storage_state(library);
    if (library->destroy_requested) { g_free(candidate); return UMI_STATUS_CANCELLED; }
    update_controls(library);
    show_status(library, status != UMI_STATUS_OK ? umi_status_text(status) :
        (candidate->editing ? "Finish or cancel Edit Layout before changing the library."
        : (candidate->layout_count == 0U ? "No named layouts are available." : "Layout library refreshed.")), status != UMI_STATUS_OK);
    g_free(candidate);
    return status;
}

/* Optional storage remains disabled until an owner supplies both callbacks.
 * Replacing the binding never performs a save or a restore. */
UmiStatus umi_gtk4_ws_layout_library_set_storage_handlers(
    UmiGtk4WorkspaceLayoutLibrary *library,
    UmiGtk4WorkspaceLayoutLibraryStorageReadHandler read_handler,
    UmiGtk4WorkspaceLayoutLibraryStorageOperationHandler operation_handler, void *context)
{
    UmiStatus status;
    if (library == NULL || library->destroy_requested ||
        (read_handler == NULL) != (operation_handler == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    if (library->pending_id != 0U || library->in_callback) return UMI_STATUS_BUSY;
    ++library->operation_depth;
    library->storage_read_handler = read_handler;
    library->storage_operation_handler = operation_handler;
    library->storage_context = context;
    library->storage_valid = false;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm_restore), FALSE);
    status = refresh_storage_state(library);
    if (!library->destroy_requested) {
        update_controls(library);
        if (status != UMI_STATUS_OK) show_status(library, umi_status_text(status), true);
    }
    (void)finish_operation(library);
    return status;
}

/* Public refresh acknowledges a fresh owner revision without changing drafts. */
UmiStatus umi_gtk4_ws_layout_library_refresh(UmiGtk4WorkspaceLayoutLibrary *library)
{
    UmiStatus status;
    if (library == NULL || library->destroy_requested) return UMI_STATUS_INVALID_ARGUMENT;
    if (library->pending_id != 0U || library->in_callback) return UMI_STATUS_BUSY;
    ++library->operation_depth;
    status = refresh_view(library);
    (void)finish_operation(library);
    return status;
}

/* The Refresh button uses the same public read boundary as an owner refresh. */
static void on_refresh_clicked(GtkButton *button, gpointer data)
{
    (void)button;
    (void)umi_gtk4_ws_layout_library_refresh(data);
}

/* Opening after other workspace actions must acknowledge the current owner
 * revision. Queued operations keep their original request and return BUSY. */
static void on_popover_visible(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    if (gtk_widget_get_visible(GTK_WIDGET(object)))
        (void)umi_gtk4_ws_layout_library_refresh(data);
}

/* Submit a stable copied request after its originating GTK signal unwinds. */
static gboolean apply_from_idle(gpointer data)
{
    PendingLibraryRequest *pending = data;
    UmiGtk4WorkspaceLayoutLibrary *library = pending->library;
    UmiUiWorkspaceLibraryRequest request = {0};
    UmiStatus status;
    library->pending_id = 0U;
    ++library->operation_depth;
    request.action = pending->action; request.target_layout_id = pending->target;
    request.new_layout_id = pending->new_id; request.name = pending->name;
    request.expected_customisation_revision = pending->expected_revision;
    request.confirmed = pending->confirmed;
    library->in_callback = true;
    status = library->apply_handler(&request, library->context);
    library->in_callback = false;
    if (!library->destroy_requested) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm), FALSE);
        if (status == UMI_STATUS_OK) {
            if (request.action == UMI_UI_WORKSPACE_LIBRARY_DUPLICATE)
                (void)g_strlcpy(library->selected_id, pending->new_id, sizeof(library->selected_id));
            status = refresh_view(library);
            if (!library->destroy_requested && status == UMI_STATUS_OK)
                show_status(library, "Layout change accepted for this session.", false);
        } else {
            /* Do not silently refresh a stale expected revision and retry. The
             * developer must inspect/refresh before submitting another change. */
            if (status == UMI_STATUS_INVALID_STATE || status == UMI_STATUS_BUSY) library->valid = false;
            show_status(library, status == UMI_STATUS_INVALID_STATE
                ? "The layout library changed. Refresh, check your selection, and try again."
                : umi_status_text(status), true);
        }
        if (!library->destroy_requested) update_controls(library);
    }
    (void)finish_operation(library);
    return G_SOURCE_REMOVE;
}

/* Execute only explicit storage clicks. The owner checks model revision,
 * storage revision and backend generation before performing its operation. */
static gboolean storage_from_idle(gpointer data)
{
    PendingLibraryStorageRequest *pending = data;
    UmiGtk4WorkspaceLayoutLibrary *library = pending->library;
    UmiStatus status;
    library->pending_id = 0U;
    ++library->operation_depth;
    library->in_callback = true;
    status = library->storage_operation_handler(&pending->request, library->storage_context);
    library->in_callback = false;
    if (!library->destroy_requested) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm_restore), FALSE);
        if (status == UMI_STATUS_OK) {
            status = refresh_view(library);
            if (!library->destroy_requested && status == UMI_STATUS_OK) {
                const bool restore = pending->request.action == UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_RESTORE;
                const char *summary = restore
                    ? "Saved named layouts restored to this session. Documents and global settings were not restored."
                    : (library->storage_state.durable ? "Named layout library saved for restart."
                        : "Named layout library saved in memory only; it will not survive restart.");
                char *message = library->storage_state.message[0] != '\0'
                    ? g_strdup_printf("%s\n%s", summary, library->storage_state.message) : g_strdup(summary);
                show_status(library, message, false);
                g_free(message);
            }
        } else {
            /* Do not retry or silently read storage after conflict. View
             * refresh only copies owner status; confirmed Restore rereads it. */
            if (status == UMI_STATUS_INVALID_STATE || status == UMI_STATUS_BUSY)
                library->storage_valid = false;
            /* An explicit missing-library probe can establish fresh known-zero
             * evidence. Copy that cached result without another storage read;
             * retain NOT_FOUND feedback and leave the session list untouched. */
            if (status == UMI_STATUS_NOT_FOUND)
                (void)refresh_storage_state(library);
            if (!library->destroy_requested)
                show_status(library, status == UMI_STATUS_INVALID_STATE
                    ? "The workspace or saved library changed. Refresh this view, then confirm Restore library to read the saved list before replacing this session; Save was not retried."
                    : umi_status_text(status), true);
        }
        if (!library->destroy_requested) update_controls(library);
    }
    (void)finish_operation(library);
    return G_SOURCE_REMOVE;
}

/* Capture both click-time revisions and replacement consent as plain values. */
static void on_storage_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4WorkspaceLayoutLibrary *library = data;
    PendingLibraryStorageRequest *pending;
    const bool restore = GTK_WIDGET(button) == library->restore_library;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(button)) || library->pending_id != 0U ||
        library->in_callback || library->destroy_requested || !library->valid || library->snapshot.editing ||
        !library->storage_valid || !library->storage_state.supported || library->storage_operation_handler == NULL)
        return;
    if (restore ? (!library->storage_state.restore_enabled ||
                   !gtk_check_button_get_active(GTK_CHECK_BUTTON(library->confirm_restore)))
                : (!library->storage_state.revision_known || !library->storage_state.save_enabled)) return;
    pending = g_try_new0(PendingLibraryStorageRequest, 1);
    if (pending == NULL) { show_status(library, umi_status_text(UMI_STATUS_OUT_OF_MEMORY), true); return; }
    pending->library = library;
    pending->request.action = restore ? UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_RESTORE
        : UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_SAVE;
    pending->request.expected_customisation_revision = library->snapshot.customisation_revision;
    pending->request.expected_storage_revision = library->storage_state.storage_revision;
    pending->request.expected_storage_generation = library->storage_state.storage_generation;
    pending->request.restore_confirmed = restore && gtk_check_button_get_active(GTK_CHECK_BUTTON(library->confirm_restore));
    library->pending_id = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, storage_from_idle, pending, g_free);
    if (library->pending_id == 0U) { g_free(pending); return; }
    show_status(library, restore ? "Restoring the confirmed named layout library…" : "Saving the named layout library…", false);
    update_controls(library);
}

/* Copy bounded UTF-8 inputs without truncating the developer's visible draft. */
static bool copy_input(char *destination, size_t capacity, const char *text)
{
    return text != NULL && g_utf8_validate(text, -1, NULL) &&
        g_strlcpy(destination, text, capacity) < capacity;
}

/* Queue only enabled actions, retaining the revision the developer actually saw. */
static void on_action_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4WorkspaceLayoutLibrary *library = data;
    PendingLibraryRequest *pending;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(button)) || library->pending_id != 0U ||
        library->in_callback || library->destroy_requested || !library->valid || selected_row(library) == NULL)
        return;
    pending = g_try_new0(PendingLibraryRequest, 1);
    if (pending == NULL) { show_status(library, umi_status_text(UMI_STATUS_OUT_OF_MEMORY), true); return; }
    pending->library = library;
    pending->action = (UmiUiWorkspaceLibraryAction)GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "umicom-layout-library-action"));
    pending->expected_revision = library->snapshot.customisation_revision;
    pending->confirmed = gtk_check_button_get_active(GTK_CHECK_BUTTON(library->confirm));
    if (!copy_input(pending->target, sizeof(pending->target), library->selected_id) ||
        ((pending->action == UMI_UI_WORKSPACE_LIBRARY_DUPLICATE) &&
         !copy_input(pending->new_id, sizeof(pending->new_id), gtk_editable_get_text(GTK_EDITABLE(library->new_id)))) ||
        ((pending->action == UMI_UI_WORKSPACE_LIBRARY_DUPLICATE || pending->action == UMI_UI_WORKSPACE_LIBRARY_RENAME) &&
         !copy_input(pending->name, sizeof(pending->name), gtk_editable_get_text(GTK_EDITABLE(library->name))))) {
        g_free(pending);
        show_status(library, "The layout ID or name is too long. Shorten it and try again; no change was submitted.", true);
        return;
    }
    library->pending_id = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, apply_from_idle, pending, g_free);
    if (library->pending_id == 0U) { g_free(pending); return; }
    show_status(library, "Applying the requested layout change…", false);
    update_controls(library);
}

/* Build one native action control with stable automation identity. */
static GtkWidget *action_button(UmiGtk4WorkspaceLayoutLibrary *library, const char *label,
    const char *id, UmiUiWorkspaceLibraryAction action)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(button, id);
    g_object_set_data(G_OBJECT(button), "umicom-layout-library-action", GINT_TO_POINTER((int)action));
    g_signal_connect(button, "clicked", G_CALLBACK(on_action_clicked), library);
    return button;
}

/* Return the controller-retained popover for ordinary GtkMenuButton parenting. */
GtkWidget *umi_gtk4_ws_layout_library_popover(UmiGtk4WorkspaceLayoutLibrary *library)
{
    return library != NULL && !library->destroy_requested ? library->popover : NULL;
}

/* Compose native theme-aware controls; no model, persistence or native product
 * launch is performed here. The first owner read must succeed before return. */
UmiStatus umi_gtk4_ws_layout_library_create(
    UmiGtk4WorkspaceLayoutLibraryReadHandler read_handler,
    UmiGtk4WorkspaceLayoutLibraryApplyHandler apply_handler,
    void *context, UmiGtk4WorkspaceLayoutLibrary **out_library)
{
    UmiGtk4WorkspaceLayoutLibrary *library;
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *notice;
    GtkWidget *scroller;
    GtkWidget *frame;
    GtkWidget *actions;
    GtkWidget *storage_actions;
    GtkWidget *outer_scroll;
    GtkWidget *id_label;
    GtkWidget *name_label;
    GtkWidget *empty;
    UmiStatus status;
    if (out_library == NULL || read_handler == NULL || apply_handler == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_library = NULL;
    library = g_try_new0(UmiGtk4WorkspaceLayoutLibrary, 1);
    if (library == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    library->read_handler = read_handler; library->apply_handler = apply_handler; library->context = context;
    library->operation_depth = 1U;
    library->popover = g_object_ref_sink(gtk_popover_new());
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    title = gtk_label_new("Layout Library");
    notice = gtk_label_new(""); library->notice = notice;
    library->search = gtk_search_entry_new(); library->list = gtk_list_box_new();
    empty = gtk_label_new("No layouts match this search.");
    library->selected_label = gtk_label_new(""); library->new_id = gtk_entry_new();
    library->name = gtk_entry_new(); library->confirm = gtk_check_button_new_with_label("I confirm removal of the selected layout");
    library->status = gtk_label_new(""); library->refresh = gtk_button_new_with_label("Refresh");
    library->storage_status = gtk_label_new("");
    library->save_library = gtk_button_new_with_label("Save library");
    library->restore_library = gtk_button_new_with_label("Restore library");
    library->confirm_restore = gtk_check_button_new_with_label("Replace this session's named layout list");
    library->open = action_button(library, "Open", "workstation.layout-library.open", UMI_UI_WORKSPACE_LIBRARY_ACTIVATE);
    library->duplicate = action_button(library, "Duplicate", "workstation.layout-library.duplicate", UMI_UI_WORKSPACE_LIBRARY_DUPLICATE);
    library->rename = action_button(library, "Rename", "workstation.layout-library.rename", UMI_UI_WORKSPACE_LIBRARY_RENAME);
    library->remove = action_button(library, "Remove", "workstation.layout-library.remove", UMI_UI_WORKSPACE_LIBRARY_REMOVE);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    storage_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    outer_scroll = gtk_scrolled_window_new();
    scroller = gtk_scrolled_window_new(); frame = gtk_frame_new(NULL);
    id_label = gtk_label_new("New layout ID (full ID)"); name_label = gtk_label_new("Layout name");
    gtk_widget_set_margin_start(box, 12); gtk_widget_set_margin_end(box, 12);
    gtk_widget_set_margin_top(box, 12); gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_add_css_class(title, "heading"); gtk_widget_add_css_class(notice, "dim-label");
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE); gtk_label_set_max_width_chars(GTK_LABEL(notice), 64);
    gtk_label_set_wrap(GTK_LABEL(library->selected_label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(library->selected_label), TRUE);
    gtk_label_set_wrap(GTK_LABEL(library->status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(library->status), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(library->status), 64);
    gtk_label_set_wrap(GTK_LABEL(library->storage_status), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(library->storage_status), 64);
    gtk_label_set_xalign(GTK_LABEL(library->storage_status), 0.0F);
    gtk_widget_add_css_class(library->storage_status, "dim-label");
    gtk_widget_set_tooltip_text(library->confirm_restore,
        "Confirm before replacing the current session's entire named layout list and active layout with the saved library. Unsaved layout changes can be lost; document drafts are not a library backup.");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F); gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(library->selected_label), 0.0F); gtk_label_set_xalign(GTK_LABEL(library->status), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(id_label), 0.0F); gtk_label_set_xalign(GTK_LABEL(name_label), 0.0F);
    g_object_set(library->search, "placeholder-text", "Search layout names or IDs", NULL);
    gtk_widget_set_tooltip_text(library->new_id, "Use a unique full ID in the same application namespace as the selected layout ID above.");
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(library->list), GTK_SELECTION_SINGLE);
    gtk_label_set_wrap(GTK_LABEL(empty), TRUE);
    gtk_widget_set_margin_top(empty, 12); gtk_widget_set_margin_bottom(empty, 12);
    gtk_list_box_set_placeholder(GTK_LIST_BOX(library->list), empty);
    gtk_list_box_set_filter_func(GTK_LIST_BOX(library->list), filter_row, library, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroller), 160);
    gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(scroller), 280);
    gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(scroller), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), library->list);
    gtk_frame_set_child(GTK_FRAME(frame), scroller);
    gtk_box_append(GTK_BOX(actions), library->open); gtk_box_append(GTK_BOX(actions), library->duplicate);
    gtk_box_append(GTK_BOX(actions), library->rename); gtk_box_append(GTK_BOX(actions), library->remove);
    gtk_box_append(GTK_BOX(actions), library->refresh);
    gtk_box_append(GTK_BOX(storage_actions), library->save_library);
    gtk_box_append(GTK_BOX(storage_actions), library->restore_library);
    gtk_box_append(GTK_BOX(box), title); gtk_box_append(GTK_BOX(box), notice);
    gtk_box_append(GTK_BOX(box), library->search); gtk_box_append(GTK_BOX(box), frame);
    gtk_box_append(GTK_BOX(box), library->selected_label); gtk_box_append(GTK_BOX(box), id_label);
    gtk_box_append(GTK_BOX(box), library->new_id); gtk_box_append(GTK_BOX(box), name_label);
    gtk_box_append(GTK_BOX(box), library->name); gtk_box_append(GTK_BOX(box), library->confirm);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(box), library->storage_status);
    gtk_box_append(GTK_BOX(box), library->confirm_restore);
    gtk_box_append(GTK_BOX(box), storage_actions);
    gtk_box_append(GTK_BOX(box), library->status);
    /* Both control groups remain reachable on smaller desktops without an
     * expanding popover forcing the native application outside its monitor. */
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(outer_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(outer_scroll), 600);
    gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(outer_scroll), TRUE);
    gtk_scrolled_window_set_propagate_natural_width(GTK_SCROLLED_WINDOW(outer_scroll), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(outer_scroll), box);
    gtk_popover_set_child(GTK_POPOVER(library->popover), outer_scroll);
    (void)umi_gtk4_automation_tag_widget(library->popover, "workstation.layout-library");
    (void)umi_gtk4_automation_tag_widget(library->list, "workstation.layout-library.list");
    (void)umi_gtk4_automation_tag_widget(library->search, "workstation.layout-library.search");
    (void)umi_gtk4_automation_tag_widget(library->selected_label, "workstation.layout-library.selection");
    (void)umi_gtk4_automation_tag_widget(library->new_id, "workstation.layout-library.new-id");
    (void)umi_gtk4_automation_tag_widget(library->name, "workstation.layout-library.name");
    (void)umi_gtk4_automation_tag_widget(library->confirm, "workstation.layout-library.confirm-remove");
    (void)umi_gtk4_automation_tag_widget(library->status, "workstation.layout-library.status");
    (void)umi_gtk4_automation_tag_widget(library->refresh, "workstation.layout-library.refresh");
    (void)umi_gtk4_automation_tag_widget(library->notice, "workstation.layout-library.notice");
    (void)umi_gtk4_automation_tag_widget(library->storage_status, "workstation.layout-library.storage-status");
    (void)umi_gtk4_automation_tag_widget(library->save_library, "workstation.layout-library.save-library");
    (void)umi_gtk4_automation_tag_widget(library->restore_library, "workstation.layout-library.restore-library");
    (void)umi_gtk4_automation_tag_widget(library->confirm_restore, "workstation.layout-library.confirm-restore");
    g_signal_connect(library->list, "row-selected", G_CALLBACK(on_row_selected), library);
    g_signal_connect(library->search, "search-changed", G_CALLBACK(on_search_changed), library);
    g_signal_connect(library->new_id, "changed", G_CALLBACK(on_input_changed), library);
    g_signal_connect(library->name, "changed", G_CALLBACK(on_input_changed), library);
    g_signal_connect(library->confirm, "toggled", G_CALLBACK(on_input_changed), library);
    g_signal_connect(library->refresh, "clicked", G_CALLBACK(on_refresh_clicked), library);
    g_signal_connect(library->confirm_restore, "toggled", G_CALLBACK(on_input_changed), library);
    g_signal_connect(library->save_library, "clicked", G_CALLBACK(on_storage_clicked), library);
    g_signal_connect(library->restore_library, "clicked", G_CALLBACK(on_storage_clicked), library);
    g_signal_connect(library->popover, "notify::visible", G_CALLBACK(on_popover_visible), library);
    status = refresh_view(library);
    if (status == UMI_STATUS_OK && !library->destroy_requested) *out_library = library;
    else umi_gtk4_ws_layout_library_destroy(library);
    (void)finish_operation(library);
    return status;
}
