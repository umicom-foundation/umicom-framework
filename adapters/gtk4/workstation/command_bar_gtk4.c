/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/command_bar_gtk4.c
 *
 * PURPOSE:
 *   Render the portable workstation command catalogue as a responsive GTK4
 *   search component shared by Studio, Trader and other suite applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/command_bar.h"
#include "umicom/ui/gtk4/automation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_GTK4_COMMAND_BAR_DEFAULT_RESULTS 10U

struct UmiGtk4WorkstationCommandBar {
    UmiWsCommandBarModel model;
    GtkWidget *root;
    GtkWidget *scope_label;
    GtkWidget *entry;
    GtkWidget *result_button;
    GtkWidget *popover;
    GtkWidget *result_list;
    /* The controller owns navigation callbacks; widgets own the GTK objects. */
    GtkWidget *result_status;
    GtkWidget *previous_page;
    GtkWidget *next_page;
    GtkEventController *entry_keys;
    GtkEventController *popover_keys;
    GtkEventController *focus_watch;
    UmiStatus search_status;
    bool replacing_results;
    UmiGtk4WorkstationCommandBarActivatedHandler activated_handler;
    void *activated_user_data;
    char placeholder[UMI_UI_TEXT_CAPACITY];
    char compact_placeholder[UMI_UI_TEXT_CAPACITY];
    size_t maximum_visible_results;
    int changing_text;
    uint64_t revision;
};

/* Public models are ordinary C values and may come from a plug-in boundary.
 * Check every count and fixed string before a renderer reads the value. */
static bool model_is_safe(const UmiWsCommandBarModel *model)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || model->count > UMI_WS_MAX_PALETTE_ITEMS ||
        memchr(model->query.text, '\0', sizeof(model->query.text)) == NULL ||
        model->query.scope < UMI_WS_COMMAND_SCOPE_ALL ||
        model->query.scope > UMI_WS_COMMAND_SCOPE_AI ||
        model->presentation < UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED ||
        model->presentation > UMI_WS_COMMAND_BAR_PRESENTATION_BUTTON) {
        return false;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        const UmiWsCommandBarItem *item = &model->items[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_ws_id_valid(item->item_id) ||
            !umi_ws_id_valid(item->command_id) ||
            memchr(item->title, '\0', sizeof(item->title)) == NULL ||
            memchr(item->description, '\0', sizeof(item->description)) == NULL ||
            memchr(item->keywords, '\0', sizeof(item->keywords)) == NULL ||
            item->scope < UMI_WS_COMMAND_SCOPE_ALL ||
            item->scope > UMI_WS_COMMAND_SCOPE_AI) {
            return false;
        }
    }
    return true;
}

/* Rebuild derived result indices after copying a model. Keeping a caller's
 * current query makes an availability refresh invisible to someone typing. */
static UmiStatus copy_model_with_query(
    UmiWsCommandBarModel *destination,
    const UmiWsCommandBarModel *source,
    const UmiWsCommandBarQuery *query)
{
    char input[UMI_UI_TEXT_CAPACITY + 2U];
    char prefix;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || !model_is_safe(source) || query == NULL ||
        memchr(query->text, '\0', sizeof(query->text)) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    prefix = umi_ws_command_bar_scope_prefix(query->scope);
    written = prefix == '\0'
        ? snprintf(input, sizeof(input), "%s", query->text)
        : snprintf(input, sizeof(input), "%c%s", prefix, query->text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(input)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *destination = *source;
    return umi_ws_command_bar_model_set_query(destination, input);
}

/* Widgets created before parenting still carry a floating reference. This
 * helper releases each successful allocation on an uncommon partial failure. */
static void release_unparented_widget(GtkWidget *widget)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (widget == NULL) return;
    g_object_ref_sink(widget);
    g_object_unref(widget);
}

/* Return a short label that explains how the current prefix narrows results. */
static const char *scope_text(UmiWsCommandScope scope)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (scope) {
    case UMI_WS_COMMAND_SCOPE_COMMAND: return "Commands";
    case UMI_WS_COMMAND_SCOPE_SYMBOL: return "Symbols";
    case UMI_WS_COMMAND_SCOPE_TEXT: return "Text";
    case UMI_WS_COMMAND_SCOPE_LINE: return "Lines";
    case UMI_WS_COMMAND_SCOPE_SETTING: return "Settings";
    case UMI_WS_COMMAND_SCOPE_PANEL: return "Windows";
    case UMI_WS_COMMAND_SCOPE_AI: return "Assistant";
    default: return "All";
    }
}

/* Remove old rows before a new query is projected. GTK owns every child after
 * append, so removing it from the list also releases the old row safely. */
/* Retained rows and entry widgets must not keep callbacks into a released or
 * replaced controller. GTK's own signal handlers have different user data. */
static void disconnect_command_bar_widgets(
    GtkWidget *widget, UmiGtk4WorkstationCommandBar *command_bar)
{
    GtkWidget *child;
    if (widget == NULL) return;
    g_signal_handlers_disconnect_by_data(widget, command_bar);
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        disconnect_command_bar_widgets(child, command_bar);
}

/* Disconnect each old result before removing its parent-owned reference. */
static void clear_result_list(GtkWidget *list, UmiGtk4WorkstationCommandBar *command_bar)
{
    GtkWidget *child;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) return;
    child = gtk_widget_get_first_child(list);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        disconnect_command_bar_widgets(child, command_bar);
        gtk_list_box_remove(GTK_LIST_BOX(list), child);
        child = next;
    }
}

