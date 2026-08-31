/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/application_suite_workstation_gtk4.c
 *
 * PURPOSE:
 *   Connect canonical Application Suite layout runtime and selector metadata to
 *   the reusable GTK4 workspace-layout host for all thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_workstation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/suite_layout/geometry.h"
#include "umicom/desktop/ui_bridge.h"
#include "umicom/ui/workspace_customisation.h"

struct UmiApplicationSuiteGtk4Workstation {
    UmiApplicationSuiteLayoutRuntime runtime;
    UmiApplicationSuiteLayoutSelectorModel selector;
    UmiUiWorkspaceCustomisation customisation;
    UmiGtk4WorkspaceLayoutHost *host;
    GtkWidget *root;
    GtkWidget *title_label;
    GtkWidget *layout_dropdown;
    GtkWidget *new_window_button;
    GtkWidget *new_window_popover;
    GtkWidget *new_window_search;
    GtkWidget *new_window_list;
    GtkWidget *new_window_category;
    GtkWidget *new_window_region;
    GtkWidget *new_window_floating;
    GtkWidget *new_window_status;
    GtkWidget *edit_layout_button;
    GtkWidget *cancel_edit_button;
    int changing_selection;
    uint64_t revision;
};

static const char *WINDOW_REGIONS[] = {
    "centre", "left", "right", "bottom", "top"
};

static const UmiUiWindowCategory WINDOW_CATEGORIES[] = {
    0,
    UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,
    UMI_UI_WINDOW_CATEGORY_NAVIGATION,
    UMI_UI_WINDOW_CATEGORY_OPERATIONS,
    UMI_UI_WINDOW_CATEGORY_DATA,
    UMI_UI_WINDOW_CATEGORY_AI,
    UMI_UI_WINDOW_CATEGORY_TRADING,
    UMI_UI_WINDOW_CATEGORY_GENERAL
};

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static void refresh_heading(UmiApplicationSuiteGtk4Workstation *workstation)
{
    const UmiApplicationSuiteLayoutChoice *choice =
        umi_application_suite_layout_selector_current(&workstation->selector);
    if (choice != NULL)
        gtk_label_set_text(GTK_LABEL(workstation->title_label), choice->title);
}

static const UmiUiWorkspaceLayout *active_layout(
    const UmiApplicationSuiteGtk4Workstation *workstation)
{
    return workstation != NULL
        ? umi_ui_workspace_customisation_active_const(
              &workstation->customisation)
        : NULL;
}

static UmiStatus rebuild_active_layout(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    const UmiUiWorkspaceLayout *layout = active_layout(workstation);
    if (workstation == NULL || layout == NULL) return UMI_STATUS_INVALID_STATE;
    return umi_gtk4_workspace_layout_host_rebuild(workstation->host, layout);
}

static void refresh_edit_controls(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    int editing;
    if (workstation == NULL) return;
    editing = workstation->customisation.edit_active ? 1 : 0;
    if (workstation->edit_layout_button != NULL) {
        gtk_button_set_label(
            GTK_BUTTON(workstation->edit_layout_button),
            editing ? "Apply and Lock" : "Edit Layout");
    }
    if (workstation->cancel_edit_button != NULL) {
        gtk_widget_set_visible(workstation->cancel_edit_button, editing);
    }
    if (workstation->new_window_button != NULL) {
        gtk_widget_set_sensitive(workstation->new_window_button, editing);
    }
    if (workstation->layout_dropdown != NULL) {
        gtk_widget_set_sensitive(workstation->layout_dropdown, !editing);
    }
}

