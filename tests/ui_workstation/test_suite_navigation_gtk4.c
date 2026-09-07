/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_suite_navigation_gtk4.c
 * Purpose: Check normal tool navigation and named-layout search in shared clients.
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_workstation.h"
#include "umicom/application/suite_layout/suite_layout.h"
#include "umicom/application/experience_catalogue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Checks stay active in release builds. No fixture presents native windows,
 * opens files, runs a product service, or starts an application process. */
#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "%s, line %d: %s\n", experience->application_id, __LINE__, #expression); \
        failed = 1; goto cleanup; \
    } \
} while (0)

typedef struct NavigationFixture { size_t created; size_t released; } NavigationFixture;

/* Observe ownership without retaining the provider through a hidden global. */
static void body_released(gpointer data, GObject *object)
{
    NavigationFixture *fixture = data;
    (void)object;
    ++fixture->released;
}

/* Real editable bodies make focus-only draft preservation observable. */
static GtkWidget *create_body(const UmiUiWorkspaceWindow *window, void *data)
{
    NavigationFixture *fixture = data;
    GtkWidget *entry = gtk_entry_new();
    g_object_set_data_full(G_OBJECT(entry), "navigation-body-id", g_strdup(window->window_id), g_free);
    g_object_weak_ref(G_OBJECT(entry), body_released, fixture);
    ++fixture->created;
    return entry;
}