/* Find an item by copied identifier instead of storing a pointer in a button.
 * This remains safe when an application replaces the whole catalogue model. */
static const UmiWsCommandBarItem *find_item(
    const UmiGtk4WorkstationCommandBar *command_bar,
    const char *item_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL || item_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < command_bar->model.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(command_bar->model.items[index].item_id, item_id) == 0) {
            return &command_bar->model.items[index];
        }
    }
    return NULL;
}

/* Copy dispatch data before closing the popover. The owner may replace the
 * model or destroy this component; no controller state is read afterwards. */
static void activate_item(UmiGtk4WorkstationCommandBar *command_bar,
                          const UmiWsCommandBarItem *item)
{
    UmiWsCommandBarItem copied;
    UmiGtk4WorkstationCommandBarActivatedHandler handler;
    void *context;
    if (command_bar == NULL || item == NULL || !item->enabled) return;
    /* A rejected edit must never activate results from an older accepted query. */
    if (command_bar->search_status != UMI_STATUS_OK) return;
    copied = *item;
    handler = command_bar->activated_handler;
    context = command_bar->activated_user_data;
    gtk_popover_popdown(GTK_POPOVER(command_bar->popover));
    if (handler != NULL) handler(&copied, context);
}

/* Dispatch only enabled entries. Disabled entries remain visible so a user
 * can discover an action even when its current context prevents execution. */
static void on_result_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar =
        (UmiGtk4WorkstationCommandBar *)user_data;
    const char *item_id;
    const UmiWsCommandBarItem *item;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL || button == NULL) return;
    item_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-command-bar-item-id");
    item = find_item(command_bar, item_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || !item->enabled) return;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    activate_item(command_bar, item);
}

/* Project portable results into native rows. Only a bounded number is rendered
 * while the model continues to report the complete number of matches. */
/* This original projection is preserved for review. The active implementation
 * below renders the selected page and hides stale results after invalid input;
 * the earlier first-page-only behaviour is no longer the UI authority. */
#if 0
static void rebuild_result_widgets(
    UmiGtk4WorkstationCommandBar *command_bar)
{
    size_t limit;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL || command_bar->result_list == NULL) return;
    clear_result_list(command_bar->result_list, command_bar);
    gtk_label_set_text(
        GTK_LABEL(command_bar->scope_label),
        scope_text(command_bar->model.query.scope));

    limit = command_bar->model.result_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit > command_bar->maximum_visible_results) {
        limit = command_bar->maximum_visible_results;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < limit; ++index) {
        const UmiWsCommandBarItem *item =
            umi_ws_command_bar_model_result_at(&command_bar->model, index);
        GtkWidget *button;
        GtkWidget *content;
        GtkWidget *title;
        GtkWidget *description;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (item == NULL) continue;
        button = gtk_button_new();
        content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        title = gtk_label_new(item->title);
        description = gtk_label_new(item->description);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (button == NULL || content == NULL || title == NULL ||
            description == NULL) {
            continue;
        }

        gtk_widget_add_css_class(button, "umicom-command-bar-result");
        gtk_widget_add_css_class(description, "dim-label");
        gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
        gtk_label_set_ellipsize(
            GTK_LABEL(description), PANGO_ELLIPSIZE_END);
        gtk_box_append(GTK_BOX(content), title);
        gtk_box_append(GTK_BOX(content), description);
        gtk_button_set_child(GTK_BUTTON(button), content);
        gtk_widget_set_sensitive(button, item->enabled);
        gtk_widget_set_tooltip_text(button, item->description);
        g_object_set_data_full(
            G_OBJECT(button),
            "umicom-command-bar-item-id",
            g_strdup(item->item_id),
            g_free);
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(on_result_clicked),
            command_bar);
        gtk_list_box_append(GTK_LIST_BOX(command_bar->result_list), button);
    }
    ++command_bar->revision;
}
#endif

static void focus_selected_result(UmiGtk4WorkstationCommandBar *command_bar);

static void rebuild_result_widgets(
    UmiGtk4WorkstationCommandBar *command_bar)
{
    size_t limit;
    GtkRoot *root;
    GtkWidget *focused;
    bool restore_result_focus;
    UmiWsCommandBarPage page = {0};
    char status_text[128];
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL || command_bar->result_list == NULL) return;
    /* Removing a focused old row is an internal refresh, not a user leaving
     * search. Restore native focus to the selected replacement afterwards. */
    root = gtk_widget_get_root(command_bar->root);
    focused = root != NULL ? gtk_root_get_focus(root) : NULL;
    restore_result_focus = focused != NULL &&
        gtk_widget_is_ancestor(focused, command_bar->result_list);
    command_bar->replacing_results = true;
    clear_result_list(command_bar->result_list, command_bar);
    gtk_label_set_text(
        GTK_LABEL(command_bar->scope_label),
        scope_text(command_bar->model.query.scope));

    /* The former first-page limit is retained for review. The portable page
     * calculation below reveals whichever result keyboard navigation selects. */
#if 0
    limit = command_bar->model.result_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit > command_bar->maximum_visible_results) {
        limit = command_bar->maximum_visible_results;
    }