UmiStatus umi_application_suite_gtk4_workstation_select_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *layout_id)
{
    UmiStatus status;
    size_t index;
    if (workstation == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    status = umi_application_suite_layout_runtime_select(
        &workstation->runtime, layout_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_suite_layout_selector_select(
        &workstation->selector, layout_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_customisation_activate(
        &workstation->customisation, layout_id);
    if (status != UMI_STATUS_OK) return status;
    status = rebuild_active_layout(workstation);
    if (status != UMI_STATUS_OK) return status;
    workstation->changing_selection = 1;
    for (index = 0U; index < workstation->selector.count; ++index) {
        if (workstation->selector.choices[index].selected) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(workstation->layout_dropdown),
                                       (guint)index);
            break;
        }
    }
    workstation->changing_selection = 0;
    refresh_heading(workstation);
    refresh_edit_controls(workstation);
    workstation->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_suite_gtk4_workstation_begin_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_begin_edit(
        &workstation->customisation);
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        refresh_edit_controls(workstation);
        workstation->revision += 1U;
    }
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_commit_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_commit_edit(
        &workstation->customisation);
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        refresh_edit_controls(workstation);
        workstation->revision += 1U;
    }
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_cancel_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_cancel_edit(
        &workstation->customisation);
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        refresh_edit_controls(workstation);
        workstation->revision += 1U;
    }
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_open_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *tool_id,
    const char *group_id,
    int floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity)
{
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_open_window(
        &workstation->customisation,
        tool_id,
        group_id,
        floating != 0,
        opened_at_ms,
        out_window_id,
        out_window_id_capacity);
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        workstation->revision += 1U;
    }
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_move_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    const char *group_id,
    double x,
    double y,
    double width,
    double height)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(
        &workstation->customisation);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_workspace_layout_set_group(layout, window_id, group_id);
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_set_placement(
            layout, window_id, group_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_layout_place_window(
            layout, window_id, x, y, width, height);
    }
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        workstation->revision += 1U;
    }
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_close_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(
        &workstation->customisation);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_workspace_layout_remove_window(layout, window_id);
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        workstation->revision += 1U;
    }
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_set_window_pinned(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int pinned)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    if (workstation == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_workspace_layout_set_pinned(
        layout, window_id, pinned != 0);
    if (status == UMI_STATUS_OK) status = rebuild_active_layout(workstation);
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_set_window_floating(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int floating)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    if (workstation == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_workspace_layout_set_floating(
        layout, window_id, floating != 0);
    if (status == UMI_STATUS_OK) status = rebuild_active_layout(workstation);
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_set_window_maximised(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int maximised)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    if (workstation == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    if (!workstation->customisation.edit_active)
        return UMI_STATUS_INVALID_STATE;
    status = umi_ui_workspace_layout_set_maximised(
        layout, window_id, maximised != 0);
    if (status == UMI_STATUS_OK) status = rebuild_active_layout(workstation);
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

UmiStatus umi_application_suite_gtk4_workstation_set_window_context_group(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    const char *context_group_id)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    if (workstation == NULL || window_id == NULL || context_group_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_workspace_layout_set_context_group(
        layout, window_id, context_group_id);
    if (status == UMI_STATUS_OK) status = rebuild_active_layout(workstation);
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

static void on_panel_action(const char *window_id,
                            UmiWsPanelAction action,
                            void *user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceWindow *window;
    if (workstation == NULL || window_id == NULL) return;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    if (window == NULL) return;
    switch (action) {
    case UMI_WS_PANEL_ACTION_PIN_TOGGLE:
        (void)umi_application_suite_gtk4_workstation_set_window_pinned(
            workstation, window_id, window->pinned ? 0 : 1);
        break;
    case UMI_WS_PANEL_ACTION_FLOAT_TOGGLE:
        (void)umi_application_suite_gtk4_workstation_set_window_floating(
            workstation, window_id, window->floating ? 0 : 1);
        break;
    case UMI_WS_PANEL_ACTION_MAXIMISE_TOGGLE:
        (void)umi_application_suite_gtk4_workstation_set_window_maximised(
            workstation, window_id, window->maximised ? 0 : 1);
        break;
    case UMI_WS_PANEL_ACTION_CLOSE:
        (void)umi_application_suite_gtk4_workstation_close_window(
            workstation, window_id);
        break;
    default:
        /* Move, context and settings need a chooser.  Their buttons expose a
         * stable action now; product-specific popovers can be attached later
         * without teaching panel chrome about application state. */
        break;
    }
}

static void on_layout_selected(GObject *object,
                               GParamSpec *pspec,
                               gpointer data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)data;
    guint selected;
    const UmiApplicationSuiteLayoutChoice *choice;
    (void)pspec;
    if (workstation == NULL || workstation->changing_selection) return;
    selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    choice = umi_application_suite_layout_selector_at(
        &workstation->selector, (size_t)selected);
    if (choice != NULL)
        (void)umi_application_suite_gtk4_workstation_select_layout(
            workstation, choice->layout_id);
}

static int text_contains_ignore_case(const char *text, const char *query)
{
    char *text_casefold;
    char *query_casefold;
    int matches;
    if (query == NULL || query[0] == '\0') return 1;
    if (text == NULL) return 0;
    text_casefold = g_utf8_casefold(text, -1);
    query_casefold = g_utf8_casefold(query, -1);
    matches = text_casefold != NULL && query_casefold != NULL &&
        strstr(text_casefold, query_casefold) != NULL;
    g_free(query_casefold);
    g_free(text_casefold);
    return matches;
}

static void refresh_new_window_filter(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    const char *query;
    guint category_index;
    UmiUiWindowCategory category;
    GtkWidget *row;

    if (workstation == NULL || workstation->new_window_list == NULL) return;
    query = gtk_editable_get_text(
        GTK_EDITABLE(workstation->new_window_search));
    category_index = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(workstation->new_window_category));
    category = category_index < G_N_ELEMENTS(WINDOW_CATEGORIES)
        ? WINDOW_CATEGORIES[category_index]
        : 0;
    row = gtk_widget_get_first_child(workstation->new_window_list);
    while (row != NULL) {
        GtkWidget *button = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row));
        const UmiUiWindowDescriptor *descriptor = button != NULL
            ? (const UmiUiWindowDescriptor *)g_object_get_data(
                  G_OBJECT(button), "umicom-window-descriptor")
            : NULL;
        GtkWidget *next = gtk_widget_get_next_sibling(row);
        int visible = descriptor != NULL &&
            (category == 0 || descriptor->category == category) &&
            (text_contains_ignore_case(descriptor->title, query) ||
             text_contains_ignore_case(descriptor->description, query) ||
             text_contains_ignore_case(descriptor->tool_id, query));
        gtk_widget_set_visible(row, visible);
        row = next;
    }
}

