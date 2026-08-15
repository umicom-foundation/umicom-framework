/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workspace_profile_gtk4.c
 *
 * PURPOSE:
 *   Render the toolkit-neutral workspace-profile catalogue as a compact GTK4
 *   layout picker in the main workbench toolbar.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The picker contains no Studio-specific profile names. It reads Framework
 * snapshots, asks the Framework workbench to activate the selected profile,
 * and then uses the normal full refresh path to update every visible region.
 */

#include "gtk4_internal.h"

#include <stdio.h>

typedef struct UmiGtk4ProfileDesignerControls {
    UmiGtk4Adapter *adapter;
    char profile_id[UMI_UI_ID_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    GtkWidget *name_entry;
    GtkWidget *sidebar_check;
    GtkWidget *auxiliary_check;
    GtkWidget *bottom_check;
    GtkWidget *sidebar_size;
    GtkWidget *auxiliary_size;
    GtkWidget *bottom_size;
} UmiGtk4ProfileDesignerControls;

static UmiUiWorkbench *designer_workbench(
    UmiGtk4ProfileDesignerControls *controls)
{
    if (controls == NULL || controls->adapter == NULL ||
        controls->adapter->shell == NULL) {
        return NULL;
    }
    return umi_ui_application_shell_workbench(controls->adapter->shell);
}

static UmiStatus apply_designer_values(
    UmiGtk4ProfileDesignerControls *controls)
{
    UmiUiWorkbench *workbench = designer_workbench(controls);
    UmiUiWorkbenchState state;
    UmiStatus status;
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workbench_state_snapshot(workbench, &state);
    if (status != UMI_STATUS_OK) return status;

    state.sidebar_visible = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(controls->sidebar_check));
    state.auxiliary_sidebar_visible = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(controls->auxiliary_check));
    state.bottom_panel_visible = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(controls->bottom_check));
    state.sidebar_size = gtk_spin_button_get_value_as_int(
        GTK_SPIN_BUTTON(controls->sidebar_size));
    state.auxiliary_sidebar_size = gtk_spin_button_get_value_as_int(
        GTK_SPIN_BUTTON(controls->auxiliary_size));
    state.bottom_panel_size = gtk_spin_button_get_value_as_int(
        GTK_SPIN_BUTTON(controls->bottom_size));

    /* Geometry editing must not replay navigation actions or reapply the
     * selected preset before the user's new values are committed. */
    state.active_activity[0] = '\0';
    state.active_view_container[0] = '\0';
    state.active_perspective[0] = '\0';
    state.active_document[0] = '\0';
    state.active_workspace_profile[0] = '\0';
    return umi_ui_workbench_state_apply(workbench, &state);
}

static void on_designer_apply(GtkButton *button, gpointer user_data)
{
    UmiGtk4ProfileDesignerControls *controls =
        (UmiGtk4ProfileDesignerControls *)user_data;
    (void)button;
    if (apply_designer_values(controls) == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_workbench(controls->adapter);
    }
}

static void on_designer_save_as(GtkButton *button, gpointer user_data)
{
    UmiGtk4ProfileDesignerControls *controls =
        (UmiGtk4ProfileDesignerControls *)user_data;
    UmiUiWorkbench *workbench = designer_workbench(controls);
    const char *label;
    char profile_id[UMI_UI_ID_CAPACITY];
    (void)button;
    if (workbench == NULL || apply_designer_values(controls) !=
        UMI_STATUS_OK) {
        return;
    }
    label = gtk_editable_get_text(GTK_EDITABLE(controls->name_entry));
    if (label == NULL || label[0] == '\0') label = "My Layout";
    if (umi_ui_workbench_save_workspace_profile(
            workbench,
            label,
            "User-created Umicom Studio workspace layout",
            profile_id,
            sizeof(profile_id)) == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_workbench(controls->adapter);
    }
}

static void on_designer_update(GtkButton *button, gpointer user_data)
{
    UmiGtk4ProfileDesignerControls *controls =
        (UmiGtk4ProfileDesignerControls *)user_data;
    UmiUiWorkbench *workbench = designer_workbench(controls);
    const char *label;
    (void)button;
    if (workbench == NULL || apply_designer_values(controls) !=
        UMI_STATUS_OK) {
        return;
    }
    label = gtk_editable_get_text(GTK_EDITABLE(controls->name_entry));
    if (label == NULL || label[0] == '\0') return;
    if (umi_ui_workbench_update_workspace_profile(
            workbench,
            controls->profile_id,
            label,
            controls->description) == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_workbench(controls->adapter);
    }
}