#endif
    (void)umi_gtk4_ws_command_bar_page(command_bar, &page);
    limit = page.first_result + page.result_count;
    gtk_widget_set_sensitive(command_bar->previous_page, page.has_previous);
    gtk_widget_set_sensitive(command_bar->next_page, page.has_next);
    if (command_bar->search_status != UMI_STATUS_OK)
        (void)snprintf(status_text, sizeof(status_text), "Search text is too long. Shorten it to continue.");
    else if (page.total_results == 0U)
        (void)snprintf(status_text, sizeof(status_text), "No matching commands.");
    else
        (void)snprintf(status_text, sizeof(status_text), "%zu-%zu of %zu matches",
            page.first_result + 1U, limit, page.total_results);
    gtk_label_set_text(GTK_LABEL(command_bar->result_status), status_text);
    /* Visit each bounded item once so every record receives the same rule. */
    /* Each rendered button carries its absolute result index for accessibility
     * and testing; pages never renumber commands or change their stable IDs. */
    for (index = page.first_result; index < limit; ++index) {
        const UmiWsCommandBarItem *item =
            umi_ws_command_bar_model_result_at(&command_bar->model, index);
        GtkWidget *button;
        GtkWidget *content;
        GtkWidget *title;
        GtkWidget *description;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (item == NULL) continue;
        button = gtk_button_new();
        content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        title = gtk_label_new(item->title);
        description = gtk_label_new(item->description);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (button == NULL || content == NULL || title == NULL ||
            description == NULL) {
            continue;
        }

        gtk_widget_add_css_class(button, "umicom-command-bar-result");
        if (index == command_bar->model.selected_result)
            gtk_widget_add_css_class(button, "suggested-action");
        g_object_set_data(G_OBJECT(button), "umicom-command-result-index",
            GSIZE_TO_POINTER(index + 1U));
        gtk_widget_add_css_class(description, "dim-label");
        gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
        gtk_label_set_ellipsize(
            GTK_LABEL(description), PANGO_ELLIPSIZE_END);
        gtk_box_append(GTK_BOX(content), title);
        gtk_box_append(GTK_BOX(content), description);
        gtk_button_set_child(GTK_BUTTON(button), content);
        gtk_widget_set_sensitive(button, item->enabled);
        gtk_widget_set_tooltip_text(button, item->description);
        g_object_set_data_full(
            G_OBJECT(button),
            "umicom-command-bar-item-id",
            g_strdup(item->item_id),
            g_free);
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(on_result_clicked),
            command_bar);
        gtk_list_box_append(GTK_LIST_BOX(command_bar->result_list), button);
    }
    ++command_bar->revision;
    command_bar->replacing_results = false;
    if (restore_result_focus) focus_selected_result(command_bar);
}

/* The delayed search and first-result activation are retained for review.
 * Synchronous validation and shared selection below replace them, preventing
 * stale actions and making results beyond the first page reachable. */
#if 0
/* Re-run portable filtering whenever keyboard input changes. The results
 * button owns the popover, keeping it anchored to the header during resize. */
static void on_search_changed(GtkSearchEntry *entry, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar =
        (UmiGtk4WorkstationCommandBar *)user_data;
    const char *text;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL || command_bar->changing_text) return;
    text = gtk_editable_get_text(GTK_EDITABLE(entry));
    (void)umi_ws_command_bar_model_set_query(
        &command_bar->model,
        text);
    rebuild_result_widgets(command_bar);
    /* Suggestions follow non-empty typing and disappear when the query is
     * cleared. The result button can still open the complete catalogue. */
    if (text[0] != '\0' && gtk_widget_get_mapped(command_bar->root)) {
        gtk_popover_popup(GTK_POPOVER(command_bar->popover));
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        gtk_popover_popdown(GTK_POPOVER(command_bar->popover));
    }
}

/* Enter activates the highest-priority matching action. Every visible result
 * remains clickable when the user needs a different action with similar text. */
static void on_search_activate(GtkSearchEntry *entry, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar =
        (UmiGtk4WorkstationCommandBar *)user_data;
    const UmiWsCommandBarItem *item;
    (void)entry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL) return;
    item = umi_ws_command_bar_model_selected(&command_bar->model);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || !item->enabled ||
        command_bar->activated_handler == NULL) {
        return;
    }
    activate_item(command_bar, item);
}

#endif

/* Editable changes are synchronous. GtkSearchEntry's delayed search signal
 * alone would leave old rows activatable immediately after a long paste. */
static UmiStatus synchronise_query(UmiGtk4WorkstationCommandBar *command_bar)
{
    const char *text = gtk_editable_get_text(GTK_EDITABLE(command_bar->entry));
    UmiWsCommandBarQuery parsed;
    UmiStatus status = umi_ws_command_bar_parse(text, &parsed);
    if (status == UMI_STATUS_OK &&
        (command_bar->search_status != UMI_STATUS_OK ||
         parsed.scope != command_bar->model.query.scope ||
         strcmp(parsed.text, command_bar->model.query.text) != 0))
        status = umi_ws_command_bar_model_set_query(&command_bar->model, text);
    command_bar->search_status = status;
    return status;
}

