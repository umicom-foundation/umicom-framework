/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/quick_open_gtk4.c
 * PURPOSE: Own indexed-file presentation and retire stale rows before dispatching an explicit open.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/quick_open.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <string.h>
#define QUICK_OPEN_PAGE_SIZE 64U
typedef struct QuickOpenState
{
    GtkWindow *window; /* Borrowed: the window owns this state. */
    GtkWidget *query, *list, *summary, *openButton, *previous, *next;
    UmiGtk4QuickOpenRead read;
    UmiGtk4QuickOpenOpen open;
    gpointer data;
    GDestroyNotify destroy;
    UmiFileIndexEntry *entries;
    UmiFileIndexPage page;
    gchar *capturedQuery;
    uint64_t generation, requestedRevision;
    size_t requestedOffset;
    guint pending;
    int closed, busy, ready;
} QuickOpenState;
typedef struct QuickOpenWake
{
    GWeakRef window;
} QuickOpenWake;
typedef struct QuickOpenRow
{
    uint64_t generation;
    size_t position;
} QuickOpenRow;
static void QuickOpenRequest(QuickOpenState *state, size_t offset, uint64_t revision);
static void QuickOpenRetire(GtkWidget *window, gpointer data)
{
    (void)window;
    QuickOpenState *state = data;
    state->closed = 1;
    state->ready = 0;
    if (state->pending != 0U)
    {
        g_source_remove(state->pending);
        state->pending = 0U;
    }
}
static void QuickOpenParentClosed(GtkWidget *parent, gpointer data)
{
    (void)parent;
    QuickOpenState *state = data;
    QuickOpenRetire(GTK_WIDGET(state->window), state);
    gtk_window_destroy(state->window);
}
static void QuickOpenFree(gpointer data)
{
    QuickOpenState *state = data;
    QuickOpenRetire(NULL, state);
    if (state->destroy != NULL)
        state->destroy(state->data);
    g_clear_object(&state->query);
    g_clear_object(&state->list);
    g_clear_object(&state->summary);
    g_clear_object(&state->openButton);
    g_clear_object(&state->previous);
    g_clear_object(&state->next);
    g_free(state->entries);
    g_free(state->capturedQuery);
    g_free(state);
}
static QuickOpenState *QuickOpenOwner(gpointer window)
{
    QuickOpenState *state = g_object_get_data(G_OBJECT(window), "umicom-quick-open");
    return state != NULL && !state->closed && UmiGtk4WindowIsOpen(state->window) ? state : NULL;
}
static int QuickOpenCurrent(QuickOpenState *state, uint64_t generation)
{
    return !state->closed && state->generation == generation && UmiGtk4WindowIsOpen(state->window);
}
static int QuickOpenText(const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    return end != NULL && g_utf8_validate(text, (gssize)(end - text), NULL);
}
static int QuickOpenPageValid(const UmiFileIndexPage *page, const UmiFileIndexEntry *entries, size_t offset,
                              uint64_t revision)
{
    if (page->count > QUICK_OPEN_PAGE_SIZE || page->offset != offset || page->stats.revision == 0U ||
        (revision != 0U && page->stats.revision != revision) || page->matched > page->stats.files ||
        page->count > page->matched ||
        (page->count != 0U && (offset > page->matched || page->count > page->matched - offset)) ||
        !QuickOpenText(page->stats.root, sizeof(page->stats.root)) || !umi_path_is_absolute(page->stats.root))
        return 0;
    size_t remaining = offset < page->matched ? page->matched - offset : 0U;
    if (page->count != (remaining < QUICK_OPEN_PAGE_SIZE ? remaining : QUICK_OPEN_PAGE_SIZE))
        return 0;
    int more = offset < page->matched && page->count < page->matched - offset;
    if (!!page->has_more != more || (more && page->count == 0U))
        return 0;
    for (size_t i = 0U; i < page->count; ++i)
        if (!QuickOpenText(entries[i].path, sizeof(entries[i].path)) ||
            !QuickOpenText(entries[i].relative_path, sizeof(entries[i].relative_path)) ||
            !QuickOpenText(entries[i].name, sizeof(entries[i].name)) ||
            !QuickOpenText(entries[i].extension, sizeof(entries[i].extension)) ||
            !umi_path_is_absolute(entries[i].path) || entries[i].relative_path[0] == '\0')
            return 0;
    return 1;
}
static void QuickOpenMessage(QuickOpenState *state, UmiStatus status)
{
    gchar *text = g_strdup_printf(
        "Files are not ready: %s. Refresh results or reopen Quick Open for the current workspace.",
        umi_status_text(status));
    gtk_label_set_text(GTK_LABEL(state->summary), text);
    g_free(text);
}
static void QuickOpenOpenPosition(QuickOpenState *state, size_t position)
{
    if (state->closed || state->busy || !state->ready || position >= state->page.count)
        return;
    GtkWindow *window = g_object_ref(state->window);
    uint64_t generation = state->generation;
    /* The host may cause selection or window callbacks while opening. Give
     * it copies, so a new query cannot replace the chosen identity mid-call. */
    UmiFileIndexEntry *entry = g_memdup2(&state->entries[position], sizeof(*entry));
    UmiFileIndexPage *page = g_memdup2(&state->page, sizeof(*page));
    gchar *query = g_strdup(state->capturedQuery);
    state->busy = 1;
    UmiStatus status = state->open(state->data, query, page, position, entry);
    g_free(query);
    g_free(page);
    g_free(entry);
    state->busy = 0;
    if (QuickOpenCurrent(state, generation))
    {
        if (status == UMI_STATUS_OK)
            gtk_window_destroy(window);
        else
        {
            state->ready = 0;
            gtk_widget_set_sensitive(state->openButton, FALSE);
            QuickOpenMessage(state, status);
        }
    }
    g_object_unref(window);
}
static void QuickOpenSelected(GtkListBox *list, GtkListBoxRow *row, gpointer data)
{
    (void)list;
    QuickOpenState *state = QuickOpenOwner(data);
    if (state == NULL)
        return;
    const QuickOpenRow *choice =
        row != NULL ? g_object_get_data(G_OBJECT(row), "umicom-quick-open-row") : NULL;
    gtk_widget_set_sensitive(state->openButton, state->ready && choice != NULL &&
                                                    choice->generation == state->generation &&
                                                    choice->position < state->page.count);
}
static void QuickOpenActivated(GtkListBox *list, GtkListBoxRow *row, gpointer data)
{
    (void)list;
    QuickOpenState *state = QuickOpenOwner(data);
    const QuickOpenRow *choice =
        row != NULL ? g_object_get_data(G_OBJECT(row), "umicom-quick-open-row") : NULL;
    if (state != NULL && choice != NULL && choice->generation == state->generation)
        QuickOpenOpenPosition(state, choice->position);
}
static void QuickOpenChosen(GtkWidget *button, gpointer data)
{
    (void)button;
    QuickOpenState *state = QuickOpenOwner(data);
    if (state != NULL)
        QuickOpenActivated(GTK_LIST_BOX(state->list),
                           gtk_list_box_get_selected_row(GTK_LIST_BOX(state->list)), data);
}
static void QuickOpenWakeFree(gpointer data)
{
    QuickOpenWake *wake = data;
    g_weak_ref_clear(&wake->window);
    g_free(wake);
}
static gboolean QuickOpenLoad(gpointer data)
{
    QuickOpenWake *wake = data;
    GtkWindow *window = g_weak_ref_get(&wake->window);
    if (window == NULL)
        return G_SOURCE_REMOVE;
    QuickOpenState *state = QuickOpenOwner(window);
    if (state == NULL)
    {
        g_object_unref(window);
        return G_SOURCE_REMOVE;
    }
    if (state->busy)
    {
        g_object_unref(window);
        return G_SOURCE_CONTINUE;
    }
    state->pending = 0U;
    state->busy = 1;
    uint64_t generation = state->generation, revision = state->requestedRevision;
    size_t offset = state->requestedOffset;
    gchar *query = g_strdup(gtk_editable_get_text(GTK_EDITABLE(state->query)));
    UmiFileIndexEntry *entries = g_new0(UmiFileIndexEntry, QUICK_OPEN_PAGE_SIZE);
    UmiFileIndexPage page = {0};
    UmiStatus status = strlen(query) <= 1024U ? state->read(state->data, query, offset, revision, entries,
                                                            QUICK_OPEN_PAGE_SIZE, &page)
                                              : UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && !QuickOpenPageValid(&page, entries, offset, revision))
        status = UMI_STATUS_INVALID_STATE;
    /* Build labels from complete copied records; never parse a displayed path
     * back into a filename. No result is actionable until the whole page is
     * published under the same generation as its query. */
    if (QuickOpenCurrent(state, generation))
    {
        GtkWidget *row;
        while ((row = gtk_widget_get_first_child(state->list)) != NULL && QuickOpenCurrent(state, generation))
            gtk_list_box_remove(GTK_LIST_BOX(state->list), row);
        if (status == UMI_STATUS_OK)
        {
            for (size_t i = 0U; i < page.count && QuickOpenCurrent(state, generation); ++i)
            {
                GtkWidget *item = gtk_list_box_row_new(), *label = gtk_label_new(entries[i].relative_path);
                gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
                gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_MIDDLE);
                gtk_label_set_max_width_chars(GTK_LABEL(label), 90);
                gtk_widget_set_tooltip_text(item, entries[i].path);
                gtk_widget_set_margin_top(label, 6);
                gtk_widget_set_margin_bottom(label, 6);
                QuickOpenRow *choice = g_new(QuickOpenRow, 1);
                choice->generation = generation;
                choice->position = i;
                g_object_set_data_full(G_OBJECT(item), "umicom-quick-open-row", choice, g_free);
                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(item), label);
                gtk_list_box_append(GTK_LIST_BOX(state->list), item);
            }
            if (QuickOpenCurrent(state, generation))
            {
                memcpy(state->entries, entries, page.count * sizeof(*entries));
                state->page = page;
                g_free(state->capturedQuery);
                state->capturedQuery = g_strdup(query);
                state->ready = 1;
                gchar *text =
                    page.count != 0U
                        ? g_strdup_printf("Files %zu–%zu of %zu in %s", offset + 1U, offset + page.count,
                                          page.matched, page.stats.root)
                        : g_strdup_printf(
                              "No indexed files match in %s. Refresh Explorer to discover new files.",
                              page.stats.root);
                gtk_label_set_text(GTK_LABEL(state->summary), text);
                g_free(text);
                if (QuickOpenCurrent(state, generation))
                    gtk_widget_set_sensitive(state->previous, offset != 0U);
                if (QuickOpenCurrent(state, generation))
                    gtk_widget_set_sensitive(state->next, page.has_more);
                if (QuickOpenCurrent(state, generation))
                    gtk_list_box_select_row(GTK_LIST_BOX(state->list),
                                            page.count != 0U
                                                ? gtk_list_box_get_row_at_index(GTK_LIST_BOX(state->list), 0)
                                                : NULL);
            }
        }
        else if (QuickOpenCurrent(state, generation))
            QuickOpenMessage(state, status);
    }
    state->busy = 0;
    g_free(entries);
    g_free(query);
    g_object_unref(window);
    return G_SOURCE_REMOVE;
}
static void QuickOpenRequest(QuickOpenState *state, size_t offset, uint64_t revision)
{
    if (state->closed)
        return;
    state->ready = 0;
    if (state->generation == UINT64_MAX)
    {
        QuickOpenMessage(state, UMI_STATUS_CAPACITY_EXCEEDED);
        return;
    }
    ++state->generation;
    state->requestedOffset = offset;
    state->requestedRevision = revision;
    /* Retire first. Sensitivity notifications can synchronously call host
     * code, but old rows can no longer open a file even when retained. */
    gtk_widget_set_sensitive(state->openButton, FALSE);
    gtk_widget_set_sensitive(state->previous, FALSE);
    gtk_widget_set_sensitive(state->next, FALSE);
    if (!state->closed && state->pending == 0U)
    {
        QuickOpenWake *wake = g_new0(QuickOpenWake, 1);
        g_weak_ref_init(&wake->window, state->window);
        state->pending = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, QuickOpenLoad, wake, QuickOpenWakeFree);
    }
}
static void QuickOpenChanged(GtkEditable *entry, gpointer data)
{
    (void)entry;
    QuickOpenState *state = QuickOpenOwner(data);
    if (state != NULL)
        QuickOpenRequest(state, 0U, 0U);
}
static void QuickOpenRefresh(GtkButton *button, gpointer data)
{
    (void)button;
    QuickOpenState *state = QuickOpenOwner(data);
    if (state != NULL)
        QuickOpenRequest(state, 0U, 0U);
}
static void QuickOpenPage(GtkButton *button, gpointer data)
{
    QuickOpenState *state = QuickOpenOwner(data);
    if (state == NULL || !state->ready || state->busy)
        return;
    size_t offset = state->page.offset;
    if (GTK_WIDGET(button) == state->next)
    {
        if (!state->page.has_more)
            return;
        offset += state->page.count;
    }
    else
    {
        if (offset == 0U)
            return;
        offset = offset >= QUICK_OPEN_PAGE_SIZE ? offset - QUICK_OPEN_PAGE_SIZE : 0U;
    }
    QuickOpenRequest(state, offset, state->page.stats.revision);
}
static gboolean QuickOpenKey(GtkEventControllerKey *controller, guint keyval, guint keycode,
                             GdkModifierType modifiers, gpointer data)
{
    (void)controller;
    (void)keycode;
    QuickOpenState *state = QuickOpenOwner(data);
    if (state == NULL || (modifiers & (GDK_CONTROL_MASK | GDK_ALT_MASK | GDK_SUPER_MASK)) != 0U)
        return FALSE;
    if (keyval == GDK_KEY_Escape)
    {
        gtk_window_destroy(state->window);
        return TRUE;
    }
    if (keyval == GDK_KEY_Down &&
        (gtk_root_get_focus(GTK_ROOT(state->window)) == state->query ||
         (gtk_root_get_focus(GTK_ROOT(state->window)) != NULL &&
          gtk_widget_is_ancestor(gtk_root_get_focus(GTK_ROOT(state->window)), state->query))) &&
        state->ready && state->page.count != 0U)
    {
        GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(state->list));
        if (row == NULL)
            row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(state->list), 0);
        if (row != NULL)
            gtk_widget_grab_focus(GTK_WIDGET(row));
        return row != NULL;
    }
    return FALSE;
}
static GtkWidget *QuickOpenControl(GtkWidget *widget, const char *id)
{
    g_object_set_data(G_OBJECT(widget), "umicom-automation-id", (gpointer)id);
    return widget;
}
UmiStatus UmiGtk4QuickOpenCreate(GtkWindow *parent, UmiGtk4QuickOpenRead read, UmiGtk4QuickOpenOpen open,
                                 gpointer userData, GDestroyNotify destroyData, GtkWindow **outWindow)
{
    if (outWindow == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outWindow = NULL;
    if (!GTK_IS_WINDOW(parent) || !UmiGtk4WindowIsOpen(parent) || read == NULL || open == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    GtkWindow *window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    QuickOpenState *state = g_new0(QuickOpenState, 1);
    state->window = window;
    state->read = read;
    state->open = open;
    state->data = userData;
    state->destroy = destroyData;
    state->entries = g_new0(UmiFileIndexEntry, QUICK_OPEN_PAGE_SIZE);
    state->query = g_object_ref_sink(QuickOpenControl(gtk_entry_new(), "umicom-quick-open-query"));
    state->list = g_object_ref_sink(QuickOpenControl(gtk_list_box_new(), "umicom-quick-open-results"));
    state->summary = g_object_ref_sink(
        QuickOpenControl(gtk_label_new("Reading indexed filenames…"), "umicom-quick-open-status"));
    state->openButton = g_object_ref_sink(
        QuickOpenControl(gtk_button_new_with_label("Open selected"), "umicom-quick-open-open"));
    state->previous = g_object_ref_sink(
        QuickOpenControl(gtk_button_new_with_label("Previous"), "umicom-quick-open-previous"));
    state->next =
        g_object_ref_sink(QuickOpenControl(gtk_button_new_with_label("Next"), "umicom-quick-open-next"));
    GtkWidget *refresh =
        QuickOpenControl(gtk_button_new_with_label("Refresh results"), "umicom-quick-open-refresh");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8),
              *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6), *scroll = gtk_scrolled_window_new();
    gtk_window_set_title(window, "Quick Open");
    gtk_window_set_default_size(window, 680, 480);
    gtk_window_set_transient_for(window, parent);
    gtk_window_set_destroy_with_parent(window, TRUE);
    gtk_entry_set_max_length(GTK_ENTRY(state->query), 256);
    gtk_entry_set_placeholder_text(GTK_ENTRY(state->query), "Filter indexed filenames or relative paths");
    gtk_accessible_update_property(GTK_ACCESSIBLE(state->query), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Find an indexed file", -1);
    gtk_widget_set_tooltip_text(state->query, "Literal path search; ASCII letters ignore case. Enter opens "
                                              "the selected visible result after filtering completes.");
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(state->list), GTK_SELECTION_SINGLE);
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(state->list), FALSE);
    gtk_label_set_wrap(GTK_LABEL(state->summary), TRUE);
    gtk_label_set_xalign(GTK_LABEL(state->summary), 0.0F);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), state->list);
    gtk_box_append(GTK_BOX(box), state->query);
    gtk_box_append(GTK_BOX(box), state->summary);
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_box_append(GTK_BOX(bar), state->previous);
    gtk_box_append(GTK_BOX(bar), state->next);
    gtk_box_append(GTK_BOX(bar), refresh);
    gtk_box_append(GTK_BOX(bar), state->openButton);
    gtk_box_append(GTK_BOX(box), bar);
    gtk_window_set_child(window, box);
    g_object_set_data_full(G_OBJECT(window), "umicom-quick-open", state, QuickOpenFree);
    g_signal_connect_object(state->query, "changed", G_CALLBACK(QuickOpenChanged), window, 0);
    g_signal_connect_object(state->query, "activate", G_CALLBACK(QuickOpenChosen), window, 0);
    g_signal_connect_object(state->list, "row-selected", G_CALLBACK(QuickOpenSelected), window, 0);
    g_signal_connect_object(state->list, "row-activated", G_CALLBACK(QuickOpenActivated), window, 0);
    g_signal_connect_object(state->openButton, "clicked", G_CALLBACK(QuickOpenChosen), window, 0);
    g_signal_connect_object(state->previous, "clicked", G_CALLBACK(QuickOpenPage), window, 0);
    g_signal_connect_object(state->next, "clicked", G_CALLBACK(QuickOpenPage), window, 0);
    g_signal_connect_object(refresh, "clicked", G_CALLBACK(QuickOpenRefresh), window, 0);
    GtkEventController *keys = gtk_event_controller_key_new();
    gtk_event_controller_set_name(keys, "umicom-quick-open-keys");
    gtk_event_controller_set_propagation_phase(keys, GTK_PHASE_CAPTURE);
    g_signal_connect_object(keys, "key-pressed", G_CALLBACK(QuickOpenKey), window, 0);
    gtk_widget_add_controller(GTK_WIDGET(window), keys);
    (void)UmiGtk4WatchWindowClosed(window, G_OBJECT(window), QuickOpenRetire, state);
    (void)UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), QuickOpenParentClosed, state);
    QuickOpenRequest(state, 0U, 0U);
    *outWindow = window;
    return UMI_STATUS_OK;
}