static void on_new_window_search_changed(
    GtkSearchEntry *entry,
    gpointer user_data)
{
    (void)entry;
    refresh_new_window_filter(
        (UmiApplicationSuiteGtk4Workstation *)user_data);
}

static void on_new_window_category_changed(
    GObject *object,
    GParamSpec *property,
    gpointer user_data)
{
    (void)object;
    (void)property;
    refresh_new_window_filter(
        (UmiApplicationSuiteGtk4Workstation *)user_data);
}

static void on_new_window_selected(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    const UmiUiWindowDescriptor *descriptor;
    guint region_index;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    int floating;
    UmiStatus status;

    if (workstation == NULL) return;
    descriptor = (const UmiUiWindowDescriptor *)g_object_get_data(
        G_OBJECT(button), "umicom-window-descriptor");
    if (descriptor == NULL) return;
    region_index = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(workstation->new_window_region));
    if (region_index >= G_N_ELEMENTS(WINDOW_REGIONS)) region_index = 0U;
    floating = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(workstation->new_window_floating));
    status = UMI_STATUS_NOT_FOUND;

    /* Singleton entries double as a simple panel mover. This mirrors the
     * discoverability of a New Window catalogue without creating duplicate
     * instances of tools that are intentionally unique. */
    if (!descriptor->supports_multiple) {
        UmiUiWorkspaceLayout *layout =
            umi_ui_workspace_customisation_active(&workstation->customisation);
        size_t index;
        for (index = 0U; layout != NULL && index < layout->window_count; ++index) {
            UmiUiWorkspaceWindow *window = &layout->windows[index];
            if (strcmp(window->tool_id, descriptor->tool_id) == 0) {
                UmiUiPlacement placement;
                UmiApplicationSuiteLayoutRect region;
                if (floating) {
                    placement = UMI_UI_PLACEMENT_FLOATING;
                } else if (umi_ui_placement_parse(
                               WINDOW_REGIONS[region_index], &placement) !=
                           UMI_STATUS_OK) {
                    placement = UMI_UI_PLACEMENT_CENTRE;
                }
                region = umi_application_suite_layout_region_rect(placement);
                status = umi_ui_workspace_layout_set_floating(
                    layout, window->window_id, floating != 0);
                if (status == UMI_STATUS_OK) {
                    status = umi_application_suite_gtk4_workstation_move_window(
                        workstation,
                        window->window_id,
                        WINDOW_REGIONS[region_index],
                        region.x,
                        region.y,
                        region.width,
                        region.height);
                }
                break;
            }
        }
    }
    if (status == UMI_STATUS_NOT_FOUND) {
        status = umi_application_suite_gtk4_workstation_open_window(
            workstation,
            descriptor->tool_id,
            WINDOW_REGIONS[region_index],
            floating,
            (uint64_t)(g_get_real_time() / 1000),
            window_id,
            sizeof(window_id));
    }
    if (status == UMI_STATUS_OK) {
        gtk_label_set_text(
            GTK_LABEL(workstation->new_window_status), "");
        gtk_popover_popdown(GTK_POPOVER(workstation->new_window_popover));
    } else if (workstation->new_window_status != NULL) {
        gtk_label_set_text(
            GTK_LABEL(workstation->new_window_status),
            status == UMI_STATUS_PERMISSION_DENIED ||
                    status == UMI_STATUS_INVALID_STATE
                ? "Unlock the layout before changing its windows."
                : (status == UMI_STATUS_CAPACITY_EXCEEDED
                    ? "This layout has reached its window capacity."
                    : "The selected window could not be added or moved."));
    }
}