static void on_search_changed(GtkEditable *entry, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar = user_data;
    if (command_bar == NULL || command_bar->changing_text) return;
    (void)synchronise_query(command_bar);
    rebuild_result_widgets(command_bar);
    if (gtk_editable_get_text(entry)[0] != '\0' && gtk_widget_get_mapped(command_bar->root))
        gtk_popover_popup(GTK_POPOVER(command_bar->popover));
    else
        gtk_popover_popdown(GTK_POPOVER(command_bar->popover));
}

/* Copy the page only when entry validation succeeded. Callers cannot mistake
 * a retained catalogue's old results for the current invalid query. */
UmiStatus umi_gtk4_ws_command_bar_page(
    const UmiGtk4WorkstationCommandBar *command_bar, UmiWsCommandBarPage *out_page)
{
    if (command_bar == NULL || out_page == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_page = (UmiWsCommandBarPage){0};
    if (command_bar->search_status != UMI_STATUS_OK) return command_bar->search_status;
    return umi_ws_command_bar_model_page(&command_bar->model,
        command_bar->maximum_visible_results, out_page);
}

/* Navigation always renders the selected page before a user can activate it.
 * The shared model remains the selection authority for every application. */
UmiStatus umi_gtk4_ws_command_bar_select_result(
    UmiGtk4WorkstationCommandBar *command_bar, size_t result_index)
{
    UmiStatus status;
    if (command_bar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (command_bar->search_status != UMI_STATUS_OK) return command_bar->search_status;
    status = umi_ws_command_bar_model_select_result(&command_bar->model, result_index);
    if (status == UMI_STATUS_OK) rebuild_result_widgets(command_bar);
    return status;
}

UmiStatus umi_gtk4_ws_command_bar_move_selection(
    UmiGtk4WorkstationCommandBar *command_bar, int32_t offset)
{
    UmiStatus status;
    if (command_bar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (command_bar->search_status != UMI_STATUS_OK) return command_bar->search_status;
    status = umi_ws_command_bar_model_move_selection(&command_bar->model, offset);
    if (status == UMI_STATUS_OK) rebuild_result_widgets(command_bar);
    return status;
}

/* Commands may replace or destroy this controller. Capture the action before
 * dispatch, and do not read controller state after the callback returns. */
UmiStatus umi_gtk4_ws_command_bar_activate_selected(UmiGtk4WorkstationCommandBar *command_bar)
{
    const UmiWsCommandBarItem *item;
    UmiStatus status;
    if (command_bar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = synchronise_query(command_bar);
    if (status != UMI_STATUS_OK) return status;
    item = umi_ws_command_bar_model_selected(&command_bar->model);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    if (!item->enabled || command_bar->activated_handler == NULL) return UMI_STATUS_UNAVAILABLE;
    activate_item(command_bar, item);
    return UMI_STATUS_OK;
}

static void on_search_activate(GtkSearchEntry *entry, gpointer user_data)
{
    (void)entry;
    (void)umi_gtk4_ws_command_bar_activate_selected(user_data);
}

/* Page buttons remain available for mouse, touch and assistive input. Selection
 * moves to the first result on the neighbouring page, without running it. */
static void on_page_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar = user_data;
    UmiWsCommandBarPage page;
    if (umi_gtk4_ws_command_bar_page(command_bar, &page) != UMI_STATUS_OK) return;
    if (GTK_WIDGET(button) == command_bar->previous_page && page.has_previous)
        (void)umi_gtk4_ws_command_bar_select_result(command_bar,
            page.first_result - command_bar->maximum_visible_results);
    else if (GTK_WIDGET(button) == command_bar->next_page && page.has_next)
        (void)umi_gtk4_ws_command_bar_select_result(command_bar,
            page.first_result + page.result_count);
}

/* A nonmodal suggestion popover lets typing continue in the search field.
 * Leaving the whole component dismisses it; moving into its own buttons does
 * not. GTK owns the focus controller and we disconnect it during teardown. */
static void on_command_focus_leave(GtkEventControllerFocus *controller, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar = user_data;
    (void)controller;
    if (!command_bar->replacing_results)
        gtk_popover_popdown(GTK_POPOVER(command_bar->popover));
}

/* After navigating inside the popup, move native focus to the highlighted
 * button too. Disabled results focus the popup itself so Enter stays blocked. */
static void focus_selected_result(UmiGtk4WorkstationCommandBar *command_bar)
{
    UmiWsCommandBarPage page;
    GtkListBoxRow *row;
    GtkWidget *button;
    if (umi_gtk4_ws_command_bar_page(command_bar, &page) != UMI_STATUS_OK ||
        page.result_count == 0U) return;
    row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(command_bar->result_list),
        (int)(page.selected_result - page.first_result));
    button = row != NULL ? gtk_list_box_row_get_child(row) : NULL;
    if (button != NULL && gtk_widget_get_sensitive(button)) gtk_widget_grab_focus(button);
    else gtk_widget_grab_focus(command_bar->popover);
}

/* Capture navigation before the entry or result buttons consume it. Ordinary
 * Home/End still edit the text; Ctrl+Home/End select the first/last result. */
static gboolean on_navigation_key(GtkEventControllerKey *controller, guint keyval,
    guint keycode, GdkModifierType state, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar = user_data;
    int32_t step;
    (void)keycode;
    if ((state & (GDK_ALT_MASK | GDK_SUPER_MASK | GDK_SHIFT_MASK)) != 0) return FALSE;
    if ((state & GDK_CONTROL_MASK) != 0) {
        if (keyval != GDK_KEY_Home && keyval != GDK_KEY_End) return FALSE;
        (void)umi_gtk4_ws_command_bar_select_result(command_bar,
            keyval == GDK_KEY_Home || command_bar->model.result_count == 0U
                ? 0U : command_bar->model.result_count - 1U);
        if (GTK_EVENT_CONTROLLER(controller) == command_bar->popover_keys)
            focus_selected_result(command_bar);
        return TRUE;
    }
    if (keyval == GDK_KEY_Escape) {
        gtk_popover_popdown(GTK_POPOVER(command_bar->popover));
        return TRUE;
    }
    /* Entry Enter already emits activate. Popup Enter runs the selected
     * action, except when the user deliberately focused a paging control. */
    if ((keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) &&
        GTK_EVENT_CONTROLLER(controller) == command_bar->popover_keys) {
        if (gtk_widget_has_focus(command_bar->previous_page) ||
            gtk_widget_has_focus(command_bar->next_page)) return FALSE;
        (void)umi_gtk4_ws_command_bar_activate_selected(command_bar);
        return TRUE;
    }
    if (keyval == GDK_KEY_Up) step = -1;
    else if (keyval == GDK_KEY_Down) step = 1;
    else if (keyval == GDK_KEY_Page_Up) step = -(int32_t)command_bar->maximum_visible_results;
    else if (keyval == GDK_KEY_Page_Down) step = (int32_t)command_bar->maximum_visible_results;
    else return FALSE;
    (void)umi_gtk4_ws_command_bar_move_selection(command_bar, step);
    if (GTK_EVENT_CONTROLLER(controller) == command_bar->popover_keys)
        focus_selected_result(command_bar);
    if (gtk_widget_get_mapped(command_bar->root))
        gtk_popover_popup(GTK_POPOVER(command_bar->popover));
    return TRUE;
}

/* Keep actions visible on small screens by shortening the field before it can
 * push the central editor or chart workspace beyond the monitor edge. */
static void apply_presentation(UmiGtk4WorkstationCommandBar *command_bar)
{
    UmiWsCommandBarPresentation presentation;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL) return;
    presentation = command_bar->model.presentation;
    gtk_widget_set_visible(
        command_bar->scope_label,
        presentation == UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED);
    gtk_widget_set_visible(
        command_bar->entry,
        presentation != UMI_WS_COMMAND_BAR_PRESENTATION_BUTTON);
    /* Apply this branch only when its contract condition is satisfied. */
    if (presentation == UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED) {
        gtk_search_entry_set_placeholder_text(
            GTK_SEARCH_ENTRY(command_bar->entry), command_bar->placeholder);
        gtk_editable_set_width_chars(GTK_EDITABLE(command_bar->entry), 24);
        gtk_menu_button_set_icon_name(
            GTK_MENU_BUTTON(command_bar->result_button),
            "pan-down-symbolic");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (presentation == UMI_WS_COMMAND_BAR_PRESENTATION_COMPACT) {
        gtk_search_entry_set_placeholder_text(
            GTK_SEARCH_ENTRY(command_bar->entry),
            command_bar->compact_placeholder);
        gtk_editable_set_width_chars(GTK_EDITABLE(command_bar->entry), 10);
        gtk_menu_button_set_icon_name(
            GTK_MENU_BUTTON(command_bar->result_button),
            "system-search-symbolic");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        gtk_menu_button_set_label(
            GTK_MENU_BUTTON(command_bar->result_button), "Commands");
    }
    ++command_bar->revision;
}

/*
 * Provide the gtk4 ws command bar config default operation used by this module and its
 * client applications.
 */
UmiGtk4WorkstationCommandBarConfig
umi_gtk4_ws_command_bar_config_default(void)
{
    UmiGtk4WorkstationCommandBarConfig config = {0};
    config.placeholder = "Search commands, windows and settings";
    config.compact_placeholder = "Search";
    config.maximum_visible_results = UMI_GTK4_COMMAND_BAR_DEFAULT_RESULTS;
    config.initial_available_width = 320;
    return config;
}

/*
 * Provide the gtk4 ws command bar create managed operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_ws_command_bar_create_managed(
    const UmiGtk4WorkstationCommandBarConfig *config,
    const UmiWsCommandBarModel *model,
    UmiGtk4WorkstationCommandBar **out_command_bar)
{
    UmiGtk4WorkstationCommandBar *command_bar;
    GtkWidget *popover_root;
    GtkWidget *help;
    GtkWidget *page_controls;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || model == NULL || out_command_bar == NULL ||
        config->initial_available_width < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_command_bar = NULL;
    command_bar = (UmiGtk4WorkstationCommandBar *)calloc(
        1U, sizeof(*command_bar));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = copy_model_with_query(
        &command_bar->model, model, &model->query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(command_bar);
        return status;
    }
    command_bar->maximum_visible_results =
        config->maximum_visible_results > 0U
            ? config->maximum_visible_results
            : UMI_GTK4_COMMAND_BAR_DEFAULT_RESULTS;
    /* A page cannot exceed the fixed catalogue, even if a host supplies SIZE_MAX. */
    if (command_bar->maximum_visible_results > UMI_WS_MAX_PALETTE_ITEMS)
        command_bar->maximum_visible_results = UMI_WS_MAX_PALETTE_ITEMS;
    status = umi_ws_copy_text(
        command_bar->placeholder,
        sizeof(command_bar->placeholder),
        config->placeholder != NULL
            ? config->placeholder
            : "Search commands, windows and settings");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ws_copy_text(
            command_bar->compact_placeholder,
            sizeof(command_bar->compact_placeholder),
            config->compact_placeholder != NULL
                ? config->compact_placeholder
                : "Search");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(command_bar);
        return status;
    }

    command_bar->root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    command_bar->scope_label = gtk_label_new("All");
    command_bar->entry = gtk_search_entry_new();
    command_bar->result_button = gtk_menu_button_new();
    command_bar->popover = gtk_popover_new();
    command_bar->result_list = gtk_list_box_new();
    command_bar->result_status = gtk_label_new("");
    command_bar->previous_page = gtk_button_new_with_label("Previous");
    command_bar->next_page = gtk_button_new_with_label("Next");
    page_controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    /* The command field uses one shared address in every Umicom application. */
    (void)umi_gtk4_automation_tag_widget(
        command_bar->entry,
        "umicom.command.search");
    popover_root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    help = gtk_label_new(
        "Use > for commands, + for windows, / for settings or ? for the assistant.");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar->root == NULL || command_bar->scope_label == NULL ||
        command_bar->entry == NULL || command_bar->result_button == NULL ||
        command_bar->popover == NULL || command_bar->result_list == NULL ||
        popover_root == NULL || help == NULL || page_controls == NULL ||
        command_bar->result_status == NULL || command_bar->previous_page == NULL ||
        command_bar->next_page == NULL) {
        release_unparented_widget(page_controls);
        release_unparented_widget(command_bar->result_status);
        release_unparented_widget(command_bar->previous_page);
        release_unparented_widget(command_bar->next_page);
        release_unparented_widget(help);
        release_unparented_widget(popover_root);
        release_unparented_widget(command_bar->result_list);
        release_unparented_widget(command_bar->popover);
        release_unparented_widget(command_bar->result_button);
        release_unparented_widget(command_bar->entry);
        release_unparented_widget(command_bar->scope_label);
        release_unparented_widget(command_bar->root);
        free(command_bar);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    g_object_ref_sink(command_bar->root);
    /* Suggestions must not take keyboard focus away after the first letter. */
    gtk_popover_set_autohide(GTK_POPOVER(command_bar->popover), FALSE);
    gtk_widget_set_focusable(command_bar->popover, TRUE);


    gtk_widget_add_css_class(command_bar->root, "umicom-command-bar-shell");
    gtk_widget_add_css_class(command_bar->scope_label, "dim-label");
    gtk_widget_add_css_class(command_bar->entry, "umicom-command-bar");
    gtk_widget_add_css_class(help, "dim-label");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_label_set_xalign(GTK_LABEL(help), 0.0F);
    gtk_widget_set_margin_top(popover_root, 8);
    gtk_widget_set_margin_bottom(popover_root, 8);
    gtk_widget_set_margin_start(popover_root, 8);
    gtk_widget_set_margin_end(popover_root, 8);
    gtk_widget_set_size_request(popover_root, 420, -1);
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(command_bar->result_list), GTK_SELECTION_NONE);
    gtk_box_append(GTK_BOX(popover_root), help);
    gtk_box_append(GTK_BOX(popover_root), command_bar->result_list);
    gtk_widget_set_hexpand(command_bar->result_status, TRUE);
    gtk_label_set_wrap(GTK_LABEL(command_bar->result_status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(command_bar->result_status), 0.0F);
    gtk_box_append(GTK_BOX(page_controls), command_bar->previous_page);
    gtk_box_append(GTK_BOX(page_controls), command_bar->result_status);
    gtk_box_append(GTK_BOX(page_controls), command_bar->next_page);
    gtk_box_append(GTK_BOX(popover_root), page_controls);
    gtk_widget_set_tooltip_text(command_bar->entry,
        "Use Up/Down, Page Up/Page Down or Ctrl+Home/End to select; Enter opens the selection.");
    (void)umi_gtk4_automation_tag_widget(command_bar->result_status, "umicom.command.results");
    (void)umi_gtk4_automation_tag_widget(command_bar->previous_page, "umicom.command.previous");
    (void)umi_gtk4_automation_tag_widget(command_bar->next_page, "umicom.command.next");
    g_signal_connect(command_bar->previous_page, "clicked", G_CALLBACK(on_page_clicked), command_bar);
    g_signal_connect(command_bar->next_page, "clicked", G_CALLBACK(on_page_clicked), command_bar);
    gtk_popover_set_child(GTK_POPOVER(command_bar->popover), popover_root);
    gtk_menu_button_set_popover(
        GTK_MENU_BUTTON(command_bar->result_button), command_bar->popover);
    gtk_widget_set_tooltip_text(
        command_bar->result_button, "Show matching commands and actions");
    gtk_box_append(GTK_BOX(command_bar->root), command_bar->scope_label);
    gtk_box_append(GTK_BOX(command_bar->root), command_bar->entry);
    gtk_box_append(GTK_BOX(command_bar->root), command_bar->result_button);

    /* Preserve the delayed connection for review. The editable signal below
     * validates pasted text before any following activation can be dispatched. */
#if 0
    g_signal_connect(
        command_bar->entry,
        "search-changed",
        G_CALLBACK(on_search_changed),
        command_bar);
#endif
    g_signal_connect(command_bar->entry, "changed", G_CALLBACK(on_search_changed), command_bar);
    command_bar->entry_keys = gtk_event_controller_key_new();
    command_bar->popover_keys = gtk_event_controller_key_new();
    gtk_event_controller_set_name(command_bar->entry_keys, "umicom.command.navigation");
    gtk_event_controller_set_name(command_bar->popover_keys, "umicom.command.navigation");
    gtk_event_controller_set_propagation_phase(command_bar->entry_keys, GTK_PHASE_CAPTURE);
    gtk_event_controller_set_propagation_phase(command_bar->popover_keys, GTK_PHASE_CAPTURE);
    g_signal_connect(command_bar->entry_keys, "key-pressed", G_CALLBACK(on_navigation_key), command_bar);
    g_signal_connect(command_bar->popover_keys, "key-pressed", G_CALLBACK(on_navigation_key), command_bar);
    gtk_widget_add_controller(command_bar->entry, command_bar->entry_keys);
    gtk_widget_add_controller(command_bar->popover, command_bar->popover_keys);
    command_bar->focus_watch = gtk_event_controller_focus_new();
    g_signal_connect(command_bar->focus_watch, "leave", G_CALLBACK(on_command_focus_leave), command_bar);
    gtk_widget_add_controller(command_bar->root, command_bar->focus_watch);
    g_signal_connect(
        command_bar->entry,
        "activate",
        G_CALLBACK(on_search_activate),
        command_bar);
    {
        char initial_text[UMI_UI_TEXT_CAPACITY + 2U];
        char prefix = umi_ws_command_bar_scope_prefix(command_bar->model.query.scope);
        if (prefix != '\0')
            (void)snprintf(initial_text, sizeof(initial_text), "%c%s", prefix, command_bar->model.query.text);
        else
            (void)snprintf(initial_text, sizeof(initial_text), "%s", command_bar->model.query.text);
        command_bar->changing_text = 1;
        gtk_editable_set_text(GTK_EDITABLE(command_bar->entry), initial_text);
        command_bar->changing_text = 0;
    }
    (void)umi_ws_command_bar_model_set_available_width(
        &command_bar->model, config->initial_available_width);
    rebuild_result_widgets(command_bar);
    apply_presentation(command_bar);
    *out_command_bar = command_bar;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by gtk4 ws command bar so the same storage can be reused
 * safely.
 */
void umi_gtk4_ws_command_bar_destroy(
    UmiGtk4WorkstationCommandBar *command_bar)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL) return;
    /* Retained widgets/controllers must not call back into a freed owner. */
    if (command_bar->focus_watch != NULL)
        g_signal_handlers_disconnect_by_data(command_bar->focus_watch, command_bar);
    if (command_bar->entry_keys != NULL)
        g_signal_handlers_disconnect_by_data(command_bar->entry_keys, command_bar);
    if (command_bar->popover_keys != NULL)
        g_signal_handlers_disconnect_by_data(command_bar->popover_keys, command_bar);
    command_bar->activated_handler = NULL;
    command_bar->activated_user_data = NULL;
    disconnect_command_bar_widgets(command_bar->popover, command_bar);
    disconnect_command_bar_widgets(command_bar->root, command_bar);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar->root != NULL) g_object_unref(command_bar->root);
    free(command_bar);
}

