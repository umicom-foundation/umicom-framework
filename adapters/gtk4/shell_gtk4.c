/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/shell_gtk4.c
 *
 * PURPOSE:
 *   Create the GTK4 application window and reusable IDE-style workbench hierarchy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The shell owns geometry only. Menus, commands, panes, documents and status
 * information still come from Framework models, which keeps this professional
 * layout reusable by Studio and by future Umicom applications.
 */

#include "gtk4_internal.h"

#define UMI_GTK4_ACTIVITY_RAIL_WIDTH 48
#define UMI_GTK4_MIN_TOOL_SIZE 160

static void configure_tool_notebook(GtkWidget *notebook,
                                    GtkPositionType tab_position)
{
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), tab_position);
    gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
    gtk_widget_add_css_class(notebook, "umicom-tool-notebook");
    gtk_widget_set_hexpand(notebook, TRUE);
    gtk_widget_set_vexpand(notebook, TRUE);
}

static void configure_editor_notebook(GtkWidget *notebook,
                                      const char *css_class)
{
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
    /* Matching group names enable native GTK tab drag-and-drop between the
     * two editor groups.  Model synchronisation remains in editor_gtk4.c. */
    gtk_notebook_set_group_name(GTK_NOTEBOOK(notebook),
                                "umicom-editor-workbench");
    gtk_widget_add_css_class(notebook, "umicom-document-notebook");
    gtk_widget_add_css_class(notebook, css_class);
    gtk_widget_set_hexpand(notebook, TRUE);
    gtk_widget_set_vexpand(notebook, TRUE);
}

static GtkWidget *brand_icon_for_workbench(UmiUiWorkbench *workbench)
{
    UmiUiContextSnapshot value;
    GtkWidget *picture;
    if (workbench != NULL &&
        umi_ui_context_get(umi_ui_workbench_context(workbench),
                           "studio.brand.icon-path", &value) ==
            UMI_STATUS_OK &&
        value.kind == UMI_UI_CONTEXT_STRING &&
        value.string_value[0] != '\0' &&
        g_file_test(value.string_value, G_FILE_TEST_IS_REGULAR)) {
        picture = gtk_picture_new_for_filename(value.string_value);
        gtk_picture_set_content_fit(GTK_PICTURE(picture),
                                    GTK_CONTENT_FIT_CONTAIN);
        gtk_widget_set_size_request(picture, 24, 24);
        gtk_widget_add_css_class(picture, "umicom-brand-icon");
        gtk_accessible_update_property(
            GTK_ACCESSIBLE(picture), GTK_ACCESSIBLE_PROPERTY_LABEL,
            "Umicom trademark", -1);
        return picture;
    }
    picture = gtk_image_new_from_icon_name("applications-development-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(picture), 18);
    return picture;
}

static void on_splitter_position_changed(GObject *object,
                                         GParamSpec *property,
                                         gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchState state;
    GtkWidget *splitter = GTK_WIDGET(object);
    int position;
    int available;
    int changed = 0;

    (void)property;
    if (adapter == NULL || adapter->shell == NULL ||
        adapter->applying_layout_state) {
        return;
    }

    workbench = umi_ui_application_shell_workbench(adapter->shell);
    if (umi_ui_workbench_state_snapshot(workbench, &state) != UMI_STATUS_OK) {
        return;
    }

    position = gtk_paned_get_position(GTK_PANED(splitter));
    if (splitter == adapter->middle_paned) {
        int sidebar_size = position - UMI_GTK4_ACTIVITY_RAIL_WIDTH;
        if (sidebar_size >= UMI_GTK4_MIN_TOOL_SIZE &&
            sidebar_size != state.sidebar_size) {
            state.sidebar_size = sidebar_size;
            changed = 1;
        }
    } else if (splitter == adapter->centre_paned) {
        available = gtk_widget_get_width(splitter);
        if (available - position >= UMI_GTK4_MIN_TOOL_SIZE &&
            available - position != state.auxiliary_sidebar_size) {
            state.auxiliary_sidebar_size = available - position;
            changed = 1;
        }
    } else if (splitter == adapter->content_paned) {
        available = gtk_widget_get_height(splitter);
        if (available - position >= UMI_GTK4_MIN_TOOL_SIZE &&
            available - position != state.bottom_panel_size) {
            state.bottom_panel_size = available - position;
            changed = 1;
        }
    } else if (splitter == adapter->editor_paned &&
               state.editor_split_mode != UMI_UI_EDITOR_SPLIT_SINGLE) {
        available = state.editor_split_mode == UMI_UI_EDITOR_SPLIT_COLUMNS
            ? gtk_widget_get_width(splitter)
            : gtk_widget_get_height(splitter);
        if (available > 0) {
            int ratio = (int)(((int64_t)position * 10000) / available);
            if (ratio >= UMI_UI_EDITOR_SPLIT_RATIO_MIN &&
                ratio <= UMI_UI_EDITOR_SPLIT_RATIO_MAX &&
                ratio != state.editor_split_ratio) {
                state.editor_split_ratio = ratio;
                changed = 1;
            }
        }
    }

    if (changed) {
        /* Only chrome geometry changed; avoid replaying navigation commands
         * while the user drags a splitter. */
        state.active_activity[0] = '\0';
        state.active_view_container[0] = '\0';
        state.active_perspective[0] = '\0';
        state.active_document[0] = '\0';
        (void)umi_ui_workbench_state_apply(workbench, &state);
    }
}

void umi_gtk4_clear_box(GtkWidget *box)
{
    GtkWidget *child;
    if (box == NULL || !GTK_IS_BOX(box)) return;
    child = gtk_widget_get_first_child(box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(box), child);
        child = next;
    }
}