static void on_designer_lock(GtkButton *button, gpointer user_data)
{
    UmiGtk4ProfileDesignerControls *controls =
        (UmiGtk4ProfileDesignerControls *)user_data;
    UmiUiWorkbench *workbench = designer_workbench(controls);
    UmiUiWorkspaceProfileSnapshot profile;
    (void)button;
    if (workbench == NULL ||
        umi_ui_workspace_profile_model_find(
            umi_ui_workbench_workspace_profiles(workbench),
            controls->profile_id,
            &profile) != UMI_STATUS_OK) {
        return;
    }
    if (umi_ui_workbench_set_workspace_profile_locked(
            workbench, controls->profile_id, !profile.locked) ==
        UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_workbench(controls->adapter);
    }
}

static void on_designer_delete(GtkButton *button, gpointer user_data)
{
    UmiGtk4ProfileDesignerControls *controls =
        (UmiGtk4ProfileDesignerControls *)user_data;
    UmiUiWorkbench *workbench = designer_workbench(controls);
    (void)button;
    if (workbench != NULL &&
        umi_ui_workbench_remove_workspace_profile(
            workbench, controls->profile_id) == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_workbench(controls->adapter);
    }
}

static void on_designer_reset(GtkButton *button, gpointer user_data)
{
    UmiGtk4ProfileDesignerControls *controls =
        (UmiGtk4ProfileDesignerControls *)user_data;
    UmiUiWorkbench *workbench = designer_workbench(controls);
    (void)button;
    if (workbench != NULL &&
        umi_ui_workbench_activate_workspace_profile(
            workbench, controls->profile_id) == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_workbench(controls->adapter);
    }
}

static GtkWidget *create_region_row(const char *label,
                                    int visible,
                                    int size,
                                    GtkWidget **out_check,
                                    GtkWidget **out_size)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *check = gtk_check_button_new_with_label(label);
    GtkWidget *spin = gtk_spin_button_new_with_range(160.0, 720.0, 10.0);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(check), visible != 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (double)size);
    gtk_widget_set_hexpand(check, TRUE);
    gtk_widget_set_tooltip_text(spin, "Preferred region size in pixels");
    gtk_box_append(GTK_BOX(row), check);
    gtk_box_append(GTK_BOX(row), spin);
    *out_check = check;
    *out_size = spin;
    return row;
}