/*
 * Provide the gtk4 ws command bar widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_ws_command_bar_widget(
    UmiGtk4WorkstationCommandBar *command_bar)
{
    return command_bar != NULL ? command_bar->root : NULL;
}

/*
 * Provide the gtk4 ws command bar set model operation used by this module and its client
 * applications.
 */
/* The original refresh reset selection on every catalogue replacement.
 * Retain it for review; the candidate-based refresh below keeps a surviving
 * action selected and preserves the whole live model on validation failure. */
#if 0
UmiStatus umi_gtk4_ws_command_bar_set_model(
    UmiGtk4WorkstationCommandBar *command_bar,
    const UmiWsCommandBarModel *model)
{
    UmiWsCommandBarQuery current_query;
    UmiWsCommandBarPresentation current_presentation;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL || model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    current_query = command_bar->model.query;
    current_presentation = command_bar->model.presentation;
    status = copy_model_with_query(
        &command_bar->model, model, &current_query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* A catalogue refresh changes available actions, not the space assigned to
     * the widget. Keep the responsive presentation chosen for the current
     * window width instead of accepting the source model's default value. */
    command_bar->model.presentation = current_presentation;
    rebuild_result_widgets(command_bar);
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_gtk4_ws_command_bar_set_model(
    UmiGtk4WorkstationCommandBar *command_bar, const UmiWsCommandBarModel *model)
{
    UmiWsCommandBarModel *candidate;
    UmiWsCommandBarItem selected = {0};
    const UmiWsCommandBarItem *previous;
    UmiStatus status;
    if (command_bar == NULL || model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    previous = umi_ws_command_bar_model_selected(&command_bar->model);
    if (previous != NULL) selected = *previous;
    /* A heap candidate avoids large Windows stack frames and leaves the live
     * catalogue untouched if a replacement is invalid or cannot be allocated. */
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = copy_model_with_query(candidate, model, &command_bar->model.query);
    if (status == UMI_STATUS_OK) {
        candidate->presentation = command_bar->model.presentation;
        for (size_t index = 0U; index < candidate->result_count; ++index) {
            const UmiWsCommandBarItem *item = umi_ws_command_bar_model_result_at(candidate, index);
            /* An item key reused for a different action is a new selection,
             * not permission to transfer focus to a different operation. */
            if (strcmp(item->item_id, selected.item_id) == 0 &&
                strcmp(item->command_id, selected.command_id) == 0) {
                (void)umi_ws_command_bar_model_select_result(candidate, index);
                break;
            }
        }
        command_bar->model = *candidate;
    }
    free(candidate);
    if (status == UMI_STATUS_OK) rebuild_result_widgets(command_bar);
    return status;
}

/*
 * Provide the gtk4 ws command bar set query text operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_ws_command_bar_set_query_text(
    UmiGtk4WorkstationCommandBar *command_bar,
    const char *text)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ws_command_bar_model_set_query(&command_bar->model, text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    command_bar->search_status = UMI_STATUS_OK;
    command_bar->changing_text = 1;
    gtk_editable_set_text(GTK_EDITABLE(command_bar->entry), text);
    command_bar->changing_text = 0;
    rebuild_result_widgets(command_bar);
    return UMI_STATUS_OK;
}

/*
 * Provide the gtk4 ws command bar set available width operation used by this module and
 * its client applications.
 */
UmiStatus umi_gtk4_ws_command_bar_set_available_width(
    UmiGtk4WorkstationCommandBar *command_bar,
    int32_t available_width)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ws_command_bar_model_set_available_width(
        &command_bar->model, available_width);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) apply_presentation(command_bar);
    return status;
}

/*
 * Provide the gtk4 ws command bar set activated handler operation used by this module and
 * its client applications.
 */
UmiStatus umi_gtk4_ws_command_bar_set_activated_handler(
    UmiGtk4WorkstationCommandBar *command_bar,
    UmiGtk4WorkstationCommandBarActivatedHandler handler,
    void *user_data)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    command_bar->activated_handler = handler;
    command_bar->activated_user_data = user_data;
    return UMI_STATUS_OK;
}