UmiStatus umi_gtk4_build_shell(UmiGtk4Adapter *adapter)
{
    GtkWidget *left_cluster;
    GtkWidget *centre_box;
    GtkWidget *project_icon;
    GtkWidget *project_title;
    GtkWidget *toolbar_spacer;
    GtkWidget *profile_content;
    GtkWidget *profile_icon;
    GtkWidget *appearance_content;
    GtkWidget *appearance_icon;
    GtkWidget *status_context;
    GtkWidget *layout_scroller;
    UmiUiWorkbench *workbench;

    if (adapter == NULL || adapter->application == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    workbench = adapter->shell != NULL
        ? umi_ui_application_shell_workbench(adapter->shell)
        : NULL;
    adapter->window = GTK_WINDOW(gtk_application_window_new(adapter->application));
    gtk_widget_add_css_class(GTK_WIDGET(adapter->window), "umicom-workbench");
    adapter->root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(adapter->root_box, "umicom-workbench-root");

    adapter->menu_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(adapter->menu_bar, "umicom-menubar");

    adapter->toolbar_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(adapter->toolbar_box, "umicom-main-toolbar");

    adapter->project_widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 7);
    gtk_widget_add_css_class(adapter->project_widget, "umicom-project-widget");
    project_icon = brand_icon_for_workbench(workbench);
    project_title = gtk_label_new("Umicom Studio");
    gtk_widget_add_css_class(project_title, "umicom-project-title");
    gtk_box_append(GTK_BOX(adapter->project_widget), project_icon);
    gtk_box_append(GTK_BOX(adapter->project_widget), project_title);
    gtk_box_append(GTK_BOX(adapter->toolbar_box), adapter->project_widget);

    adapter->toolbar_actions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_add_css_class(adapter->toolbar_actions_box,
                             "umicom-toolbar-actions");
    gtk_box_append(GTK_BOX(adapter->toolbar_box), adapter->toolbar_actions_box);

    toolbar_spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(toolbar_spacer, TRUE);
    gtk_box_append(GTK_BOX(adapter->toolbar_box), toolbar_spacer);

    /*
     * Named workspace profiles are Framework data rendered by the adapter.
     * Keeping the picker permanently visible mirrors the quick layout tabs in
     * professional IDEs and trading workstations without coupling this shell
     * to any Studio-specific profile name.
     */
    adapter->workspace_profile_button = gtk_menu_button_new();
    gtk_widget_add_css_class(adapter->workspace_profile_button,
                             "umicom-workspace-profile-button");
    profile_content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    profile_icon = gtk_image_new_from_icon_name("view-grid-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(profile_icon), 15);
    adapter->workspace_profile_label = gtk_label_new("Layout");
    gtk_box_append(GTK_BOX(profile_content), profile_icon);
    gtk_box_append(GTK_BOX(profile_content), adapter->workspace_profile_label);
    gtk_menu_button_set_child(GTK_MENU_BUTTON(
        adapter->workspace_profile_button), profile_content);
    gtk_widget_set_tooltip_text(adapter->workspace_profile_button,
                                "Switch the active workspace layout");
    gtk_box_append(GTK_BOX(adapter->toolbar_box),
                   adapter->workspace_profile_button);

    /* Appearance is a first-class workbench preference rather than a hidden
     * text setting.  The popover is populated from Framework profiles during
     * refresh, alongside the saved-layout selector. */
    adapter->appearance_button = gtk_menu_button_new();
    gtk_widget_add_css_class(adapter->appearance_button,
                             "umicom-appearance-button");
    appearance_content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    appearance_icon = gtk_image_new_from_icon_name("applications-graphics-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(appearance_icon), 15);
    adapter->appearance_label = gtk_label_new("Appearance");
    gtk_box_append(GTK_BOX(appearance_content), appearance_icon);
    gtk_box_append(GTK_BOX(appearance_content), adapter->appearance_label);
    gtk_menu_button_set_child(GTK_MENU_BUTTON(adapter->appearance_button),
                              appearance_content);
    gtk_widget_set_tooltip_text(adapter->appearance_button,
                                "Choose theme, density and typography");
    gtk_box_append(GTK_BOX(adapter->toolbar_box), adapter->appearance_button);

    /* Command palette / quick access stays visible but compact in the toolbar. */
    adapter->quick_access_entry = gtk_search_entry_new();
    gtk_search_entry_set_placeholder_text(
        GTK_SEARCH_ENTRY(adapter->quick_access_entry),
        "Search commands and actions (Ctrl+Shift+P)"
    );
    gtk_widget_set_size_request(adapter->quick_access_entry, 340, -1);
    gtk_widget_add_css_class(adapter->quick_access_entry,
                             "umicom-command-search");
    gtk_box_append(GTK_BOX(adapter->toolbar_box), adapter->quick_access_entry);

    /* Results are a normal list rather than a toolkit-specific data source. */
    adapter->quick_access_list = gtk_list_box_new();
    gtk_widget_set_visible(adapter->quick_access_list, FALSE);
    gtk_widget_add_css_class(adapter->quick_access_list,
                             "umicom-command-results");

    adapter->content_paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    adapter->middle_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    adapter->centre_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_add_css_class(adapter->content_paned, "umicom-primary-split");
    gtk_widget_add_css_class(adapter->middle_paned, "umicom-primary-split");
    gtk_widget_add_css_class(adapter->centre_paned, "umicom-primary-split");
    gtk_paned_set_wide_handle(GTK_PANED(adapter->content_paned), TRUE);
    gtk_paned_set_wide_handle(GTK_PANED(adapter->middle_paned), TRUE);
    gtk_paned_set_wide_handle(GTK_PANED(adapter->centre_paned), TRUE);

    left_cluster = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(left_cluster, "umicom-left-cluster");
    adapter->activity_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_size_request(adapter->activity_box,
                                UMI_GTK4_ACTIVITY_RAIL_WIDTH,
                                -1);
    gtk_widget_add_css_class(adapter->activity_box, "umicom-activity-rail");

    adapter->sidebar_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(adapter->sidebar_box, "umicom-primary-sidebar");
    adapter->sidebar_header = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(adapter->sidebar_header), 0.0F);
    gtk_label_set_ellipsize(GTK_LABEL(adapter->sidebar_header),
                            PANGO_ELLIPSIZE_END);
    gtk_widget_add_css_class(adapter->sidebar_header,
                             "umicom-tool-header");
    adapter->left_box = gtk_notebook_new();
    configure_tool_notebook(adapter->left_box, GTK_POS_TOP);
    umi_gtk4_configure_dock_notebook(adapter,
                                     adapter->left_box,
                                     UMI_UI_PLACEMENT_LEFT);
    gtk_widget_set_size_request(adapter->sidebar_box,
                                UMI_GTK4_MIN_TOOL_SIZE,
                                -1);
    gtk_box_append(GTK_BOX(adapter->sidebar_box), adapter->sidebar_header);
    gtk_box_append(GTK_BOX(adapter->sidebar_box), adapter->left_box);

    gtk_box_append(GTK_BOX(left_cluster), adapter->activity_box);
    gtk_box_append(GTK_BOX(left_cluster), adapter->sidebar_box);

    centre_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(centre_box, "umicom-editor-area");
    adapter->breadcrumb_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(adapter->breadcrumb_box, "umicom-breadcrumbs");
    adapter->document_notebook = gtk_notebook_new();
    adapter->secondary_document_notebook = gtk_notebook_new();
    configure_editor_notebook(adapter->document_notebook,
                              "umicom-editor-group-primary");
    configure_editor_notebook(adapter->secondary_document_notebook,
                              "umicom-editor-group-secondary");
    adapter->editor_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_add_css_class(adapter->editor_paned,
                             "umicom-editor-split");
    gtk_paned_set_wide_handle(GTK_PANED(adapter->editor_paned), TRUE);
    gtk_paned_set_start_child(GTK_PANED(adapter->editor_paned),
                              adapter->document_notebook);
    gtk_paned_set_end_child(GTK_PANED(adapter->editor_paned),
                            adapter->secondary_document_notebook);
    gtk_paned_set_resize_start_child(GTK_PANED(adapter->editor_paned), TRUE);
    gtk_paned_set_resize_end_child(GTK_PANED(adapter->editor_paned), TRUE);
    gtk_paned_set_shrink_start_child(GTK_PANED(adapter->editor_paned), FALSE);
    gtk_paned_set_shrink_end_child(GTK_PANED(adapter->editor_paned), FALSE);
    gtk_widget_set_visible(adapter->secondary_document_notebook, FALSE);
    gtk_box_append(GTK_BOX(centre_box), adapter->breadcrumb_box);
    gtk_box_append(GTK_BOX(centre_box), adapter->editor_paned);

    adapter->right_box = gtk_notebook_new();
    configure_tool_notebook(adapter->right_box, GTK_POS_TOP);
    umi_gtk4_configure_dock_notebook(adapter,
                                     adapter->right_box,
                                     UMI_UI_PLACEMENT_RIGHT);
    gtk_widget_add_css_class(adapter->right_box, "umicom-auxiliary-sidebar");
    gtk_widget_set_size_request(adapter->right_box,
                                UMI_GTK4_MIN_TOOL_SIZE,
                                -1);

    adapter->bottom_box = gtk_notebook_new();
    configure_tool_notebook(adapter->bottom_box, GTK_POS_TOP);
    umi_gtk4_configure_dock_notebook(adapter,
                                     adapter->bottom_box,
                                     UMI_UI_PLACEMENT_BOTTOM);
    gtk_widget_add_css_class(adapter->bottom_box, "umicom-bottom-panel");
    gtk_widget_set_size_request(adapter->bottom_box,
                                -1,
                                UMI_GTK4_MIN_TOOL_SIZE);

    adapter->notification_label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(adapter->notification_label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(adapter->notification_label), TRUE);
    gtk_widget_add_css_class(adapter->notification_label,
                             "umicom-notification-banner");
    gtk_widget_set_visible(adapter->notification_label, FALSE);

    adapter->status_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_add_css_class(adapter->status_box, "umicom-statusbar");
    adapter->status_label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(adapter->status_label), 0.0F);
    gtk_label_set_ellipsize(GTK_LABEL(adapter->status_label),
                            PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(adapter->status_label, TRUE);
    status_context = gtk_label_new("Framework Workbench");
    gtk_widget_add_css_class(status_context, "umicom-status-context");
    gtk_box_append(GTK_BOX(adapter->status_box), adapter->status_label);
    gtk_box_append(GTK_BOX(adapter->status_box), status_context);

    /* Framework-owned application layouts are first-class desktop tabs at the
     * bottom of the canvas, following the proven multi-workspace interaction
     * used by professional trading terminals. */
    adapter->desktop_layout_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(adapter->desktop_layout_bar,
                             "umicom-desktop-layout-bar");
    adapter->desktop_layout_tabs_box = gtk_box_new(
        GTK_ORIENTATION_HORIZONTAL, 2);
    layout_scroller = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(layout_scroller),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_NEVER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(layout_scroller),
                                  adapter->desktop_layout_tabs_box);
    gtk_widget_set_hexpand(layout_scroller, TRUE);
    gtk_box_append(GTK_BOX(adapter->desktop_layout_bar), layout_scroller);
    adapter->desktop_monitor_label = gtk_label_new("");
    gtk_widget_add_css_class(adapter->desktop_monitor_label,
                             "umicom-desktop-monitor-label");
    gtk_box_append(GTK_BOX(adapter->desktop_layout_bar),
                   adapter->desktop_monitor_label);
    adapter->desktop_designer_button = gtk_menu_button_new();
    gtk_menu_button_set_icon_name(
        GTK_MENU_BUTTON(adapter->desktop_designer_button),
        "view-grid-symbolic");
    gtk_widget_set_tooltip_text(adapter->desktop_designer_button,
                                "Open the visual desktop layout designer");
    gtk_widget_add_css_class(adapter->desktop_designer_button,
                             "umicom-desktop-designer-button");
    gtk_box_append(GTK_BOX(adapter->desktop_layout_bar),
                   adapter->desktop_designer_button);
    gtk_widget_set_visible(adapter->desktop_layout_bar, FALSE);

    gtk_paned_set_start_child(GTK_PANED(adapter->centre_paned), centre_box);
    gtk_paned_set_end_child(GTK_PANED(adapter->centre_paned), adapter->right_box);
    gtk_paned_set_resize_start_child(GTK_PANED(adapter->centre_paned), TRUE);
    gtk_paned_set_resize_end_child(GTK_PANED(adapter->centre_paned), FALSE);
    gtk_paned_set_shrink_start_child(GTK_PANED(adapter->centre_paned), FALSE);
    gtk_paned_set_shrink_end_child(GTK_PANED(adapter->centre_paned), FALSE);

    gtk_paned_set_start_child(GTK_PANED(adapter->middle_paned), left_cluster);
    gtk_paned_set_end_child(GTK_PANED(adapter->middle_paned),
                            adapter->centre_paned);
    gtk_paned_set_resize_start_child(GTK_PANED(adapter->middle_paned), FALSE);
    gtk_paned_set_resize_end_child(GTK_PANED(adapter->middle_paned), TRUE);
    gtk_paned_set_shrink_start_child(GTK_PANED(adapter->middle_paned), FALSE);
    gtk_paned_set_shrink_end_child(GTK_PANED(adapter->middle_paned), FALSE);

    gtk_paned_set_start_child(GTK_PANED(adapter->content_paned),
                              adapter->middle_paned);
    gtk_paned_set_end_child(GTK_PANED(adapter->content_paned), adapter->bottom_box);
    gtk_paned_set_resize_start_child(GTK_PANED(adapter->content_paned), TRUE);
    gtk_paned_set_resize_end_child(GTK_PANED(adapter->content_paned), FALSE);
    gtk_paned_set_shrink_start_child(GTK_PANED(adapter->content_paned), FALSE);
    gtk_paned_set_shrink_end_child(GTK_PANED(adapter->content_paned), FALSE);

    gtk_box_append(GTK_BOX(adapter->root_box), adapter->menu_bar);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->toolbar_box);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->quick_access_list);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->notification_label);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->content_paned);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->desktop_layout_bar);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->status_box);
    gtk_window_set_child(adapter->window, adapter->root_box);

    g_signal_connect(adapter->middle_paned,
                     "notify::position",
                     G_CALLBACK(on_splitter_position_changed),
                     adapter);
    g_signal_connect(adapter->centre_paned,
                     "notify::position",
                     G_CALLBACK(on_splitter_position_changed),
                     adapter);
    g_signal_connect(adapter->content_paned,
                     "notify::position",
                     G_CALLBACK(on_splitter_position_changed),
                     adapter);
    g_signal_connect(adapter->editor_paned,
                     "notify::position",
                     G_CALLBACK(on_splitter_position_changed),
                     adapter);

    g_signal_connect(adapter->quick_access_entry,
                     "search-changed",
                     G_CALLBACK(umi_gtk4_on_quick_access_changed),
                     adapter);
    g_signal_connect(adapter->quick_access_entry,
                     "activate",
                     G_CALLBACK(umi_gtk4_on_quick_access_activate),
                     adapter);
    g_signal_connect(adapter->quick_access_list,
                     "row-activated",
                     G_CALLBACK(umi_gtk4_on_quick_access_row_activated),
                     adapter);

    /*
     * Install one window-level key controller. It delegates all shortcut policy
     * to Framework's context-sensitive keybinding registry.
     */
    return umi_gtk4_install_keybindings(adapter);
}