/* Find production tags or a fixture's provider ID without private child indexes. */
static GtkWidget *find_data(GtkWidget *root, const char *key, const char *value)
{
    const char *data;
    GtkWidget *child;
    if (root == NULL) return NULL;
    data = g_object_get_data(G_OBJECT(root), key);
    if (data != NULL && strcmp(data, value) == 0) return root;
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_data(child, key, value);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Search-entry changes are emitted locally; the unparented bar cannot popup. */
static void set_search(GtkWidget *entry, const char *text)
{
    gtk_editable_set_text(GTK_EDITABLE(entry), text);
    g_signal_emit_by_name(entry, "search-changed");
}

/* Panel actions are queued until their originating GTK callback has returned.
 * Drain only ready work; never sleep for a timer or present a desktop surface. */
static void drain_navigation_actions(void)
{
    for (unsigned index = 0U; index < 64U && g_main_context_pending(NULL); ++index)
        (void)g_main_context_iteration(NULL, FALSE);
}

/* Exercise each canonical product's shared navigation projection. This is
 * catalogue coverage with test bodies, not proof of each dedicated frontend. */
static int check_product(const UmiApplicationExperienceDefinition *experience)
{
    NavigationFixture fixture = {0};
    UmiApplicationSuiteGtk4WorkstationConfig config = {
        experience->application_id, experience->display_name, "Navigation fixture", create_body, &fixture
    };
    UmiApplicationSuiteGtk4Workstation *workstation = NULL;
    UmiUiWorkspaceLayout *layout = calloc(1U, sizeof(*layout));
    UmiApplicationSuiteGtk4WorkstationSnapshot *before = calloc(1U, sizeof(*before));
    UmiApplicationSuiteGtk4WorkstationSnapshot *after = calloc(1U, sizeof(*after));
    char *original = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    char *current = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    GtkWidget *root = NULL;
    GtkWidget *body = NULL;
    GtkWidget *found;
    GtkWidget *search;
    GtkWidget *menu;
    GtkWidget *row;
    GtkWidget *close_button;
    const UmiUiWorkspaceWindow *selected = NULL;
    char query[UMI_UI_TEXT_CAPACITY];
    char named_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char opened_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char close_tag[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 20U];
    char tool_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    size_t provider_count;
    int written;
    int failed = 0;
    CHECK(layout != NULL && before != NULL && after != NULL && original != NULL && current != NULL);
    CHECK(umi_application_suite_layout_project_default(experience->application_id, layout) == UMI_STATUS_OK);
    /* Fail before construction if a future preset could present a detached window. */
    for (size_t index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        CHECK(!window->visible || !window->floating);
        if (selected == NULL && window->visible && !window->floating &&
            strcmp(window->placement_id, "canvas") != 0) {
            const UmiExperiencePanelDefinition *panel =
                umi_application_experience_panel_find(experience, window->tool_id);
            if (panel == NULL || (panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) != 0U)
                selected = window;
        }
    }
    CHECK(selected != NULL);
    CHECK(g_strlcpy(tool_id, selected->tool_id, sizeof(tool_id)) < sizeof(tool_id));
    CHECK(umi_application_suite_gtk4_workstation_create(&config, &workstation) == UMI_STATUS_OK);
    root = g_object_ref(umi_application_suite_gtk4_workstation_widget(workstation));
    *before = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(before->layout_locked && !before->editing_layout);
    menu = find_data(root, "umicom-automation-id", "umicom.window-catalogue.menu");
    search = find_data(root, "umicom-automation-id", "umicom.command.search");
    CHECK(GTK_IS_MENU_BUTTON(menu) && gtk_widget_get_sensitive(menu));
    CHECK(GTK_IS_EDITABLE(search));
    found = find_data(root, "umicom-automation-id", "umicom.window-catalogue.region");
    CHECK(GTK_IS_DROP_DOWN(found) && !gtk_widget_get_sensitive(found));
    found = find_data(root, "navigation-body-id", selected->window_id);
    CHECK(GTK_IS_EDITABLE(found));
    body = g_object_ref(found);
    gtk_editable_set_text(GTK_EDITABLE(body), "Unsaved navigation draft");
    provider_count = fixture.created;
    CHECK(umi_application_suite_gtk4_workstation_export_layout(workstation, 123U,
        original, UMI_UI_LAYOUT_ENCODED_CAPACITY) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_show_window(workstation, selected->window_id) == UMI_STATUS_OK);
    CHECK(fixture.created == provider_count);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(body)), "Unsaved navigation draft") == 0);
    CHECK(umi_application_suite_gtk4_workstation_show_window(workstation, "missing.instance") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_application_suite_gtk4_workstation_show_window(workstation, "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_application_suite_gtk4_workstation_export_layout(workstation, 123U,
        current, UMI_UI_LAYOUT_ENCODED_CAPACITY) == UMI_STATUS_OK);
    CHECK(strcmp(original, current) == 0);

    /* Use the actual search activation callback, without opening its popover. */
    written = snprintf(query, sizeof(query), "+ %s", tool_id);
    CHECK(written >= 0 && (size_t)written < sizeof(query));
    set_search(search, query);
    *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(after->command_bar.result_count != 0U);
    g_signal_emit_by_name(search, "activate");
    CHECK(fixture.created == provider_count);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(body)), "Unsaved navigation draft") == 0);
    g_clear_object(&body);

    /* Make one ordinary dock instance, then close and reopen it while locked.
     * This checks normal navigation without treating Close as product deletion. */
    written = snprintf(named_id, sizeof(named_id), "%s.navigation", experience->application_id);
    CHECK(written >= 0 && (size_t)written < sizeof(named_id));
    CHECK(umi_application_suite_gtk4_workstation_create_blank_layout(workstation, named_id,
        "Navigation recovery workspace") == UMI_STATUS_OK);
    set_search(search, query);
    *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(after->command_bar.result_count == 0U);
    CHECK(umi_application_suite_gtk4_workstation_show_window(workstation, selected->window_id) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_application_suite_gtk4_workstation_begin_layout_edit(workstation) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_open_window(workstation, tool_id, "left", 0, 10U,
        opened_id, sizeof(opened_id)) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_commit_layout_edit(workstation) == UMI_STATUS_OK);
    written = snprintf(close_tag, sizeof(close_tag), "%s.action.close", opened_id);
    CHECK(written >= 0 && (size_t)written < sizeof(close_tag));
    close_button = find_data(root, "umicom-automation-id", close_tag);
    CHECK(GTK_IS_BUTTON(close_button) && gtk_widget_get_sensitive(close_button));
    g_signal_emit_by_name(close_button, "clicked");
    drain_navigation_actions();
    *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(after->layout_locked && !after->editing_layout && after->rendered_panel_count == 0U);
    row = find_data(root, "umicom-automation-id", tool_id);
    CHECK(GTK_IS_BUTTON(row) && gtk_widget_get_sensitive(row));
    g_signal_emit_by_name(row, "clicked");
    *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(after->layout_locked && !after->editing_layout && after->rendered_panel_count == 1U);
    CHECK(find_data(root, "navigation-body-id", opened_id) != NULL);

    /* Named layouts are searchable; a blank active layout has no stale panel rows. */
    CHECK(umi_application_suite_gtk4_workstation_select_layout(workstation, before->active_layout_id) == UMI_STATUS_OK);
    set_search(search, "Navigation recovery workspace");
    *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(after->command_bar.result_count == 1U);
    g_signal_emit_by_name(search, "activate");
    *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(strcmp(after->active_layout_id, named_id) == 0);
    CHECK(umi_application_suite_gtk4_workstation_begin_layout_edit(workstation) == UMI_STATUS_OK);
    set_search(search, "Navigation recovery workspace");
    g_signal_emit_by_name(search, "activate");
    *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(after->editing_layout && strcmp(after->active_layout_id, named_id) == 0);
    CHECK(umi_application_suite_gtk4_workstation_cancel_layout_edit(workstation) == UMI_STATUS_OK);
    /* No fixture may create a visible top-level as a side effect. */
    {
        GListModel *windows = gtk_window_get_toplevels();
        for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index) {
            GtkWidget *window = g_list_model_get_item(windows, index);
            const int shown = gtk_widget_get_visible(window) || gtk_widget_get_realized(window);
            g_object_unref(window);
            CHECK(!shown);
        }
    }
cleanup:
    g_clear_object(&body);
    umi_application_suite_gtk4_workstation_destroy(workstation);
    g_clear_object(&root);
    free(original);
    free(current);
    free(after);
    free(before);
    free(layout);
    if (fixture.created != fixture.released) failed = 1;
    return failed;
}

/* The actual product frontends have their own acceptance tests. This loop
 * guards Framework navigation projection for the entire experience catalogue. */
int main(void)
{
    const size_t count = umi_application_experience_catalogue_count();
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    if (count == 0U) return 1;
    for (size_t index = 0U; index < count; ++index) {
        const UmiApplicationExperienceDefinition *experience = umi_application_experience_catalogue_at(index);
        if (experience == NULL || check_product(experience)) return 1;
    }
    return 0;
}
