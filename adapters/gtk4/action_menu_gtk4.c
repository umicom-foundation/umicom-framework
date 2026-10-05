/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/action_menu_gtk4.c
 * PURPOSE: Own searchable GTK action groups while leaving command dispatch in the original buttons.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/action_menu.h"
#include <string.h>
typedef struct ActionFilter
{
    gchar *query;
} ActionFilter;
typedef struct ActionMenu
{
    GtkWidget *popover, *search, *list, *empty;
    ActionFilter *filter;
    guint pending;
    size_t count, visible;
    int busy;
} ActionMenu;
typedef struct ActionMenuWake
{
    GWeakRef menu;
} ActionMenuWake;
static int ActionMenuText(const char *text, size_t limit, int required)
{
    if (text == NULL)
        return !required;
    size_t bytes = 0U;
    while (bytes <= limit && text[bytes] != '\0')
        ++bytes;
    return bytes <= limit && (!required || bytes != 0U) && g_utf8_validate(text, (gssize)bytes, NULL);
}
static gchar *ActionMenuFold(const char *text)
{
    /* Normalize compatibility forms before folding case, then compose any
     * combining characters introduced by folding. Both labels and queries
     * use this path so their written form does not change search behavior. */
    gchar *canonical = g_utf8_normalize(text, -1, G_NORMALIZE_ALL_COMPOSE);
    gchar *folded = g_utf8_casefold(canonical, -1),
          *normalized = g_utf8_normalize(folded, -1, G_NORMALIZE_ALL_COMPOSE);
    g_free(canonical);
    g_free(folded);
    return normalized;
}
static ActionMenu *ActionMenuState(GtkWidget *menu)
{
    if (!GTK_IS_MENU_BUTTON(menu))
        return NULL;
    ActionMenu *state = g_object_get_data(G_OBJECT(menu), "umicom-action-menu");
    return state != NULL && gtk_menu_button_get_popover(GTK_MENU_BUTTON(menu)) == GTK_POPOVER(state->popover)
               ? state
               : NULL;
}
static void ActionFilterFree(gpointer data)
{
    ActionFilter *filter = data;
    g_free(filter->query);
}
static void ActionFilterRelease(gpointer data) { g_rc_box_release_full(data, ActionFilterFree); }
static gboolean ActionMenuMatches(GtkListBoxRow *row, gpointer data)
{
    const ActionFilter *filter = data;
    const char *terms = g_object_get_data(G_OBJECT(row), "umicom-action-search");
    GtkWidget *button = gtk_list_box_row_get_child(row);
    return button != NULL && gtk_widget_get_visible(button) && terms != NULL &&
           (filter->query[0] == '\0' || strstr(terms, filter->query) != NULL);
}
static void ActionMenuWakeFree(gpointer data)
{
    ActionMenuWake *wake = data;
    g_weak_ref_clear(&wake->menu);
    g_free(wake);
}
static void ActionMenuFree(gpointer data)
{
    ActionMenu *state = data;
    if (state->pending != 0U)
        g_source_remove(state->pending);
    g_clear_object(&state->popover);
    g_clear_object(&state->search);
    g_clear_object(&state->list);
    g_clear_object(&state->empty);
    /* A host may retain the list widget for inspection. Its filter context
     * has an independent reference and remains valid until that list dies. */
    ActionFilterRelease(state->filter);
    g_free(state);
}
static gboolean ActionMenuFilterIdle(gpointer data)
{
    ActionMenuWake *wake = data;
    GtkWidget *menu = g_weak_ref_get(&wake->menu);
    if (menu == NULL)
        return G_SOURCE_REMOVE;
    ActionMenu *state = ActionMenuState(menu);
    if (state == NULL)
    {
        /* Retiring the public popover does not remove its private owner data.
         * Clear the completed source ID so finalization never removes it twice. */
        ActionMenu *retired = g_object_get_data(G_OBJECT(menu), "umicom-action-menu");
        if (retired != NULL)
            retired->pending = 0U;
        g_object_unref(menu);
        return G_SOURCE_REMOVE;
    }
    if (state->busy)
    {
        g_object_unref(menu);
        return G_SOURCE_CONTINUE;
    }
    state->pending = 0U;
    state->busy = 1;
    gchar *query = ActionMenuFold(gtk_editable_get_text(GTK_EDITABLE(state->search)));
    g_free(state->filter->query);
    state->filter->query = query;
    gtk_list_box_invalidate_filter(GTK_LIST_BOX(state->list));
    size_t visible = 0U;
    for (GtkWidget *row = gtk_widget_get_first_child(state->list); row != NULL;
         row = gtk_widget_get_next_sibling(row))
    {
        if (!GTK_IS_LIST_BOX_ROW(row))
            continue;
        GtkWidget *button = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row));
        if (button != NULL && gtk_widget_get_visible(button) &&
            ActionMenuMatches(GTK_LIST_BOX_ROW(row), state->filter))
            ++visible;
    }
    state->visible = visible;
    state->busy = 0;
    g_object_unref(menu);
    return G_SOURCE_REMOVE;
}
static void ActionMenuQueue(GtkWidget *menu)
{
    ActionMenu *state = ActionMenuState(menu);
    if (state == NULL || state->pending != 0U)
        return;
    /* Retained search controls must not keep a retired menu or its host alive.
     * Coalescing input also avoids refiltering from inside a native callback. */
    ActionMenuWake *wake = g_new0(ActionMenuWake, 1);
    g_weak_ref_init(&wake->menu, menu);
    state->pending = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, ActionMenuFilterIdle, wake, ActionMenuWakeFree);
}
static void ActionMenuChanged(GtkEditable *editable, gpointer data)
{
    (void)editable;
    ActionMenuQueue(data);
}
static void ActionMenuButtonVisibility(GObject *button, GParamSpec *property, gpointer data)
{
    (void)button;
    (void)property;
    ActionMenuQueue(data);
}
static void ActionMenuActivated(GtkButton *button, gpointer data)
{
    (void)button;
    gtk_popover_popdown(GTK_POPOVER(data));
}
UmiStatus UmiGtk4ActionMenuCreate(const char *label, GtkWidget **out_menu)
{
    if (out_menu == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_menu = NULL;
    if (!ActionMenuText(label, 512U, 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    GtkWidget *menu = gtk_menu_button_new(), *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6),
              *scroll = gtk_scrolled_window_new();
    ActionMenu *state = g_new0(ActionMenu, 1);
    state->filter = g_rc_box_new0(ActionFilter);
    state->filter->query = g_strdup("");
    state->popover = g_object_ref_sink(gtk_popover_new());
    state->search = g_object_ref_sink(gtk_entry_new());
    state->list = g_object_ref_sink(gtk_list_box_new());
    state->empty = g_object_ref_sink(gtk_label_new("No matching actions"));
    gtk_menu_button_set_label(GTK_MENU_BUTTON(menu), label);
    gtk_entry_set_placeholder_text(GTK_ENTRY(state->search), "Filter actions");
    gchar *accessible = g_strdup_printf("Filter %s actions", label);
    gtk_accessible_update_property(GTK_ACCESSIBLE(state->search), GTK_ACCESSIBLE_PROPERTY_LABEL, accessible,
                                   -1);
    g_free(accessible);
    gtk_entry_set_max_length(GTK_ENTRY(state->search), 256);
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(state->search), GTK_ENTRY_ICON_PRIMARY, "edit-find-symbolic");
    gtk_widget_set_tooltip_text(state->search,
                                "Filter action names and keywords. Clear the field to show all actions.");
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(state->list), GTK_SELECTION_NONE);
    gtk_list_box_set_placeholder(GTK_LIST_BOX(state->list), state->empty);
    gtk_list_box_set_filter_func(GTK_LIST_BOX(state->list), ActionMenuMatches,
                                 g_rc_box_acquire(state->filter), ActionFilterRelease);
    gtk_widget_set_size_request(scroll, 300, -1);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(scroll), 360);
    gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(scroll), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), state->list);
    gtk_box_append(GTK_BOX(box), state->search);
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_popover_set_child(GTK_POPOVER(state->popover), box);
    gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu), state->popover);
    g_object_set_data_full(G_OBJECT(menu), "umicom-action-menu", state, ActionMenuFree);
    g_signal_connect_object(state->search, "changed", G_CALLBACK(ActionMenuChanged), menu, 0);
    *out_menu = menu;
    return UMI_STATUS_OK;
}
UmiStatus UmiGtk4ActionMenuAppend(GtkWidget *menu, GtkWidget *button, const char *keywords)
{
    ActionMenu *state = ActionMenuState(menu);
    if (state == NULL || !GTK_IS_BUTTON(button) || gtk_widget_get_parent(button) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (state->busy)
        return UMI_STATUS_BUSY;
    if (state->count >= 256U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *label = gtk_button_get_label(GTK_BUTTON(button));
    if (!ActionMenuText(label, 512U, 1) || !ActionMenuText(keywords, 2048U, 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    gchar *terms = g_strconcat(label, " ", keywords != NULL ? keywords : "", NULL),
          *folded = ActionMenuFold(terms);
    g_free(terms);
    g_object_ref(menu);
    g_object_ref_sink(button);
    state->busy = 1;
    GtkWidget *row = g_object_ref_sink(gtk_list_box_row_new());
    g_object_set_data_full(G_OBJECT(row), "umicom-action-search", folded, g_free);
    gtk_list_box_row_set_selectable(GTK_LIST_BOX_ROW(row), FALSE);
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), FALSE);
    gtk_widget_set_hexpand(button, TRUE);
    gtk_widget_set_halign(button, GTK_ALIGN_FILL);
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), button);
    gtk_list_box_append(GTK_LIST_BOX(state->list), row);
    ++state->count;
    g_signal_connect_object(button, "clicked", G_CALLBACK(ActionMenuActivated), state->popover, 0);
    g_signal_connect_object(button, "notify::visible", G_CALLBACK(ActionMenuButtonVisibility), menu, 0);
    state->busy = 0;
    ActionMenuQueue(menu);
    UmiStatus status = ActionMenuState(menu) == state && gtk_widget_get_parent(button) == row
                           ? UMI_STATUS_OK
                           : UMI_STATUS_INVALID_STATE;
    g_object_unref(row);
    g_object_unref(button);
    g_object_unref(menu);
    return status;
}
UmiStatus UmiGtk4ActionMenuSetFilter(GtkWidget *menu, const char *query)
{
    ActionMenu *state = ActionMenuState(menu);
    if (state == NULL || query == NULL || !ActionMenuText(query, 1024U, 0) || g_utf8_strlen(query, -1) > 256)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (state->busy)
        return UMI_STATUS_BUSY;
    gchar *owned = g_strdup(query);
    g_object_ref(menu);
    gtk_editable_set_text(GTK_EDITABLE(state->search), owned);
    UmiStatus status = ActionMenuState(menu) == state &&
                               strcmp(gtk_editable_get_text(GTK_EDITABLE(state->search)), owned) == 0
                           ? UMI_STATUS_OK
                           : UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        ActionMenuQueue(menu);
    g_free(owned);
    g_object_unref(menu);
    return status;
}
size_t UmiGtk4ActionMenuVisibleCount(GtkWidget *menu)
{
    ActionMenu *state = ActionMenuState(menu);
    return state == NULL ? 0U : state->visible;
}
GtkWidget *UmiGtk4ActionMenuFilterEntry(GtkWidget *menu)
{
    ActionMenu *state = ActionMenuState(menu);
    return state == NULL ? NULL : state->search;
}