static GtkWidget *build_new_window_popover(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    static const char *REGION_LABELS[] = {
        "Centre", "Left", "Right", "Bottom", "Top", NULL
    };
    static const char *CATEGORY_LABELS[] = {
        "All categories", "Development", "Navigation", "Operations",
        "Data", "AI", "Trading", "General", NULL
    };
    GtkWidget *popover = gtk_popover_new();
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *heading = gtk_label_new("New Window");
    GtkWidget *help = gtk_label_new(
        "Search Framework panels, choose a destination, then add or move "
        "one in the unlocked layout.");
    GtkWidget *options = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *scroll = gtk_scrolled_window_new();
    UmiUiWindowCatalogueQuery catalogue_query =
        umi_ui_window_catalogue_query_default();
    UmiUiWindowCatalogueResult catalogue_result = {0};
    size_t index;

    if (popover == NULL || root == NULL || heading == NULL || help == NULL ||
        options == NULL || scroll == NULL) {
        return popover;
    }
    workstation->new_window_popover = popover;
    workstation->new_window_search = gtk_search_entry_new();
    workstation->new_window_list = gtk_list_box_new();
    workstation->new_window_category = gtk_drop_down_new_from_strings(
        CATEGORY_LABELS);
    workstation->new_window_region = gtk_drop_down_new_from_strings(
        REGION_LABELS);
    workstation->new_window_floating = gtk_check_button_new_with_label(
        "Floating");
    workstation->new_window_status = gtk_label_new("");

    gtk_widget_add_css_class(root, "umicom-new-window-catalogue");
    gtk_widget_add_css_class(heading, "title-4");
    gtk_widget_add_css_class(help, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(help), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_widget_set_size_request(root, 440, 520);
    gtk_widget_set_margin_top(root, 12);
    gtk_widget_set_margin_bottom(root, 12);
    gtk_widget_set_margin_start(root, 12);
    gtk_widget_set_margin_end(root, 12);
    gtk_search_entry_set_placeholder_text(
        GTK_SEARCH_ENTRY(workstation->new_window_search),
        "Search windows and tools");
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(workstation->new_window_list), GTK_SELECTION_NONE);

    (void)umi_ui_window_catalogue_query(
        &workstation->customisation.windows,
        active_layout(workstation),
        &catalogue_query,
        &catalogue_result);
    for (index = 0U; index < catalogue_result.count; ++index) {
        const UmiUiWindowDescriptor *descriptor =
            catalogue_result.items[index].descriptor;
        GtkWidget *button = gtk_button_new();
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        GtkWidget *title = gtk_label_new(descriptor->title);
        GtkWidget *detail = gtk_label_new(descriptor->description);
        char category[96];

        if (button == NULL || row == NULL || title == NULL || detail == NULL)
            continue;
        (void)snprintf(
            category,
            sizeof(category),
            "%s%s",
            umi_ui_window_category_text(descriptor->category),
            descriptor->supports_multiple ? " · multiple allowed" : "");
        {
            GtkWidget *category_label = gtk_label_new(category);
            gtk_label_set_xalign(GTK_LABEL(category_label), 0.0F);
            gtk_widget_add_css_class(category_label, "dim-label");
            gtk_box_append(GTK_BOX(row), category_label);
        }
        gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(detail), 0.0F);
        gtk_label_set_wrap(GTK_LABEL(detail), TRUE);
        gtk_widget_add_css_class(detail, "dim-label");
        gtk_box_prepend(GTK_BOX(row), title);
        gtk_box_append(GTK_BOX(row), detail);
        gtk_button_set_child(GTK_BUTTON(button), row);
        g_object_set_data(
            G_OBJECT(button), "umicom-window-descriptor", (gpointer)descriptor);
        g_signal_connect(
            button, "clicked", G_CALLBACK(on_new_window_selected), workstation);
        gtk_list_box_append(GTK_LIST_BOX(workstation->new_window_list), button);
    }

    g_signal_connect(
        workstation->new_window_search,
        "search-changed",
        G_CALLBACK(on_new_window_search_changed),
        workstation);
    g_signal_connect(
        workstation->new_window_category,
        "notify::selected",
        G_CALLBACK(on_new_window_category_changed),
        workstation);
    gtk_widget_add_css_class(workstation->new_window_status, "error");
    gtk_label_set_xalign(GTK_LABEL(workstation->new_window_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(workstation->new_window_status), TRUE);
    gtk_box_append(GTK_BOX(options), gtk_label_new("Add to"));
    gtk_box_append(GTK_BOX(options), workstation->new_window_region);
    gtk_box_append(GTK_BOX(options), workstation->new_window_floating);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(scroll), workstation->new_window_list);
    gtk_box_append(GTK_BOX(root), heading);
    gtk_box_append(GTK_BOX(root), help);
    gtk_box_append(GTK_BOX(root), workstation->new_window_search);
    gtk_box_append(GTK_BOX(root), workstation->new_window_category);
    gtk_box_append(GTK_BOX(root), options);
    gtk_box_append(GTK_BOX(root), workstation->new_window_status);
    gtk_box_append(GTK_BOX(root), scroll);
    gtk_popover_set_child(GTK_POPOVER(popover), root);
    return popover;
}

