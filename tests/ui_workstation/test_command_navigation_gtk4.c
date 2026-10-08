/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_command_navigation_gtk4.c
 * PURPOSE: Exercise native command paging, refresh, rejected input and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/command_bar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #value); \
    failed = 1; goto cleanup; } } while (0)

typedef struct Dispatch {
    size_t calls;
    char selected[UMI_UI_ID_CAPACITY];
} Dispatch;

/* The callback only records identity. Regression coverage must never run an
 * application command, place an order or open an external service. */
static void selected(const UmiWsCommandBarItem *item, void *data)
{
    Dispatch *dispatch = data;
    ++dispatch->calls;
    (void)snprintf(dispatch->selected, sizeof(dispatch->selected), "%s", item->item_id);
}

/* Search GTK-owned descendants without depending on their private widget order. */
static GtkWidget *find(GtkWidget *root, const char *id, bool search)
{
    const char *actual;
    if (root == NULL) return NULL;
    if (search && GTK_IS_SEARCH_ENTRY(root)) return root;
    actual = g_object_get_data(G_OBJECT(root), "umicom-command-bar-item-id");
    if (!search && actual != NULL && strcmp(actual, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *result = find(child, id, search);
        if (result != NULL) return result;
    }
    return NULL;
}

/* Retain the actual controller so teardown can be tested independently of
 * widget ownership. Signal emission exercises the callback without synthesising
 * a platform event or presenting a native application window. */
static GtkEventController *keys_for(GtkWidget *entry)
{
    GListModel *controllers = gtk_widget_observe_controllers(entry);
    GtkEventController *result = NULL;
    for (guint index = 0U; index < g_list_model_get_n_items(controllers); ++index) {
        GtkEventController *controller = g_list_model_get_item(controllers, index);
        if (GTK_IS_EVENT_CONTROLLER_KEY(controller) &&
            g_strcmp0(gtk_event_controller_get_name(controller), "umicom.command.navigation") == 0) {
            result = controller;
            break;
        }
        g_object_unref(controller);
    }
    g_object_unref(controllers);
    return result;
}

int main(int argc, char **argv)
{
    UmiWsCommandBarModel *model = calloc(1U, sizeof(*model));
    UmiGtk4WorkstationCommandBar *bar = NULL;
    UmiGtk4WorkstationCommandBarConfig config = umi_gtk4_ws_command_bar_config_default();
    UmiWsCommandBarPage page;
    UmiGtk4WorkstationCommandBarSnapshot snapshot;
    GtkWidget *root = NULL, *entry = NULL, *old_row = NULL;
    GtkEventController *keys = NULL;
    Dispatch dispatch = {0};
    const char *name = argc == 2 ? argv[1] : "";
    char oversized[UMI_UI_TEXT_CAPACITY + 8U];
    gboolean handled = FALSE;
    int failed = 0;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) { free(model); return 77; }
    CHECK(model != NULL);
    umi_ws_command_bar_model_init(model);
    for (size_t index = 0U; index < 23U; ++index) {
        char id[32], title[64];
        (void)snprintf(id, sizeof(id), "action.%zu", index);
        (void)snprintf(title, sizeof(title), "Open tool %zu", index);
        CHECK(umi_ws_command_bar_model_add(model, id, title, "Inspect the workspace",
            id, "navigation", UMI_WS_COMMAND_SCOPE_COMMAND, 20U) == UMI_STATUS_OK);
    }
    config.maximum_visible_results = 5U;
    if (strcmp(name, "initial-query") == 0)
        CHECK(umi_ws_command_bar_model_set_query(model, "> tool 22") == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_command_bar_create_managed(&config, model, &bar) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_command_bar_set_activated_handler(bar, selected, &dispatch) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_command_bar_widget(bar));
    CHECK(find(root, "", true) != NULL);
    entry = g_object_ref(find(root, "", true));
    keys = keys_for(entry);
    CHECK(keys != NULL);

    if (strcmp(name, "paging") == 0) {
        CHECK(find(root, "action.22", false) == NULL);
        CHECK(umi_gtk4_ws_command_bar_select_result(bar, 22U) == UMI_STATUS_OK);
        CHECK(umi_gtk4_ws_command_bar_page(bar, &page) == UMI_STATUS_OK);
        CHECK(page.first_result == 20U && page.result_count == 3U && !page.has_next);
        CHECK(find(root, "action.22", false) != NULL && find(root, "action.0", false) == NULL);
        CHECK(umi_gtk4_ws_command_bar_activate_selected(bar) == UMI_STATUS_OK);
        CHECK(dispatch.calls == 1U && strcmp(dispatch.selected, "action.22") == 0);
    } else if (strcmp(name, "keyboard") == 0) {
        g_signal_emit_by_name(keys, "key-pressed", GDK_KEY_Page_Down, 0U, (GdkModifierType)0, &handled);
        CHECK(handled && umi_gtk4_ws_command_bar_page(bar, &page) == UMI_STATUS_OK);
        CHECK(page.selected_result == 5U && page.first_result == 5U);
        handled = FALSE;
        g_signal_emit_by_name(keys, "key-pressed", GDK_KEY_End, 0U, GDK_CONTROL_MASK, &handled);
        CHECK(handled);
        g_signal_emit_by_name(entry, "activate");
        CHECK(dispatch.calls == 1U && strcmp(dispatch.selected, "action.22") == 0);
    } else if (strcmp(name, "refresh") == 0) {
        CHECK(umi_gtk4_ws_command_bar_select_result(bar, 12U) == UMI_STATUS_OK);
        model->items[22].priority = 200U;
        CHECK(umi_gtk4_ws_command_bar_set_model(bar, model) == UMI_STATUS_OK);
        snapshot = umi_gtk4_ws_command_bar_snapshot(bar);
        CHECK(strcmp(snapshot.selected_item_id, "action.12") == 0);
        CHECK(umi_gtk4_ws_command_bar_page(bar, &page) == UMI_STATUS_OK && page.selected_result == 13U);
        CHECK(umi_ws_command_bar_model_set_enabled(model, "action.12", false) == UMI_STATUS_OK);
        CHECK(umi_gtk4_ws_command_bar_set_model(bar, model) == UMI_STATUS_OK);
        CHECK(umi_gtk4_ws_command_bar_activate_selected(bar) == UMI_STATUS_UNAVAILABLE);
        CHECK(dispatch.calls == 0U);
    } else if (strcmp(name, "invalid-refresh") == 0) {
        CHECK(umi_gtk4_ws_command_bar_select_result(bar, 12U) == UMI_STATUS_OK);
        model->count = UMI_WS_MAX_PALETTE_ITEMS + 1U;
        CHECK(umi_gtk4_ws_command_bar_set_model(bar, model) == UMI_STATUS_INVALID_ARGUMENT);
        snapshot = umi_gtk4_ws_command_bar_snapshot(bar);
        CHECK(strcmp(snapshot.selected_item_id, "action.12") == 0);
    } else if (strcmp(name, "invalid-input") == 0) {
        old_row = g_object_ref(find(root, "action.0", false));
        memset(oversized, 'x', sizeof(oversized)); oversized[sizeof(oversized)-1U] = '\0';
        gtk_editable_set_text(GTK_EDITABLE(entry), oversized);
        CHECK(umi_gtk4_ws_command_bar_page(bar, &page) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(page.total_results == 0U && find(root, "action.0", false) == NULL);
        g_signal_emit_by_name(old_row, "clicked");
        g_signal_emit_by_name(entry, "activate");
        CHECK(dispatch.calls == 0U);
        CHECK(umi_gtk4_ws_command_bar_set_model(bar, model) == UMI_STATUS_OK);
        CHECK(umi_gtk4_ws_command_bar_activate_selected(bar) == UMI_STATUS_CAPACITY_EXCEEDED);
        gtk_editable_set_text(GTK_EDITABLE(entry), "tool 22");
        CHECK(umi_gtk4_ws_command_bar_activate_selected(bar) == UMI_STATUS_OK);
        CHECK(dispatch.calls == 1U && strcmp(dispatch.selected, "action.22") == 0);
    } else if (strcmp(name, "retained-controls") == 0) {
        old_row = g_object_ref(find(root, "action.0", false));
        umi_gtk4_ws_command_bar_destroy(bar); bar = NULL;
        g_signal_emit_by_name(keys, "key-pressed", GDK_KEY_Down, 0U, (GdkModifierType)0, &handled);
        g_signal_emit_by_name(old_row, "clicked");
        gtk_editable_set_text(GTK_EDITABLE(entry), "changed after teardown");
        g_signal_emit_by_name(entry, "activate");
        CHECK(dispatch.calls == 0U && !handled);
    } else if (strcmp(name, "initial-query") == 0) {
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), ">tool 22") == 0);
        CHECK(umi_gtk4_ws_command_bar_activate_selected(bar) == UMI_STATUS_OK);
        CHECK(dispatch.calls == 1U && strcmp(dispatch.selected, "action.22") == 0);
    } else if (strcmp(name, "no-matches") == 0) {
        CHECK(umi_gtk4_ws_command_bar_set_query_text(bar, "missing command") == UMI_STATUS_OK);
        CHECK(umi_gtk4_ws_command_bar_page(bar, &page) == UMI_STATUS_OK);
        CHECK(page.total_results == 0U);
        CHECK(umi_gtk4_ws_command_bar_activate_selected(bar) == UMI_STATUS_NOT_FOUND);
        CHECK(dispatch.calls == 0U);
    } else {
        CHECK(0 && "Choose a registered native case");
    }
cleanup:
    umi_gtk4_ws_command_bar_destroy(bar);
    g_clear_object(&keys);
    g_clear_object(&old_row);
    g_clear_object(&entry);
    g_clear_object(&root);
    free(model);
    return failed;
}