static GtkWidget *create_profile_designer(
    UmiGtk4Adapter *adapter,
    const UmiUiWorkspaceProfileSnapshot *profile,
    const UmiUiWorkbenchState *state)
{
    UmiGtk4ProfileDesignerControls *controls =
        g_new0(UmiGtk4ProfileDesignerControls, 1);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *heading = gtk_label_new("Customise Layout");
    GtkWidget *help = gtk_label_new(
        "Drag tool tabs between the left, right and bottom regions. "
        "Drag splitters to resize them, then save your own layout.");
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *management_buttons = NULL;
    GtkWidget *apply = gtk_button_new_with_label("Apply");
    GtkWidget *save_as = gtk_button_new_with_label("Save as Copy");
    GtkWidget *update = gtk_button_new_with_label("Update");
    GtkWidget *reset = gtk_button_new_with_label("Reset");
    GtkWidget *lock = NULL;
    GtkWidget *delete_button = NULL;
    char copy_name[UMI_UI_TEXT_CAPACITY];

    controls->adapter = adapter;
    (void)snprintf(controls->profile_id,
                   sizeof(controls->profile_id),
                   "%s",
                   profile->profile_id);
    (void)snprintf(controls->description,
                   sizeof(controls->description),
                   "%s",
                   profile->description);

    gtk_widget_add_css_class(box, "umicom-layout-designer");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_widget_add_css_class(heading, "umicom-layout-designer-title");
    gtk_label_set_xalign(GTK_LABEL(help), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(help), 42);
    gtk_widget_add_css_class(help, "umicom-workspace-profile-description");
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), help);

    (void)snprintf(copy_name, sizeof(copy_name), "%s%s",
                   profile->label,
                   profile->built_in ? " Copy" : "");
    controls->name_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(controls->name_entry), copy_name);
    gtk_entry_set_placeholder_text(GTK_ENTRY(controls->name_entry),
                                   "Layout name");
    gtk_box_append(GTK_BOX(box), controls->name_entry);
    gtk_box_append(GTK_BOX(box), create_region_row(
        "Primary sidebar", state->sidebar_visible, state->sidebar_size,
        &controls->sidebar_check, &controls->sidebar_size));
    gtk_box_append(GTK_BOX(box), create_region_row(
        "Auxiliary sidebar", state->auxiliary_sidebar_visible,
        state->auxiliary_sidebar_size, &controls->auxiliary_check,
        &controls->auxiliary_size));
    gtk_box_append(GTK_BOX(box), create_region_row(
        "Bottom panel", state->bottom_panel_visible,
        state->bottom_panel_size, &controls->bottom_check,
        &controls->bottom_size));

    gtk_box_append(GTK_BOX(buttons), apply);
    gtk_box_append(GTK_BOX(buttons), save_as);
    gtk_box_append(GTK_BOX(buttons), update);
    gtk_box_append(GTK_BOX(buttons), reset);
    gtk_box_set_homogeneous(GTK_BOX(buttons), TRUE);
    gtk_widget_add_css_class(save_as, "suggested-action");
    gtk_widget_set_tooltip_text(
        apply, "Preview these region settings without replacing a layout");
    gtk_widget_set_tooltip_text(
        save_as, "Create a new custom layout from the current workspace");
    gtk_widget_set_tooltip_text(
        update, "Replace this unlocked custom layout with the current workspace");
    gtk_widget_set_tooltip_text(
        reset, "Discard unsaved layout changes and restore this profile");
    if (!profile->built_in) {
        management_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        lock = gtk_button_new_with_label(
            profile->locked ? "Unlock" : "Lock");
        delete_button = gtk_button_new_with_label("Delete");
        gtk_box_append(GTK_BOX(management_buttons), lock);
        gtk_box_append(GTK_BOX(management_buttons), delete_button);
        gtk_box_set_homogeneous(GTK_BOX(management_buttons), TRUE);
        gtk_widget_add_css_class(delete_button, "destructive-action");
        gtk_widget_set_sensitive(delete_button, !profile->locked);
        g_signal_connect(lock, "clicked",
                         G_CALLBACK(on_designer_lock), controls);
        g_signal_connect(delete_button, "clicked",
                         G_CALLBACK(on_designer_delete), controls);
    }
    gtk_widget_set_sensitive(update,
                             !profile->built_in && !profile->locked);
    gtk_box_append(GTK_BOX(box), buttons);
    if (management_buttons != NULL) {
        gtk_box_append(GTK_BOX(box), management_buttons);
    }

    g_signal_connect(apply, "clicked",
                     G_CALLBACK(on_designer_apply), controls);
    g_signal_connect(save_as, "clicked",
                     G_CALLBACK(on_designer_save_as), controls);
    g_signal_connect(update, "clicked",
                     G_CALLBACK(on_designer_update), controls);
    g_signal_connect(reset, "clicked",
                     G_CALLBACK(on_designer_reset), controls);
    g_object_set_data_full(G_OBJECT(box),
                           "umicom-layout-designer-controls",
                           controls,
                           g_free);
    return box;
}

static void on_workspace_profile_clicked(GtkButton *button,
                                         gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *profile_id;
    UmiUiWorkbench *workbench;
    GtkWidget *popover;

    if (adapter == NULL || adapter->shell == NULL) return;
    profile_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-workspace-profile-id");
    if (profile_id == NULL) return;

    workbench = umi_ui_application_shell_workbench(adapter->shell);
    if (umi_ui_workbench_activate_workspace_profile(workbench, profile_id) !=
        UMI_STATUS_OK) {
        return;
    }

    popover = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_POPOVER);
    if (popover != NULL) gtk_popover_popdown(GTK_POPOVER(popover));
    (void)umi_gtk4_refresh_workbench(adapter);
}