static void on_edit_layout_clicked(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    (void)button;
    if (workstation == NULL) return;
    if (workstation->customisation.edit_active) {
        (void)umi_application_suite_gtk4_workstation_commit_layout_edit(
            workstation);
    } else {
        (void)umi_application_suite_gtk4_workstation_begin_layout_edit(
            workstation);
    }
}

static void on_cancel_layout_edit_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)umi_application_suite_gtk4_workstation_cancel_layout_edit(
        (UmiApplicationSuiteGtk4Workstation *)user_data);
}

UmiStatus umi_application_suite_gtk4_workstation_create(
    const UmiApplicationSuiteGtk4WorkstationConfig *config,
    UmiApplicationSuiteGtk4Workstation **out_workstation)
{
    UmiApplicationSuiteGtk4Workstation *workstation;
    const UmiUiWorkspaceLayout *layout;
    GtkStringList *choices;
    GtkWidget *header;
    GtkWidget *label;
    size_t index;
    UmiStatus status;
    if (config == NULL || out_workstation == NULL ||
        config->application_id == NULL || config->application_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_workstation = NULL;
    workstation = (UmiApplicationSuiteGtk4Workstation *)calloc(
        1U, sizeof(*workstation));
    if (workstation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_application_suite_layout_runtime_init(&workstation->runtime);
    status = umi_application_suite_layout_runtime_load(
        &workstation->runtime, config->application_id);
    if (status != UMI_STATUS_OK) goto fail;
    layout = umi_application_suite_layout_runtime_active(&workstation->runtime);
    if (layout == NULL) { status = UMI_STATUS_INVALID_STATE; goto fail; }
    status = umi_application_suite_layout_selector_build(
        workstation->runtime.experience, layout->layout_id, &workstation->selector);
    if (status != UMI_STATUS_OK) goto fail;

    /* Every suite product receives the same named-layout and New Window
     * control plane. Product panel factories still decide how each tool is
     * rendered, which keeps the Framework reusable and applications thin. */
    umi_ui_workspace_customisation_init(&workstation->customisation);
    status = umi_desktop_seed_window_catalogue(
        &workstation->customisation.windows);
    if (status != UMI_STATUS_OK) goto fail;
    for (index = 0U; index < workstation->selector.count; ++index) {
        UmiUiWorkspaceLayout projected;
        status = umi_application_suite_layout_project(
            workstation->runtime.experience,
            workstation->selector.choices[index].layout_id,
            &projected);
        if (status == UMI_STATUS_OK) {
            status = umi_ui_workspace_customisation_add_layout(
                &workstation->customisation, &projected);
        }
        if (status != UMI_STATUS_OK) goto fail;
    }
    status = umi_ui_workspace_customisation_activate(
        &workstation->customisation, layout->layout_id);
    if (status != UMI_STATUS_OK) goto fail;
    layout = active_layout(workstation);
    if (layout == NULL) { status = UMI_STATUS_INVALID_STATE; goto fail; }
    status = umi_gtk4_workspace_layout_host_create_interactive(
        layout,
        config->panel_factory,
        config->user_data,
        on_panel_action,
        workstation,
        &workstation->host);
    if (status != UMI_STATUS_OK) goto fail;

    workstation->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_object_ref_sink(workstation->root);
    header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    workstation->title_label = gtk_label_new(
        config->title != NULL ? config->title : layout->name);
    gtk_widget_add_css_class(header, "umicom-suite-layout-header");
    gtk_widget_add_css_class(workstation->title_label, "title-4");
    gtk_label_set_xalign(GTK_LABEL(workstation->title_label), 0.0F);
    gtk_widget_set_hexpand(workstation->title_label, TRUE);
    gtk_box_append(GTK_BOX(header), workstation->title_label);
    label = gtk_label_new("Layout");
    gtk_widget_add_css_class(label, "dim-label");
    gtk_box_append(GTK_BOX(header), label);

    choices = gtk_string_list_new(NULL);
    for (index = 0U; index < workstation->selector.count; ++index)
        gtk_string_list_append(choices, workstation->selector.choices[index].title);
    workstation->layout_dropdown = gtk_drop_down_new(G_LIST_MODEL(choices), NULL);
    g_object_unref(choices);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(workstation->layout_dropdown),
                               (guint)workstation->selector.selected_index);
    g_signal_connect(workstation->layout_dropdown, "notify::selected",
                     G_CALLBACK(on_layout_selected), workstation);
    gtk_box_append(GTK_BOX(header), workstation->layout_dropdown);

    workstation->new_window_button = gtk_menu_button_new();
    gtk_menu_button_set_label(
        GTK_MENU_BUTTON(workstation->new_window_button), "New Window");
    gtk_widget_set_tooltip_text(
        workstation->new_window_button,
        "Add a Framework panel to the unlocked layout");
    gtk_menu_button_set_popover(
        GTK_MENU_BUTTON(workstation->new_window_button),
        build_new_window_popover(workstation));
    gtk_box_append(GTK_BOX(header), workstation->new_window_button);

    workstation->edit_layout_button =
        gtk_button_new_with_label("Edit Layout");
    workstation->cancel_edit_button =
        gtk_button_new_with_label("Cancel");
    gtk_widget_add_css_class(workstation->cancel_edit_button, "flat");
    g_signal_connect(
        workstation->edit_layout_button,
        "clicked",
        G_CALLBACK(on_edit_layout_clicked),
        workstation);
    g_signal_connect(
        workstation->cancel_edit_button,
        "clicked",
        G_CALLBACK(on_cancel_layout_edit_clicked),
        workstation);
    gtk_box_append(GTK_BOX(header), workstation->cancel_edit_button);
    gtk_box_append(GTK_BOX(header), workstation->edit_layout_button);
    gtk_box_append(GTK_BOX(workstation->root), header);
    gtk_widget_set_hexpand(umi_gtk4_workspace_layout_host_widget(workstation->host),
                           TRUE);
    gtk_widget_set_vexpand(umi_gtk4_workspace_layout_host_widget(workstation->host),
                           TRUE);
    gtk_box_append(GTK_BOX(workstation->root),
                   umi_gtk4_workspace_layout_host_widget(workstation->host));
    refresh_heading(workstation);
    refresh_edit_controls(workstation);
    workstation->revision = 1U;
    *out_workstation = workstation;
    return UMI_STATUS_OK;

fail:
    umi_application_suite_gtk4_workstation_destroy(workstation);
    return status;
}

void umi_application_suite_gtk4_workstation_destroy(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    if (workstation == NULL) return;
    umi_gtk4_workspace_layout_host_destroy(workstation->host);
    workstation->host = NULL;
    if (workstation->root != NULL) g_object_unref(workstation->root);
    workstation->root = NULL;
    free(workstation);
}

GtkWidget *umi_application_suite_gtk4_workstation_widget(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    return workstation != NULL ? workstation->root : NULL;
}

UmiApplicationSuiteGtk4WorkstationSnapshot
umi_application_suite_gtk4_workstation_snapshot(
    const UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiApplicationSuiteGtk4WorkstationSnapshot snapshot;
    UmiGtk4WorkspaceLayoutHostSnapshot host_snapshot;
    const UmiApplicationSuiteLayoutChoice *choice;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    if (workstation == NULL) return snapshot;
    choice = umi_application_suite_layout_selector_current(&workstation->selector);
    host_snapshot = umi_gtk4_workspace_layout_host_snapshot(workstation->host);
    if (workstation->runtime.experience != NULL)
        (void)copy_text(snapshot.application_id, sizeof(snapshot.application_id),
                        workstation->runtime.experience->application_id);
    if (choice != NULL) {
        (void)copy_text(snapshot.active_layout_id,
                        sizeof(snapshot.active_layout_id), choice->layout_id);
        (void)copy_text(snapshot.active_layout_name,
                        sizeof(snapshot.active_layout_name), choice->title);
    }
    snapshot.layout_count = workstation->selector.count;
    snapshot.rendered_panel_count = host_snapshot.panel_count;
    snapshot.placeholder_count = host_snapshot.placeholder_count;
    snapshot.available_window_count = workstation->customisation.windows.count;
    snapshot.recent_window_count = workstation->customisation.windows.recent_count;
    snapshot.layout_locked = active_layout(workstation) != NULL &&
        active_layout(workstation)->locked;
    snapshot.editing_layout = workstation->customisation.edit_active;
    snapshot.revision = workstation->revision + host_snapshot.revision;
    return snapshot;
}