/*
 * Provide the gtk4 ws command bar snapshot operation used by this module and its client
 * applications.
 */
/* This original projection is preserved for review. The active implementation
 * below renders the selected page and hides stale results after invalid input;
 * the earlier first-page-only behaviour is no longer the UI authority. */
#if 0
UmiGtk4WorkstationCommandBarSnapshot umi_gtk4_ws_command_bar_snapshot(
    const UmiGtk4WorkstationCommandBar *command_bar)
{
    UmiGtk4WorkstationCommandBarSnapshot snapshot = {0};
    const UmiWsCommandBarItem *selected;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL) return snapshot;
    snapshot.scope = command_bar->model.query.scope;
    snapshot.presentation = command_bar->model.presentation;
    snapshot.item_count = command_bar->model.count;
    snapshot.result_count = command_bar->model.result_count;
    snapshot.revision = command_bar->revision;
    (void)umi_ws_copy_text(
        snapshot.query,
        sizeof(snapshot.query),
        command_bar->model.query.text);
    selected = umi_ws_command_bar_model_selected(&command_bar->model);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selected != NULL) {
        (void)umi_ws_copy_text(
            snapshot.selected_item_id,
            sizeof(snapshot.selected_item_id),
            selected->item_id);
    }
    return snapshot;
}
#endif