static GtkWidget *create_profile_button(
    UmiGtk4Adapter *adapter,
    const UmiUiWorkspaceProfileSnapshot *profile)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    GtkWidget *title = gtk_label_new(profile->label);
    GtkWidget *description = gtk_label_new(profile->description);
    GtkWidget *state_icon;

    if (profile->icon_name[0] != '\0') {
        GtkWidget *icon = gtk_image_new_from_icon_name(profile->icon_name);
        gtk_image_set_pixel_size(GTK_IMAGE(icon), 17);
        gtk_box_append(GTK_BOX(row), icon);
    }

    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(description), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(description), 38);
    gtk_widget_add_css_class(description,
                             "umicom-workspace-profile-description");
    gtk_box_append(GTK_BOX(text), title);
    gtk_box_append(GTK_BOX(text), description);
    gtk_widget_set_hexpand(text, TRUE);
    gtk_box_append(GTK_BOX(row), text);

    state_icon = gtk_image_new_from_icon_name("object-select-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(state_icon), 15);
    gtk_widget_set_opacity(state_icon, profile->active ? 1.0 : 0.0);
    gtk_box_append(GTK_BOX(row), state_icon);

    gtk_button_set_child(GTK_BUTTON(button), row);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-workspace-profile-item");
    if (profile->active) gtk_widget_add_css_class(button, "active");
    gtk_widget_set_tooltip_text(button, profile->description);
    g_object_set_data_full(G_OBJECT(button),
                           "umicom-workspace-profile-id",
                           g_strdup(profile->profile_id),
                           g_free);
    g_signal_connect(button,
                     "clicked",
                     G_CALLBACK(on_workspace_profile_clicked),
                     adapter);
    return button;
}

UmiStatus umi_gtk4_refresh_workspace_profiles(
    UmiGtk4Adapter *adapter,
    UmiUiWorkbench *workbench)
{
    UmiUiWorkspaceProfileModel *model;
    GtkWidget *popover;
    GtkWidget *items_box;
    size_t index;
    size_t count;
    UmiUiWorkbenchState state;
    char active_label[UMI_UI_TEXT_CAPACITY + 8U] = "Layout";

    if (adapter == NULL || workbench == NULL ||
        adapter->workspace_profile_button == NULL ||
        adapter->workspace_profile_label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    model = umi_ui_workbench_workspace_profiles(workbench);
    count = umi_ui_workspace_profile_model_count(model);
    umi_ui_workbench_state_init(&state);
    (void)umi_ui_workbench_state_snapshot(workbench, &state);
    gtk_widget_set_visible(adapter->workspace_profile_button, count > 0U);
    if (count == 0U) return UMI_STATUS_OK;

    popover = gtk_popover_new();
    gtk_widget_add_css_class(popover,
                             "umicom-workspace-profile-popover");
    items_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

    for (index = 0U; index < count; ++index) {
        UmiUiWorkspaceProfileSnapshot profile;
        if (umi_ui_workspace_profile_model_at(model, index, &profile) !=
            UMI_STATUS_OK) {
            continue;
        }
        if (profile.active) {
            int modified = 0;
            (void)umi_ui_workbench_workspace_profile_modified(
                workbench, profile.profile_id, &modified);
            if (modified) {
                (void)snprintf(active_label, sizeof(active_label),
                               "%s •", profile.label);
            } else {
                (void)snprintf(active_label, sizeof(active_label),
                               "%s", profile.label);
            }
        }
        gtk_box_append(GTK_BOX(items_box),
                       create_profile_button(adapter, &profile));
    }

    /* The active profile supplies the editing policy and display name. */
    for (index = 0U; index < count; ++index) {
        UmiUiWorkspaceProfileSnapshot profile;
        if (umi_ui_workspace_profile_model_at(model, index, &profile) ==
                UMI_STATUS_OK &&
            profile.active) {
            gtk_box_append(GTK_BOX(items_box),
                           create_profile_designer(adapter,
                                                   &profile,
                                                   &state));
            break;
        }
    }

    gtk_label_set_text(GTK_LABEL(adapter->workspace_profile_label),
                       active_label);
    gtk_popover_set_child(GTK_POPOVER(popover), items_box);
    gtk_menu_button_set_popover(
        GTK_MENU_BUTTON(adapter->workspace_profile_button), popover);
    return UMI_STATUS_OK;
}
