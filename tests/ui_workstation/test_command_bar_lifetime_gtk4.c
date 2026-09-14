/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_command_bar_lifetime_gtk4.c
 * PURPOSE: Verify search dispatch survives model replacement and owner teardown.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/command_bar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
    failed = 1; goto cleanup; } } while (0)

typedef struct DispatchFixture {
    UmiGtk4WorkstationCommandBar *bar;
    UmiWsCommandBarModel *replacement;
    size_t calls;
    int destroy_on_call;
    int copied_item_survived;
    UmiStatus status;
} DispatchFixture;

/* The callback deliberately changes the owner before reading the copied item. */
static void on_action(const UmiWsCommandBarItem *item, void *data)
{
    DispatchFixture *fixture = data;
    const char *expected = fixture->destroy_on_call ? "Replacement" : "Original";
    ++fixture->calls;
    if (fixture->destroy_on_call) {
        umi_gtk4_ws_command_bar_destroy(fixture->bar);
        fixture->bar = NULL;
        fixture->status = UMI_STATUS_OK;
    } else {
        fixture->status = umi_gtk4_ws_command_bar_set_model(fixture->bar, fixture->replacement);
    }
    fixture->copied_item_survived = strcmp(item->title, expected) == 0 &&
        strcmp(item->item_id, "test.open") == 0;
}

/* Find the real result row by the copied identifier used by production dispatch. */
static GtkWidget *find_result(GtkWidget *root, const char *id)
{
    GtkWidget *child;
    const char *actual;
    if (root == NULL) return NULL;
    actual = g_object_get_data(G_OBJECT(root), "umicom-command-bar-item-id");
    if (GTK_IS_BUTTON(root) && actual != NULL && strcmp(actual, id) == 0) return root;
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_result(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Search entry activation is the keyboard path, independently of result clicks. */
static GtkWidget *find_search(GtkWidget *root)
{
    GtkWidget *child;
    if (root == NULL || GTK_IS_SEARCH_ENTRY(root)) return root;
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_search(child);
        if (found != NULL) return found;
    }
    return NULL;
}

/* No native window, process, tool, timer or application activation is needed. */
int main(void)
{
    DispatchFixture fixture = {0};
    UmiWsCommandBarModel *model = calloc(1U, sizeof(*model));
    UmiGtk4WorkstationCommandBarConfig config = umi_gtk4_ws_command_bar_config_default();
    GtkWidget *root = NULL;
    GtkWidget *old_row = NULL;
    GtkWidget *new_row = NULL;
    GtkWidget *entry = NULL;
    GtkWidget *widget;
    int failed = 0;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) { free(model); return 77; }
    fixture.replacement = calloc(1U, sizeof(*fixture.replacement));
    CHECK(model != NULL && fixture.replacement != NULL);
    umi_ws_command_bar_model_init(model);
    CHECK(umi_ws_command_bar_model_add(model, "test.open", "Original", "Open a real panel",
        "test.open", "panel", UMI_WS_COMMAND_SCOPE_PANEL, 10U) == UMI_STATUS_OK);
    *fixture.replacement = *model;
    (void)g_strlcpy(fixture.replacement->items[0].title, "Replacement",
        sizeof(fixture.replacement->items[0].title));
    CHECK(umi_gtk4_ws_command_bar_create_managed(&config, model, &fixture.bar) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_command_bar_set_activated_handler(fixture.bar, on_action, &fixture) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_command_bar_widget(fixture.bar));
    widget = find_result(root, "test.open");
    CHECK(GTK_IS_BUTTON(widget));
    old_row = g_object_ref(widget);
    widget = find_search(root);
    CHECK(GTK_IS_SEARCH_ENTRY(widget));
    entry = g_object_ref(widget);
    g_signal_emit_by_name(old_row, "clicked");
    CHECK(fixture.calls == 1U && fixture.status == UMI_STATUS_OK && fixture.copied_item_survived);
    g_signal_emit_by_name(old_row, "clicked");
    CHECK(fixture.calls == 1U);
    widget = find_result(root, "test.open");
    CHECK(GTK_IS_BUTTON(widget) && widget != old_row);
    new_row = g_object_ref(widget);
    fixture.destroy_on_call = 1;
    g_signal_emit_by_name(new_row, "clicked");
    CHECK(fixture.calls == 2U && fixture.bar == NULL && fixture.copied_item_survived);
    g_signal_emit_by_name(new_row, "clicked");
    g_signal_emit_by_name(entry, "activate");
    g_signal_emit_by_name(entry, "search-changed");
    CHECK(fixture.calls == 2U);
    g_clear_object(&old_row);
    g_clear_object(&new_row);
    g_clear_object(&entry);
    g_clear_object(&root);

    CHECK(umi_gtk4_ws_command_bar_create_managed(&config, fixture.replacement,
        &fixture.bar) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_command_bar_set_activated_handler(fixture.bar, on_action, &fixture) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_command_bar_widget(fixture.bar));
    widget = find_search(root);
    CHECK(GTK_IS_SEARCH_ENTRY(widget));
    entry = g_object_ref(widget);
    g_signal_emit_by_name(entry, "activate");
    CHECK(fixture.calls == 3U && fixture.bar == NULL && fixture.copied_item_survived);
    g_signal_emit_by_name(entry, "activate");
    CHECK(fixture.calls == 3U && !gtk_widget_get_realized(root));
cleanup:
    umi_gtk4_ws_command_bar_destroy(fixture.bar);
    g_clear_object(&old_row);
    g_clear_object(&new_row);
    g_clear_object(&entry);
    g_clear_object(&root);
    free(fixture.replacement);
    free(model);
    return failed;
}