UmiGtk4WorkstationCommandBarSnapshot umi_gtk4_ws_command_bar_snapshot(
    const UmiGtk4WorkstationCommandBar *command_bar)
{
    UmiGtk4WorkstationCommandBarSnapshot snapshot = {0};
    const UmiWsCommandBarItem *selected;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_bar == NULL) return snapshot;
    snapshot.scope = command_bar->model.query.scope;
    snapshot.presentation = command_bar->model.presentation;
    snapshot.item_count = command_bar->model.count;
    snapshot.result_count = command_bar->search_status == UMI_STATUS_OK
        ? command_bar->model.result_count : 0U;
    snapshot.revision = command_bar->revision;
    (void)umi_ws_copy_text(
        snapshot.query,
        sizeof(snapshot.query),
        command_bar->model.query.text);
    selected = umi_ws_command_bar_model_selected(&command_bar->model);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selected != NULL && command_bar->search_status == UMI_STATUS_OK) {
        (void)umi_ws_copy_text(
            snapshot.selected_item_id,
            sizeof(snapshot.selected_item_id),
            selected->item_id);
    }
    return snapshot;
}

/*
 * Initialise gtk4 ws command bar from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_command_bar_create(const char *placeholder)
{
    GtkWidget *entry = gtk_search_entry_new();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return NULL;
    gtk_widget_add_css_class(entry, "umicom-command-bar");
    gtk_search_entry_set_placeholder_text(
        GTK_SEARCH_ENTRY(entry),
        placeholder != NULL
            ? placeholder
            : "Search commands, windows, settings and AI");
    gtk_widget_set_hexpand(entry, TRUE);
    return entry;
}

/*
 * Provide the gtk4 ws command bar query operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_ws_command_bar_query(
    GtkWidget *entry,
    UmiWsCommandBarQuery *out_query)
{
    const char *text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || !GTK_IS_EDITABLE(entry) || out_query == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    text = gtk_editable_get_text(GTK_EDITABLE(entry));
    return umi_ws_command_bar_parse(text, out_query);
}
