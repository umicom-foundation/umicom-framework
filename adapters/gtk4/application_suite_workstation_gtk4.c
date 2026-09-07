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
#include "umicom/ui/gtk4/drop_down.h"
#include "umicom/ui/gtk4/automation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/suite_layout/geometry.h"
#include "umicom/application/suite_layout/render_plan.h"
#include "umicom/application/suite_layout/customisation.h"
#include "umicom/desktop/ui_bridge.h"
#include "umicom/ui/workbench_canvas.h"
#include "umicom/ui/workspace_customisation.h"
#include "umicom/ui/workspace_geometry.h"
#include "umicom/ui/gtk4/workstation/workspace_storage.h"
#include "umicom/ui/gtk4/workstation/layout_library.h"
#include "umicom/ui/workspace_library_checkpoint.h"

struct UmiApplicationSuiteGtk4Workstation {
    UmiApplicationSuiteLayoutRuntime runtime;
    UmiApplicationSuiteLayoutSelectorModel selector;
    UmiUiWorkspaceCustomisation customisation;
    UmiUiWorkbenchCanvas canvas;
    UmiGtk4WorkspaceLayoutHost *host;
    /* A copied-row view; customisation remains the sole layout authority. */
    UmiGtk4WorkspaceLayoutLibrary *layout_library;
    UmiGtk4AppearanceEditor *appearance;
    UmiGtk4WorkstationShellHeader *identity;
    UmiGtk4WorkstationWindowTitlebar *titlebar;
    GtkWindow *bound_window;
    UmiGtk4WorkstationCommandBar *command_bar;
    UmiGtk4AutomationDriver *automation;
    UmiWsCommandBarModel command_model;
    GtkWidget *root;
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
    GtkWidget *save_layout_button;
    GtkWidget *restore_layout_button;
    GtkWidget *layout_status;
    GtkWidget *panel_editor_revealer;
    GtkWidget *panel_editor_title;
    GtkWidget *panel_editor_region;
    GtkWidget *panel_editor_context;
    GtkWidget *panel_editor_auto_hide;
    GtkWidget *panel_editor_apply;
    GtkWidget *panel_editor_status;
    /* Percentage controls provide the same placement operation without a mouse. */
    GtkWidget *panel_editor_geometry;
    GtkWidget *panel_editor_bounds[4];
    GtkWidget *new_layout_button;
    GtkWidget *new_layout_name;
    GtkWidget *new_layout_popover;
    GtkWidget *clear_canvas_button;
    char panel_editor_window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    /* Keep the canvas host key so shutdown can unregister only this workstation. */
    char canvas_host_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char *saved_layout_text;
    uint64_t saved_layout_at_ns;
    UmiDataServer *checkpoint_server;
    UmiDataServer *owned_checkpoint_server;
    UmiUiWorkspaceCheckpointReport checkpoint_report;
    /* Library storage is separate from the active-layout checkpoint. A binding
     * generation invalidates queued operations when the borrowed server changes. */
    UmiUiWorkspaceLibraryCheckpointReport library_checkpoint_report;
    UmiStatus library_storage_status;
    uint64_t library_storage_generation;
    bool library_has_saved;
    bool library_save_conflict;
    UmiStatus checkpoint_storage_status;
    int checkpoint_storage_requested;
    int changing_selection;
    uint64_t revision;
};

/* A corrupt manifest must never be overwritten without a known compare revision. */
static int checkpoint_save_enabled(const UmiApplicationSuiteGtk4Workstation *workstation)
{
    return !workstation->checkpoint_storage_requested ||
        (workstation->checkpoint_server != NULL &&
         workstation->checkpoint_report.storage_revision_known);
}

static const char *WINDOW_REGIONS[] = {
    "centre", "left", "right", "bottom", "top", "canvas"
};

/* The panel editor also offers Floating as an explicit placement rather than
 * hiding detachment behind an icon whose result is difficult to predict. */
static const char *PANEL_EDITOR_REGIONS[] = {
    "centre", "left", "right", "bottom", "top", "floating", "canvas"
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

/* Navigation reads the existing workspace owner; it never creates a second
 * list of panel instances or an implicit layout-edit transaction. */
static const UmiUiWorkspaceLayout *active_layout(
    const UmiApplicationSuiteGtk4Workstation *workstation);
static void refresh_window_catalogue_controls(UmiApplicationSuiteGtk4Workstation *workstation);

/* Visible tools only need focus. Ordinary hidden dock tools may be reopened;
 * adding a missing instance or changing placement remains an explicit edit. */
static int can_show_window(const UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiWorkspaceWindow *window)
{
    const UmiExperiencePanelDefinition *panel;
    if (window == NULL) return 0;
    if (window->visible || umi_ui_workspace_customisation_window_is_auto_hidden(
            &workstation->customisation, window->window_id)) return 1;
    if (window->pinned || window->floating || strcmp(window->placement_id, "canvas") == 0)
        return 0;
    panel = workstation->runtime.experience != NULL
        ? umi_application_experience_panel_find(workstation->runtime.experience, window->tool_id)
        : NULL;
    return panel == NULL || (panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) != 0U;
}

/* Four workspace actions, every stored layout and every current instance fit
 * without expanding the established public command-model structure. */
_Static_assert(4U + UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS + UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS
    <= UMI_WS_MAX_PALETTE_ITEMS, "Workspace navigation must fit the command catalogue");

/* Build into a heap candidate: allocation/validation failure cannot publish a
 * partial list, and large public models do not consume the Windows C stack.
 * Item keys identify result rows; complete target IDs are carried separately
 * in command_id so a maximum-length saved ID never needs truncating a prefix. */
static UmiStatus build_command_model(UmiApplicationSuiteGtk4Workstation *workstation)
{
    static const struct { const char *id; const char *title; const char *description;
        const char *keywords; UmiWsCommandScope scope; uint32_t priority; } actions[] = {
        { "suite.layout.edit", "Edit or apply layout",
          "Unlock panel movement, or apply and lock the current arrangement.",
          "layout lock unlock customise arrange", UMI_WS_COMMAND_SCOPE_COMMAND, 100U },
        { "suite.window.open", "Windows and tools",
          "Focus or reopen an existing tool. Edit Layout enables adding or moving tools.",
          "panel tool dock float new window", UMI_WS_COMMAND_SCOPE_PANEL, 95U },
        { "suite.layout.save", "Save layout checkpoint",
          "Keep a recovery copy of the current locked arrangement.",
          "layout checkpoint recovery", UMI_WS_COMMAND_SCOPE_COMMAND, 90U },
        { "suite.layout.restore", "Restore layout checkpoint",
          "Return to the most recently saved arrangement.",
          "layout checkpoint recover reset", UMI_WS_COMMAND_SCOPE_COMMAND, 85U }
    };
    const UmiUiWorkspaceLayout *layout;
    UmiWsCommandBarModel *candidate;
    UmiStatus status = UMI_STATUS_OK;
    size_t index;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = active_layout(workstation);
    if (workstation->customisation.layout_count > UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS ||
        (layout != NULL && layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS))
        return UMI_STATUS_INVALID_STATE;
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ws_command_bar_model_init(candidate);
    for (index = 0U; status == UMI_STATUS_OK && index < G_N_ELEMENTS(actions); ++index)
        status = umi_ws_command_bar_model_add(candidate, actions[index].id,
            actions[index].title, actions[index].description, actions[index].id,
            actions[index].keywords, actions[index].scope, actions[index].priority);
    /* Named user layouts are first-class choices alongside product presets. */
    for (index = 0U; status == UMI_STATUS_OK &&
            index < workstation->customisation.layout_count; ++index) {
        const UmiUiWorkspaceLayout *choice = &workstation->customisation.layouts[index];
        char item_id[32];
        int written = snprintf(item_id, sizeof(item_id), "suite.choice.%zu", index);
        if (written < 0 || (size_t)written >= sizeof(item_id)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        status = umi_ws_command_bar_model_add(candidate, item_id, choice->name,
            workstation->customisation.edit_active
                ? "Apply or cancel the current edit before switching layouts."
                : "Switch to this product or user-created workspace.",
            choice->layout_id, "layout workspace", UMI_WS_COMMAND_SCOPE_COMMAND, 50U);
        if (status == UMI_STATUS_OK)
            status = umi_ws_command_bar_model_set_enabled(candidate, item_id,
                !workstation->customisation.edit_active);
    }
    /* Include hidden and auto-hide instances, not just rendered GTK children.
     * Focus/reopen resolves their copied ID against the live model on dispatch. */
    for (index = 0U; status == UMI_STATUS_OK && layout != NULL &&
            index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        char item_id[32];
        const int available = can_show_window(workstation, window);
        const int rail = umi_ui_workspace_customisation_window_is_auto_hidden(
            &workstation->customisation, window->window_id);
        const char *description = !available
            ? "This hidden panel cannot be reopened here. Use Edit Layout to review its permissions and placement."
            : rail ? "Reveal this edge tool without changing the saved arrangement."
            : window->visible ? "Focus this existing panel without moving or recreating it."
            : "Reopen this docked tool at its existing position.";
        int written = snprintf(item_id, sizeof(item_id), "suite.panel.%zu", index);
        if (written < 0 || (size_t)written >= sizeof(item_id)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        status = umi_ws_command_bar_model_add(candidate, item_id,
            window->title[0] != '\0' ? window->title : window->window_id,
            description, window->window_id, window->tool_id, UMI_WS_COMMAND_SCOPE_PANEL, 75U);
        if (status == UMI_STATUS_OK)
            status = umi_ws_command_bar_model_set_enabled(candidate, item_id, available != 0);
    }
    if (status == UMI_STATUS_OK) workstation->command_model = *candidate;
    free(candidate);
    return status;
}

/* Refresh from the live model after every layout transition. The managed
 * renderer keeps the current search text while replacing its result entries. */
static void refresh_command_model(UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiStatus status;
    int editing;
    if (workstation == NULL) return;
    status = build_command_model(workstation);
    if (status != UMI_STATUS_OK) {
        if (workstation->layout_status != NULL)
            gtk_label_set_text(GTK_LABEL(workstation->layout_status),
                "Workspace search could not refresh. Existing layout data is unchanged.");
        return;
    }
    editing = workstation->customisation.edit_active ? 1 : 0;
    (void)umi_ws_command_bar_model_set_enabled(&workstation->command_model,
        "suite.layout.save", editing == 0 && checkpoint_save_enabled(workstation));
    (void)umi_ws_command_bar_model_set_enabled(&workstation->command_model,
        "suite.layout.restore", editing == 0 &&
        (workstation->checkpoint_server != NULL || workstation->saved_layout_text != NULL));
    if (workstation->command_bar != NULL)
        (void)umi_gtk4_ws_command_bar_set_model(workstation->command_bar, &workstation->command_model);
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/* Join a product ID and preset ID only when the complete name fits. Checking
 * lengths before copying avoids truncated names that could select another
 * layout, and leaves the caller's buffer unchanged when space is insufficient. */
static UmiStatus qualify_layout_id(char *destination, size_t capacity,
                                  const char *application_id, const char *preset_id)
{
    size_t prefix_length;
    size_t suffix_length;
    if (destination == NULL || application_id == NULL || preset_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    prefix_length = strlen(application_id);
    suffix_length = strlen(preset_id);
    /* Subtract only after each bound is checked, so small buffers cannot make
     * unsigned lengths wrap around and appear to have more room. */
    if (prefix_length >= capacity || capacity - prefix_length <= 1U ||
        suffix_length >= capacity - prefix_length - 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, application_id, prefix_length);
    destination[prefix_length] = '.';
    memcpy(destination + prefix_length + 1U, preset_id, suffix_length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the refresh heading operation used by this module and its client applications. */
static void refresh_heading(UmiApplicationSuiteGtk4Workstation *workstation)
{
    const UmiUiWorkspaceLayout *layout;
    UmiGtk4WorkstationShellHeaderSnapshot identity;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->identity == NULL) return;
    layout = umi_ui_workspace_customisation_active_const(
        &workstation->customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return;

    /* The product name remains stable while the copied subtitle identifies
     * the active layout. Compact headers expose it through the tooltip and
     * accessible description; the layout dropdown also keeps it visible. */
    identity = umi_gtk4_ws_shell_header_snapshot(workstation->identity);
    (void)umi_gtk4_ws_shell_header_set_text(
        workstation->identity,
        identity.title,
        layout->name,
        identity.mode_badge);
    /* The native title owns its centred layout context; the product identity
     * and operational-mode badge remain unchanged in the shared header. */
    if (workstation->bound_window != NULL) {
        char *title = g_strdup_printf("%s — %s", identity.title, layout->name);
        gtk_window_set_title(workstation->bound_window, title);
        g_free(title);
    }
}

/* Explicit window binding leaves standalone/embedded workstation creation
 * unchanged. Successful transfer moves only existing identity widgets; panel
 * bodies, catalogue state and the appearance owner are not reconstructed. */
UmiStatus umi_application_suite_gtk4_workstation_bind_window(
    UmiApplicationSuiteGtk4Workstation *workstation, GtkWindow *window)
{
    UmiGtk4WorkstationShellHeaderSnapshot identity;
    UmiStatus status;
    if (workstation == NULL || window == NULL || !GTK_IS_WINDOW(window) ||
        workstation->identity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workstation->titlebar != NULL)
        return workstation->bound_window == window ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    identity = umi_gtk4_ws_shell_header_snapshot(workstation->identity);
    status = umi_gtk4_ws_window_titlebar_create_from_header(
        window, workstation->identity, identity.title, &workstation->titlebar);
    if (status != UMI_STATUS_OK) return status;
    /* A titlebar is a sibling of content, not an ancestor retaining the
     * GtkWindow. Keep the original UAT driver and extend its observed scope. */
    status = umi_gtk4_automation_driver_add_observed_scope(workstation->automation,
        umi_gtk4_ws_window_titlebar_widget(workstation->titlebar));
    if (status != UMI_STATUS_OK) return status;
    workstation->bound_window = window;
    g_object_add_weak_pointer(G_OBJECT(window), (gpointer *)&workstation->bound_window);
    /* Reuse the existing display provider and profile scope, including custom
     * user fonts/colours, after identity moves outside the content root. */
    gtk_widget_add_css_class(umi_gtk4_ws_window_titlebar_widget(workstation->titlebar),
        "umicom-appearance-scope");
    refresh_heading(workstation);
    workstation->revision += 1U;
    return UMI_STATUS_OK;
}

/* Keep the shared SVG mark in step with the appearance editor. The callback
 * receives a borrowed profile, and the header copies its resource path. */
static void on_appearance_changed(
    const UmiUiAppearanceProfile *profile,
    void *user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->identity == NULL ||
        profile == NULL) {
        return;
    }
    (void)umi_gtk4_ws_shell_header_apply_appearance(
        workstation->identity, profile);
}

/* Provide the active layout operation used by this module and its client applications. */
static const UmiUiWorkspaceLayout *active_layout(
    const UmiApplicationSuiteGtk4Workstation *workstation)
{
    return workstation != NULL
        ? umi_ui_workspace_customisation_active_const(
              &workstation->customisation)
        : NULL;
}

/*
 * Provide the rebuild active layout operation used by this module and its client
 * applications.
 */
static UmiStatus rebuild_active_layout(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    const UmiUiWorkspaceLayout *layout = active_layout(workstation);
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || layout == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_gtk4_workspace_layout_host_rebuild(workstation->host, layout);
    if (status == UMI_STATUS_OK) {
        refresh_window_catalogue_controls(workstation);
        refresh_command_model(workstation);
    }
    return status;
}

/* Rebuild the visible choices from the same model that owns named layouts.
 * Canonical presets and user-created canvases can then be selected together. */
static void refresh_layout_choices(UmiApplicationSuiteGtk4Workstation *workstation)
{
    GtkStringList *choices;
    size_t index;
    guint selected = GTK_INVALID_LIST_POSITION;

    if (workstation == NULL || workstation->layout_dropdown == NULL) return;
    choices = gtk_string_list_new(NULL);
    for (index = 0U; index < workstation->customisation.layout_count; ++index) {
        const UmiUiWorkspaceLayout *layout =
            &workstation->customisation.layouts[index];
        gtk_string_list_append(choices, layout->name);
        if (strcmp(layout->layout_id,
                   workstation->customisation.active_layout_id) == 0)
            selected = (guint)index;
    }
    /* Replacing the list emits selection notifications; they must not activate
     * another layout while the new list is still being installed. */
    workstation->changing_selection = 1;
    gtk_drop_down_set_model(GTK_DROP_DOWN(workstation->layout_dropdown),
                            G_LIST_MODEL(choices));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(workstation->layout_dropdown), selected);
    workstation->changing_selection = 0;
    g_object_unref(choices);
}

/* A completed gesture changes the current edit, not its rollback baseline.
 * The adapter defers this call until GTK has finished the pointer callback. */
static UmiStatus on_canvas_geometry(
    const char *window_id,
    const UmiApplicationSuiteLayoutRect *rect,
    uint64_t expected_layout_revision,
    void *user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation = user_data;
    const UmiUiWorkspaceLayout *layout = active_layout(workstation);
    UmiStatus status;
    const UmiUiWorkspaceWindow *window;
    const UmiExperiencePanelDefinition *panel;

    if (workstation == NULL || rect == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (layout == NULL || layout->revision != expected_layout_revision)
        return UMI_STATUS_INVALID_STATE;
    window = umi_ui_workspace_layout_find_window(layout, window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    panel = workstation->runtime.experience != NULL
        ? umi_application_experience_panel_find(workstation->runtime.experience, window->tool_id)
        : NULL;
    /* Imported layouts cannot grant a gesture capability that the product
     * denied in its canonical descriptor. Match the panel-settings boundary. */
    if (panel != NULL && (panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) == 0U)
        return UMI_STATUS_PERMISSION_DENIED;
    status = umi_ui_workspace_customisation_place_canvas_window(
        &workstation->customisation, window_id,
        rect->x, rect->y, rect->width, rect->height);
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        workstation->revision += 1U;
    }
    if (workstation->layout_status != NULL) {
        gtk_label_set_text(GTK_LABEL(workstation->layout_status),
            status == UMI_STATUS_OK
                ? "Panel arranged. Apply and Lock to keep it, or Cancel."
                : "Panel could not be arranged. Check the layout and panel locks.");
    }
    return status;
}

/*
 * Provide the refresh edit controls operation used by this module and its client
 * applications.
 */
static void refresh_edit_controls(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    int editing;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    editing = workstation->customisation.edit_active ? 1 : 0;
    if (workstation->new_layout_button != NULL)
        gtk_widget_set_sensitive(workstation->new_layout_button, !editing);
    if (workstation->clear_canvas_button != NULL)
        gtk_widget_set_sensitive(workstation->clear_canvas_button, editing);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->edit_layout_button != NULL) {
        gtk_button_set_label(
            GTK_BUTTON(workstation->edit_layout_button),
            editing ? "Apply and Lock" : "Edit Layout");
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->cancel_edit_button != NULL) {
        gtk_widget_set_visible(workstation->cancel_edit_button, editing);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->new_window_button != NULL) {
        gtk_widget_set_sensitive(workstation->new_window_button, TRUE);
    }
    refresh_window_catalogue_controls(workstation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->layout_dropdown != NULL) {
        gtk_widget_set_sensitive(workstation->layout_dropdown, !editing);
    }
    /* Checkpoints represent committed layouts, so saving and restoring pause
     * while the user owns an editable rollback session. */
    if (workstation->save_layout_button != NULL) {
        gtk_widget_set_sensitive(workstation->save_layout_button,
            !editing && checkpoint_save_enabled(workstation));
        gtk_widget_set_tooltip_text(workstation->save_layout_button,
            workstation->checkpoint_server != NULL && !workstation->checkpoint_report.storage_revision_known
                ? "Save is disabled until Restore establishes a trustworthy storage revision; damaged manifests need repair"
                : workstation->checkpoint_server != NULL && workstation->checkpoint_report.durable
                ? "Save this committed active layout to disk for the next launch"
                : workstation->checkpoint_storage_requested && workstation->checkpoint_server == NULL
                    ? "Persistent storage is unavailable; this request will not silently save to memory"
                    : "Save this committed active layout for this session only");
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->restore_layout_button != NULL) {
        gtk_widget_set_sensitive(
            workstation->restore_layout_button,
            !editing && (workstation->checkpoint_server != NULL || workstation->saved_layout_text != NULL));
        gtk_widget_set_tooltip_text(workstation->restore_layout_button,
            workstation->checkpoint_server != NULL && workstation->checkpoint_report.durable
                ? "Validate and restore the last explicitly saved active layout from disk"
                : "Restore the last checkpoint saved in this session");
    }
    refresh_command_model(workstation);
    if (workstation->layout_library != NULL)
        (void)umi_gtk4_ws_layout_library_refresh(workstation->layout_library);
}

/*
 * Provide the application suite gtk4 workstation select layout operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_select_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *layout_id)
{
    UmiStatus status;
    size_t index;
    size_t canonical_index = UMI_APPLICATION_SUITE_LAYOUT_SELECTOR_MAX;
    char qualified_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    const char *selected_id = layout_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    /* Existing callers use short preset IDs. Accept those and their saved
     * qualified forms while keeping user layout IDs unchanged. */
    for (index = 0U; index < workstation->selector.count; ++index) {
        const char *short_id = workstation->selector.choices[index].layout_id;
        status = qualify_layout_id(qualified_id, sizeof(qualified_id),
            workstation->runtime.experience->application_id, short_id);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(layout_id, short_id) == 0 || strcmp(layout_id, qualified_id) == 0) {
            selected_id = qualified_id;
            canonical_index = index;
            break;
        }
    }
    status = umi_ui_workspace_customisation_activate(
        &workstation->customisation, selected_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = rebuild_active_layout(workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep canonical catalogue metadata in step when this is a preset. A user
     * canvas has no canonical definition and remains owned by customisation. */
    if (canonical_index < workstation->selector.count) {
        const char *short_id = workstation->selector.choices[canonical_index].layout_id;
        (void)umi_application_suite_layout_runtime_select(&workstation->runtime, short_id);
        (void)umi_application_suite_layout_selector_select(&workstation->selector, short_id);
    }
    refresh_layout_choices(workstation);
    refresh_heading(workstation);
    refresh_edit_controls(workstation);
    workstation->revision += 1U;
    return UMI_STATUS_OK;
}

/* Create a named empty layout without clearing or replacing an existing one.
 * It starts locked; Edit Layout opens the normal reversible transaction. */
UmiStatus umi_application_suite_gtk4_workstation_create_blank_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *layout_id, const char *name)
{
    UmiStatus status;
    size_t prefix_length;
    if (workstation == NULL || layout_id == NULL ||
        workstation->runtime.experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* The qualified application prefix also selects the canonical panel
     * permissions. A custom name must not accidentally bypass those rules. */
    prefix_length = strlen(workstation->runtime.experience->application_id);
    if (strncmp(layout_id, workstation->runtime.experience->application_id,
                prefix_length) != 0 || layout_id[prefix_length] != '.' ||
        layout_id[prefix_length + 1U] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_create_blank_layout(
        &workstation->customisation, layout_id, name);
    if (status != UMI_STATUS_OK) return status;
    status = rebuild_active_layout(workstation);
    refresh_layout_choices(workstation);
    refresh_heading(workstation);
    refresh_edit_controls(workstation);
    workstation->revision += 1U;
    return status;
}

/* Clear only removable view instances inside the current edit. Product data,
 * pinned panels and essential non-closable panels are not deleted. */
UmiStatus umi_application_suite_gtk4_workstation_clear_canvas(
    UmiApplicationSuiteGtk4Workstation *workstation,
    UmiUiWorkspaceCanvasClearResult *out_result)
{
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_clear_canvas(
        &workstation->customisation, out_result);
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        workstation->revision += 1U;
    }
    return status;
}

/* Forward appearance selection to the Framework-owned editor so applications
 * do not maintain separate theme state or write their own preference files. */
UmiStatus umi_application_suite_gtk4_workstation_select_appearance(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *profile_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || profile_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_gtk4_appearance_editor_select(
        workstation->appearance, profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/* Apply one complete custom profile through the same validation and storage
 * path used by the visible Appearance editor. */
UmiStatus umi_application_suite_gtk4_workstation_apply_custom_appearance(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiAppearanceProfile *profile)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_gtk4_appearance_editor_apply_custom(
        workstation->appearance, profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/* Return a value copy so product code can display the current choice without
 * reaching into Framework widgets or owning appearance memory. */
UmiStatus umi_application_suite_gtk4_workstation_active_appearance(
    const UmiApplicationSuiteGtk4Workstation *workstation,
    UmiUiAppearanceProfile *out_profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_gtk4_appearance_editor_active(
        workstation->appearance, out_profile);
}

/* Update operational mode without rebuilding layouts or replacing the
 * application name. Trading, editing and system shells can reuse this path. */
UmiStatus umi_application_suite_gtk4_workstation_set_mode_badge(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *mode_badge)
{
    UmiGtk4WorkstationShellHeaderSnapshot identity;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->identity == NULL ||
        mode_badge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    identity = umi_gtk4_ws_shell_header_snapshot(workstation->identity);
    status = umi_gtk4_ws_shell_header_set_text(
        workstation->identity,
        identity.title,
        identity.subtitle,
        mode_badge);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/*
 * Provide the application suite gtk4 workstation begin layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_begin_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_begin_edit(
        &workstation->customisation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        refresh_edit_controls(workstation);
        workstation->revision += 1U;
    }
    return status;
}

/*
 * Provide the application suite gtk4 workstation commit layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_commit_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_commit_edit(
        &workstation->customisation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        refresh_edit_controls(workstation);
        workstation->revision += 1U;
    }
    return status;
}

/*
 * Provide the application suite gtk4 workstation cancel layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_cancel_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_cancel_edit(
        &workstation->customisation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        refresh_edit_controls(workstation);
        workstation->revision += 1U;
    }
    return status;
}

/* Export the active layout without exposing the workstation's mutable model. */
UmiStatus umi_application_suite_gtk4_workstation_export_layout(
    const UmiApplicationSuiteGtk4Workstation *workstation,
    uint64_t saved_at_ns,
    char *out_text,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* An edit in progress has a rollback baseline and is not yet a committed
     * workspace, so callers must apply or cancel it before persistence. */
    if (workstation->customisation.edit_active) {
        return UMI_STATUS_BUSY;
    }
    return umi_ui_workspace_customisation_export_active(
        &workstation->customisation, saved_at_ns, out_text, capacity);
}

/* Import a canonical or user-created layout belonging to this application.
 * Stage the complete model first so rejected files cannot change live state. */
UmiStatus umi_application_suite_gtk4_workstation_import_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *text,
    int activate,
    UmiUiWorkspaceImportReport *out_report)
{
    UmiUiWorkspaceImportOptions options =
        umi_ui_workspace_import_options_default();
    UmiUiWorkspaceImportReport local_report;
    UmiUiWorkspaceCustomisation *candidate;
    UmiApplicationSuiteLayoutRenderPlan *render_plan;
    const UmiUiWorkspaceLayout *imported_layout = NULL;
    const UmiUiWorkspaceLayout *candidate_active;
    const UmiUiWorkspaceLayout *previous_active;
    size_t prefix_length;
    size_t index;
    int redraw;
    UmiStatus status;

    /* Clear stale success evidence even if decoding or product checks fail. */
    if (out_report != NULL) (void)memset(out_report, 0, sizeof(*out_report));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || text == NULL ||
        workstation->runtime.experience == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* An import must not replace the rollback baseline of an active edit. */
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    candidate = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = workstation->customisation;
    /* New named canvases must survive a fresh application instance. Permit
     * additions on the private candidate, then enforce the product prefix
     * before either the live model or its native presentation can change. */
    options.conflict_policy = UMI_UI_WORKSPACE_IMPORT_REPLACE_CONFLICT;
    options.activate_imported_layout = activate != 0;
    options.allow_new_layout = true;
    status = umi_ui_workspace_customisation_import(
        candidate, text, &options, &local_report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(candidate);
        return status;
    }
    prefix_length = strlen(workstation->runtime.experience->application_id);
    /* A similarly named product is not the same owner. Require the complete
     * ID, a separator and a non-empty layout suffix within the decoded field. */
    if (prefix_length + 1U >= sizeof(local_report.layout_id) ||
        strncmp(local_report.layout_id,
            workstation->runtime.experience->application_id, prefix_length) != 0 ||
        local_report.layout_id[prefix_length] != '.' ||
        local_report.layout_id[prefix_length + 1U] == '\0') {
        free(candidate);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Validate inactive imports too, so selecting a saved layout later cannot
     * reveal a rendering error that import silently accepted. */
    for (index = 0U; index < candidate->layout_count; ++index) {
        if (strcmp(candidate->layouts[index].layout_id, local_report.layout_id) == 0) {
            imported_layout = &candidate->layouts[index];
            break;
        }
    }
    if (imported_layout == NULL) {
        free(candidate);
        return UMI_STATUS_INVALID_STATE;
    }
    render_plan = (UmiApplicationSuiteLayoutRenderPlan *)malloc(sizeof(*render_plan));
    if (render_plan == NULL) {
        free(candidate);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = umi_application_suite_layout_render_plan_build(imported_layout, render_plan);
    free(render_plan);
    if (status != UMI_STATUS_OK) {
        free(candidate);
        return status;
    }
    previous_active = active_layout(workstation);
    candidate_active = umi_ui_workspace_customisation_active_const(candidate);
    redraw = activate != 0 || (previous_active != NULL &&
        strcmp(previous_active->layout_id, local_report.layout_id) == 0);
    /* An inactive addition leaves the visible widgets untouched. A visible
     * import is rendered before publishing its model; an allocation failure
     * keeps live state and attempts to restore its previous presentation. */
    if (redraw) {
        status = candidate_active != NULL
            ? umi_gtk4_workspace_layout_host_rebuild(workstation->host, candidate_active)
            : UMI_STATUS_INVALID_STATE;
        if (status != UMI_STATUS_OK) {
            if (previous_active != NULL)
                (void)umi_gtk4_workspace_layout_host_rebuild(workstation->host, previous_active);
            free(candidate);
            return status;
        }
    }
    /* This is the only publication point for decoded layouts and contexts. */
    workstation->customisation = *candidate;
    free(candidate);
    /* The staged renderer used the old live group store. Resolve any newly
     * imported colours only after the complete routing model is published. */
    (void)umi_gtk4_workspace_layout_host_set_context_groups(
        workstation->host, &workstation->customisation.groups);
    /* Canonical metadata keeps its established short IDs. A named canvas has
     * no canonical selector entry; its qualified ID stays in customisation. */
    if (activate != 0) {
        for (index = 0U; index < workstation->selector.count; ++index) {
            char qualified[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
            const char *short_id = workstation->selector.choices[index].layout_id;
            if (qualify_layout_id(qualified, sizeof(qualified),
                    workstation->runtime.experience->application_id, short_id) == UMI_STATUS_OK &&
                strcmp(local_report.layout_id, qualified) == 0) {
                (void)umi_application_suite_layout_runtime_select(&workstation->runtime, short_id);
                (void)umi_application_suite_layout_selector_select(&workstation->selector, short_id);
                break;
            }
        }
    }
    refresh_layout_choices(workstation);
    refresh_heading(workstation);
    refresh_edit_controls(workstation);
    workstation->revision += 1U;
    if (out_report != NULL) *out_report = local_report;
    return UMI_STATUS_OK;
}

/* Build the shared product namespace without inventing an independent store. */
static UmiStatus checkpoint_scope(
    const UmiApplicationSuiteGtk4Workstation *workstation,
    UmiUiWorkspaceCheckpointScope *scope, char *prefix, size_t capacity)
{
    int written;
    if (workstation == NULL || workstation->runtime.experience == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(prefix, capacity, "%s.",
        workstation->runtime.experience->application_id);
    if (written < 0 || (size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    scope->application_id = workstation->runtime.experience->application_id;
    scope->workspace_id = "desktop";
    scope->layout_prefix = prefix;
    return UMI_STATUS_OK;
}

/* The same product prefix protects both checkpoint imports and library actions. */
static UmiStatus suite_layout_library_read(
    UmiUiWorkspaceLibrarySnapshot *out_snapshot, void *context)
{
    UmiApplicationSuiteGtk4Workstation *workstation = context;
    UmiUiWorkspaceCheckpointScope scope;
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiStatus status = checkpoint_scope(workstation, &scope, prefix, sizeof(prefix));
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_workspace_library_snapshot(&workstation->customisation,
        &(const UmiUiWorkspaceLibraryPolicy){ prefix }, out_snapshot);
}

/* Render a validated private candidate before committing it. Metadata-only
 * changes keep panel bodies intact; a failed layout switch retains the owner
 * and attempts to restore its previous native presentation. */
static UmiStatus suite_publish_library_candidate(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiWorkspaceCustomisation *candidate, bool force_rebuild)
{
    const UmiUiWorkspaceLayout *previous;
    const UmiUiWorkspaceLayout *next;
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    if (workstation->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (candidate->revision == workstation->customisation.revision) return UMI_STATUS_OK;
    previous = active_layout(workstation);
    next = umi_ui_workspace_customisation_active_const(candidate);
    if (previous == NULL || next == NULL) status = UMI_STATUS_INVALID_STATE;
    else if (force_rebuild || strcmp(previous->layout_id, next->layout_id) != 0) {
        status = umi_gtk4_workspace_layout_host_rebuild(workstation->host, next);
        if (status != UMI_STATUS_OK)
            (void)umi_gtk4_workspace_layout_host_rebuild(workstation->host, previous);
    } else if (previous->revision != next->revision) {
        status = umi_gtk4_workspace_layout_host_update_metadata(workstation->host, next);
    }
    if (status != UMI_STATUS_OK) return status;
    workstation->customisation = *candidate;
    (void)umi_gtk4_workspace_layout_host_set_context_groups(
        workstation->host, &workstation->customisation.groups);
    /* Canonical short IDs remain compatibility metadata, not an authority
     * that can recreate a removed or renamed user-owned layout implicitly. */
    for (index = 0U; index < workstation->selector.count; ++index) {
        char qualified[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
        const char *short_id = workstation->selector.choices[index].layout_id;
        if (qualify_layout_id(qualified, sizeof(qualified),
                workstation->runtime.experience->application_id, short_id) == UMI_STATUS_OK &&
            strcmp(workstation->customisation.active_layout_id, qualified) == 0) {
            (void)umi_application_suite_layout_runtime_select(&workstation->runtime, short_id);
            (void)umi_application_suite_layout_selector_select(&workstation->selector, short_id);
            break;
        }
    }
    refresh_layout_choices(workstation);
    refresh_heading(workstation);
    refresh_edit_controls(workstation);
    ++workstation->revision;
    return UMI_STATUS_OK;
}

/* Every library command validates on private storage before native publication. */
static UmiStatus suite_layout_library_apply(
    const UmiUiWorkspaceLibraryRequest *request, void *context)
{
    UmiApplicationSuiteGtk4Workstation *workstation = context;
    UmiUiWorkspaceCheckpointScope scope;
    UmiUiWorkspaceCustomisation *candidate;
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiStatus status = checkpoint_scope(workstation, &scope, prefix, sizeof(prefix));
    if (status != UMI_STATUS_OK) return status;
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = workstation->customisation;
    status = umi_ui_workspace_library_apply(candidate,
        &(const UmiUiWorkspaceLibraryPolicy){ prefix }, request, NULL);
    if (status == UMI_STATUS_OK)
        status = suite_publish_library_candidate(workstation, candidate, false);
    free(candidate);
    return status;
}

/* Inspect once when storage is explicitly bound. Routine GUI refreshes read
 * this copied evidence and never poll SQLite or publish a saved arrangement. */
static void suite_probe_library_storage(UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiUiWorkspaceCheckpointScope scope;
    UmiUiWorkspaceCustomisation *candidate;
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    memset(&workstation->library_checkpoint_report, 0, sizeof(workstation->library_checkpoint_report));
    /* Backend capability is already known from binding, even if allocating a
     * library probe fails before the checkpoint service can return a report. */
    workstation->library_checkpoint_report.checkpoint.durable = workstation->checkpoint_report.durable;
    workstation->library_has_saved = false;
    workstation->library_save_conflict = false;
    workstation->library_storage_status = UMI_STATUS_UNAVAILABLE;
    if (workstation->checkpoint_server == NULL) return;
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) { workstation->library_storage_status = UMI_STATUS_OUT_OF_MEMORY; return; }
    workstation->library_storage_status = checkpoint_scope(workstation, &scope, prefix, sizeof(prefix));
    if (workstation->library_storage_status == UMI_STATUS_OK)
        workstation->library_storage_status = umi_ui_workspace_library_checkpoint_load_candidate(
            workstation->checkpoint_server, &scope, &workstation->customisation,
            candidate, &workstation->library_checkpoint_report);
    workstation->library_has_saved = workstation->library_storage_status != UMI_STATUS_NOT_FOUND;
    free(candidate);
}

/* Cached capability is presentation, not permission to replace another
 * writer's saved revision or a developer's current edit session. */
static UmiStatus suite_library_storage_read(
    UmiGtk4WorkspaceLayoutLibraryStorageState *out_state, void *context)
{
    UmiApplicationSuiteGtk4Workstation *workstation = context;
    const UmiUiWorkspaceCheckpointReport *report = &workstation->library_checkpoint_report.checkpoint;
    const char *message;
    bool all_locked = workstation->customisation.layout_count != 0U &&
        workstation->customisation.layout_count <= UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS;
    for (size_t index = 0U; all_locked && index < workstation->customisation.layout_count; ++index)
        all_locked = workstation->customisation.layouts[index].locked;
    memset(out_state, 0, sizeof(*out_state));
    out_state->supported = workstation->checkpoint_server != NULL;
    out_state->durable = report->durable;
    out_state->revision_known = report->storage_revision_known;
    out_state->has_saved = workstation->library_has_saved;
    out_state->save_enabled = report->storage_revision_known && all_locked && !workstation->library_save_conflict;
    out_state->restore_enabled = workstation->checkpoint_server != NULL;
    out_state->storage_revision = report->storage_revision;
    out_state->storage_generation = workstation->library_storage_generation;
    message = !out_state->supported ? "No Data Server is connected for library storage."
        : !all_locked ? "Apply and lock every layout before saving the library."
        : workstation->library_save_conflict ? "Another writer changed the saved library. Restore and review it before saving again."
        : workstation->library_storage_status == UMI_STATUS_NOT_FOUND ? "No saved library. Save library stores all committed layouts."
        : workstation->library_storage_status != UMI_STATUS_OK ? "Library storage needs attention. Restore re-reads saved data; current layouts are kept on failure."
        : report->recovered_last_good ? "Previous valid library recovered. Review it before saving."
        : report->durable ? "Library checkpoint on disk. Unsaved library changes still need Save library."
        : "Memory-only library checkpoint; it will not survive process exit.";
    (void)g_strlcpy(out_state->message, message, sizeof(out_state->message));
    return UMI_STATUS_OK;
}

/* Save does not modify the visible owner. Restore validates the entire archive
 * and renders its active layout before replacing the current named list. */
static UmiStatus suite_library_storage_operation(
    const UmiGtk4WorkspaceLayoutLibraryStorageRequest *request, void *context)
{
    UmiApplicationSuiteGtk4Workstation *workstation = context;
    UmiUiWorkspaceCheckpointScope scope;
    UmiUiWorkspaceLibraryCheckpointReport report = {0};
    UmiUiWorkspaceCustomisation *candidate = NULL;
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiStatus status;
    if (workstation->checkpoint_server == NULL) return UMI_STATUS_UNAVAILABLE;
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    if (request->expected_customisation_revision != workstation->customisation.revision ||
        request->expected_storage_generation != workstation->library_storage_generation ||
        request->expected_storage_revision != workstation->library_checkpoint_report.checkpoint.storage_revision)
        return UMI_STATUS_INVALID_STATE;
    status = checkpoint_scope(workstation, &scope, prefix, sizeof(prefix));
    if (status != UMI_STATUS_OK) return status;
    if (request->action == UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_SAVE) {
        const gint64 now = g_get_real_time();
        const uint64_t saved_at_ns = now > 0 && (uint64_t)now <= UINT64_MAX / UINT64_C(1000)
            ? (uint64_t)now * UINT64_C(1000) : 0U;
        if (!workstation->library_checkpoint_report.checkpoint.storage_revision_known)
            return UMI_STATUS_INVALID_STATE;
        status = umi_ui_workspace_library_checkpoint_save(workstation->checkpoint_server,
            &scope, &workstation->customisation, saved_at_ns,
            request->expected_storage_revision, &report);
    } else if (request->action == UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_RESTORE) {
        if (!request->restore_confirmed) return UMI_STATUS_PERMISSION_DENIED;
        candidate = malloc(sizeof(*candidate));
        if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        status = umi_ui_workspace_library_checkpoint_load_candidate(workstation->checkpoint_server,
            &scope, &workstation->customisation, candidate, &report);
        if (status == UMI_STATUS_OK) status = suite_publish_library_candidate(workstation, candidate, true);
    } else return UMI_STATUS_INVALID_ARGUMENT;
    free(candidate);
    workstation->library_storage_status = status;
    if (status == UMI_STATUS_OK) {
        workstation->library_checkpoint_report = report;
        workstation->library_has_saved = true;
        workstation->library_save_conflict = false;
    } else if (request->action == UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_RESTORE &&
               status == UMI_STATUS_NOT_FOUND && report.checkpoint.storage_revision_known &&
               report.checkpoint.storage_revision == 0U) {
        /* An explicit read confirmed that both saved copies are absent. Keep
         * session layouts, but allow a new first save against known revision 0. */
        workstation->library_checkpoint_report = report;
        workstation->library_has_saved = false;
        workstation->library_save_conflict = false;
    } else {
        /* A failed Save never adopts the competing writer's CAS revision. */
        if (report.checkpoint.storage_revision_known &&
            report.checkpoint.storage_revision != request->expected_storage_revision)
            workstation->library_save_conflict = true;
        if (report.checkpoint.primary_status != UMI_STATUS_NOT_FOUND)
            workstation->library_has_saved = true;
        if (!report.checkpoint.storage_revision_known && report.checkpoint.primary_status != UMI_STATUS_NOT_FOUND)
            workstation->library_checkpoint_report.checkpoint.storage_revision_known = false;
    }
    return status;
}

/* Explain persistence outcomes beside the actual Save/Restore controls. */
static void checkpoint_feedback(UmiApplicationSuiteGtk4Workstation *workstation,
    UmiStatus status, const char *success)
{
    char message[256];
    if (workstation == NULL || workstation->layout_status == NULL) return;
    if (status == UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(workstation->layout_status), success);
        gtk_widget_set_tooltip_text(workstation->layout_status, success);
    } else {
        (void)snprintf(message, sizeof(message),
            "Layout storage: %s. Current layout is unchanged; no checkpoint was overwritten.",
            umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(workstation->layout_status), message);
        /* Compact headers may ellipsize this label; keep the complete storage
         * result available without widening the application window. */
        gtk_widget_set_tooltip_text(workstation->layout_status, message);
    }
}

/* Attach only after existing bytes pass codec, dependency and ownership checks.
 * Binding never restores a layout or changes an active edit behind the caller. */
UmiStatus umi_application_suite_gtk4_workstation_bind_checkpoint_storage(
    UmiApplicationSuiteGtk4Workstation *workstation, UmiDataServer *server)
{
    UmiUiWorkspaceCheckpointScope scope;
    UmiUiWorkspaceCheckpointReport report = {0};
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char *candidate = NULL;
    UmiStatus status = UMI_STATUS_OK;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    if (workstation->library_storage_generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (server != NULL) {
        candidate = (char *)calloc(UMI_UI_LAYOUT_ENCODED_CAPACITY, 1U);
        if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        status = checkpoint_scope(workstation, &scope, prefix, sizeof(prefix));
        if (status == UMI_STATUS_OK)
            status = umi_ui_workspace_checkpoint_load_validated(server, &scope,
                &workstation->customisation, candidate, UMI_UI_LAYOUT_ENCODED_CAPACITY, &report);
        if (status == UMI_STATUS_NOT_FOUND) {
            free(candidate);
            candidate = NULL;
            status = UMI_STATUS_OK;
        }
        if (status != UMI_STATUS_OK) {
            free(candidate);
            return status;
        }
    }
    if (workstation->owned_checkpoint_server != server) {
        umi_data_server_destroy(workstation->owned_checkpoint_server);
        workstation->owned_checkpoint_server = NULL;
    }
    workstation->checkpoint_server = server;
    workstation->checkpoint_storage_requested = server != NULL;
    workstation->checkpoint_storage_status = UMI_STATUS_OK;
    workstation->checkpoint_report = report;
    ++workstation->library_storage_generation;
    suite_probe_library_storage(workstation);
    free(workstation->saved_layout_text);
    workstation->saved_layout_text = candidate;
    workstation->saved_layout_at_ns = candidate != NULL ? report.saved_at_ns : 0U;
    refresh_edit_controls(workstation);
    workstation->revision += 1U;
    return UMI_STATUS_OK;
}

/* Native launchers opt into durable storage; constructors remain I/O-free. */
UmiStatus umi_application_suite_gtk4_workstation_enable_checkpoint_storage(
    UmiApplicationSuiteGtk4Workstation *workstation, int restore_saved)
{
    UmiDataServer *server = NULL;
    UmiStatus status;
    if (workstation == NULL || workstation->runtime.experience == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    status = umi_gtk4_workspace_storage_open(
        workstation->runtime.experience->application_id, &server);
    if (status == UMI_STATUS_OK)
        status = umi_application_suite_gtk4_workstation_bind_checkpoint_storage(workstation, server);
    if (status == UMI_STATUS_OK) {
        workstation->owned_checkpoint_server = server;
        if (restore_saved && workstation->saved_layout_text != NULL)
            status = umi_application_suite_gtk4_workstation_restore_checkpoint(workstation);
    } else {
        umi_data_server_destroy(server);
        /* Do not pretend the legacy memory checkpoint fulfilled this explicit
         * durable request. Preserve any previously bound valid backend. */
        if (workstation->checkpoint_server == NULL)
            workstation->checkpoint_storage_requested = 1;
    }
    workstation->checkpoint_storage_status = status;
    refresh_edit_controls(workstation);
    checkpoint_feedback(workstation, status,
        workstation->checkpoint_report.recovered_last_good
            ? restore_saved
                ? "Recovered last-known-good saved layout; review before saving."
                : "Last-known-good checkpoint available; choose Restore to review it."
            : restore_saved && workstation->saved_layout_text != NULL
                ? "Saved layout restored from disk."
                : "Disk layout storage ready. Save keeps the active layout for next launch.");
    return status;
}

/* Save only committed layouts and retain the previous checkpoint on failure. */
UmiStatus umi_application_suite_gtk4_workstation_save_checkpoint(
    UmiApplicationSuiteGtk4Workstation *workstation, uint64_t saved_at_ns)
{
    UmiUiWorkspaceCheckpointScope scope;
    UmiUiWorkspaceCheckpointReport report = {0};
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char *candidate;
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    if (!checkpoint_save_enabled(workstation))
        return workstation->checkpoint_storage_status != UMI_STATUS_OK
            ? workstation->checkpoint_storage_status : UMI_STATUS_INVALID_STATE;
    candidate = (char *)calloc(UMI_UI_LAYOUT_ENCODED_CAPACITY, 1U);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_application_suite_gtk4_workstation_export_layout(
        workstation, saved_at_ns, candidate, UMI_UI_LAYOUT_ENCODED_CAPACITY);
    if (status == UMI_STATUS_OK && workstation->checkpoint_server != NULL) {
        status = checkpoint_scope(workstation, &scope, prefix, sizeof(prefix));
        if (status == UMI_STATUS_OK)
            status = umi_ui_workspace_checkpoint_save(workstation->checkpoint_server,
                &scope, &workstation->customisation, saved_at_ns,
                workstation->checkpoint_report.storage_revision, &report);
        workstation->checkpoint_storage_status = status;
        /* A damaged manifest is not permission to overwrite storage. Keep the
         * old CAS revision on conflicts; never adopt another writer's revision.
         * Early BUSY/OOM keeps default NOT_FOUND evidence and permits retry. */
        if (status != UMI_STATUS_OK && !report.storage_revision_known &&
            report.primary_status != UMI_STATUS_NOT_FOUND)
            workstation->checkpoint_report.storage_revision_known = false;
    }
    if (status != UMI_STATUS_OK) {
        free(candidate);
        refresh_edit_controls(workstation);
        return status;
    }
    /* Only successful persistent acceptance advances the cache and CAS evidence. */
    free(workstation->saved_layout_text);
    workstation->saved_layout_text = candidate;
    workstation->saved_layout_at_ns = saved_at_ns;
    if (workstation->checkpoint_server != NULL) workstation->checkpoint_report = report;
    refresh_edit_controls(workstation);
    workstation->revision += 1U;
    return UMI_STATUS_OK;
}

/* Reload storage for every explicit Restore, then use the existing staged
 * native importer so corrupt or incompatible data cannot replace live panels. */
UmiStatus umi_application_suite_gtk4_workstation_restore_checkpoint(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiUiWorkspaceCheckpointScope scope;
    UmiUiWorkspaceCheckpointReport report = {0};
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char *candidate;
    UmiStatus status;
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    if (workstation->checkpoint_server == NULL) {
        if (workstation->checkpoint_storage_requested)
            return workstation->checkpoint_storage_status;
        return workstation->saved_layout_text != NULL
            ? umi_application_suite_gtk4_workstation_import_layout(
                workstation, workstation->saved_layout_text, 1, NULL)
            : UMI_STATUS_NOT_FOUND;
    }
    candidate = (char *)calloc(UMI_UI_LAYOUT_ENCODED_CAPACITY, 1U);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = checkpoint_scope(workstation, &scope, prefix, sizeof(prefix));
    if (status == UMI_STATUS_OK)
        status = umi_ui_workspace_checkpoint_load_validated(workstation->checkpoint_server,
            &scope, &workstation->customisation, candidate, UMI_UI_LAYOUT_ENCODED_CAPACITY, &report);
    if (status == UMI_STATUS_OK)
        status = umi_application_suite_gtk4_workstation_import_layout(workstation, candidate, 1, NULL);
    workstation->checkpoint_storage_status = status;
    if (status == UMI_STATUS_OK) {
        free(workstation->saved_layout_text);
        workstation->saved_layout_text = candidate;
        workstation->saved_layout_at_ns = report.saved_at_ns;
        workstation->checkpoint_report = report;
        candidate = NULL;
    } else if (status == UMI_STATUS_NOT_FOUND && report.storage_revision_known) {
        /* An explicitly observed absent primary permits a first Save again;
         * the in-memory cache and visible layout are still left unchanged. */
        workstation->checkpoint_report = report;
    } else if (!report.storage_revision_known &&
               report.primary_status != UMI_STATUS_NOT_FOUND) {
        /* Early BUSY/OOM reports have not inspected the manifest and cannot
         * revoke previously trusted CAS evidence. Explicit damage can. */
        workstation->checkpoint_report.storage_revision_known = false;
    }
    free(candidate);
    refresh_edit_controls(workstation);
    return status;
}

/*
 * Provide the application suite gtk4 workstation open window operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_open_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *tool_id,
    const char *group_id,
    int floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity)
{
    const UmiExperiencePanelDefinition *panel;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || tool_id == NULL || group_id == NULL ||
        out_window_id == NULL || out_window_id_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = workstation->runtime.experience != NULL
        ? umi_application_experience_panel_find(
              workstation->runtime.experience, tool_id)
        : NULL;
    /* Experience panels must advertise the requested starting behavior.
     * Shared catalogue panels without an experience entry use base defaults. */
    if (panel != NULL && floating &&
        (panel->flags & UMI_EXPERIENCE_PANEL_FLOATABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (panel != NULL && !floating &&
        (panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    status = umi_ui_workspace_customisation_open_window(
        &workstation->customisation,
        tool_id,
        group_id,
        floating != 0,
        opened_at_ms,
        out_window_id,
        out_window_id_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        workstation->revision += 1U;
    }
    return status;
}

/*
 * Provide the application suite gtk4 workstation move window operation used by this module
 * and its client applications.
 */
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
    UmiUiWorkspaceWindow *window;
    UmiUiWorkspacePanelSettings settings;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    layout = umi_ui_workspace_customisation_active(
        &workstation->customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;

    /* The legacy move API is retained for compatibility, but now publishes
     * one atomic settings request so invalid geometry cannot leave a changed
     * stack or placement behind. */
    settings = umi_ui_workspace_panel_settings_default(window_id);
    settings.placement_id = group_id;
    settings.stack_id = group_id;
    settings.context_group_id = window->context_group_id;
    settings.floating = strcmp(group_id, "floating") == 0;
    settings.x = x;
    settings.y = y;
    settings.width = width;
    settings.height = height;
    return umi_application_suite_gtk4_workstation_apply_panel_settings(
        workstation, &settings);
}

/*
 * Provide the application suite gtk4 workstation close window operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_close_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workspace_customisation_close_window(
        &workstation->customisation, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
        workstation->revision += 1U;
    }
    return status;
}

/*
 * Provide the application suite gtk4 workstation set window pinned operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_window_pinned(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int pinned)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_workspace_layout_set_pinned(
        layout, window_id, pinned != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = rebuild_active_layout(workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/*
 * Provide the application suite gtk4 workstation set window floating operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_window_floating(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int floating)
{
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceWindow *window;
    UmiUiWorkspacePanelSettings settings;
    UmiUiWorkspaceRect floating_bounds;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;

    /* Toggle through the same atomic settings path used by the visible panel
     * editor, ensuring policy and rollback behavior cannot diverge. */
    settings = umi_ui_workspace_panel_settings_default(window_id);
    settings.context_group_id = window->context_group_id;
    settings.floating = floating != 0;
    settings.auto_hidden = false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (settings.floating) {
        floating_bounds =
            umi_ui_workspace_region_rect(UMI_UI_PLACEMENT_FLOATING);
        settings.placement_id = "floating";
        settings.stack_id = window_id;
        settings.x = floating_bounds.x;
        settings.y = floating_bounds.y;
        settings.width = floating_bounds.width;
        settings.height = floating_bounds.height;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        settings.placement_id =
            window->placement_id[0] != '\0' &&
            strcmp(window->placement_id, "floating") != 0
                ? window->placement_id
                : "centre";
        settings.stack_id =
            window->stack_id[0] != '\0' &&
            strcmp(window->stack_id, window_id) != 0
                ? window->stack_id
                : settings.placement_id;
    }
    return umi_application_suite_gtk4_workstation_apply_panel_settings(
        workstation, &settings);
}

/*
 * Provide the application suite gtk4 workstation set window maximised operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_window_maximised(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int maximised)
{
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this operation only while the related capability or state is available. */
    if (!workstation->customisation.edit_active)
        return UMI_STATUS_INVALID_STATE;
    status = umi_ui_workspace_layout_set_maximised(
        layout, window_id, maximised != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = rebuild_active_layout(workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/*
 * Provide the application suite gtk4 workstation set window context group operation used
 * by this module and its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_window_context_group(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    const char *context_group_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window_id == NULL || context_group_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = context_group_id[0] == '\0'
        ? umi_ui_workspace_customisation_clear_context_group(
              &workstation->customisation, window_id)
        : umi_ui_workspace_customisation_assign_context_group(
              &workstation->customisation,
              window_id,
              context_group_id,
              UMI_UI_WINDOW_GROUP_BIDIRECTIONAL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = rebuild_active_layout(workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/* Apply and render one complete panel editor request. */
UmiStatus umi_application_suite_gtk4_workstation_apply_panel_settings(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiWorkspacePanelSettings *settings)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || settings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_suite_customisation_apply_panel_settings(
        &workstation->customisation, settings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = rebuild_active_layout(workstation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        workstation->revision += 1U;
    }
    return status;
}

/* Apply several policy-checked panel requests and rebuild the visible host
 * only after the complete list has committed successfully. */
UmiStatus umi_application_suite_gtk4_workstation_apply_panel_batch(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiWorkspacePanelSettings *settings,
    size_t setting_count)
{
    UmiStatus status;

    /* A live workstation and bounded request list are required. */
    if (workstation == NULL || settings == NULL || setting_count == 0U ||
        setting_count > UMI_UI_WORKSPACE_MAX_PANEL_BATCH) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* The public workstation helper owns the edit lifetime for callers that
     * are responding directly to a multi-selection gesture. */
    status = umi_ui_workspace_customisation_begin_edit(
        &workstation->customisation);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_application_suite_customisation_apply_panel_batch(
        &workstation->customisation,
        settings,
        setting_count);
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_customisation_commit_edit(
            &workstation->customisation);
    } else {
        (void)umi_ui_workspace_customisation_cancel_edit(
            &workstation->customisation);
    }
    /* A failed commit keeps the edit open in the lower-level contract; cancel
     * it before returning so the native host cannot remain unlocked. */
    if (status != UMI_STATUS_OK) {
        if (workstation->customisation.edit_active) {
            (void)umi_ui_workspace_customisation_cancel_edit(
                &workstation->customisation);
        }
        /* Keep the native Edit Layout and New Window controls aligned with
         * the transaction state after a rejected request list. */
        refresh_edit_controls(workstation);
        return status;
    }
    status = rebuild_active_layout(workstation);
    /* The batch owns the edit lifetime, so refresh buttons even when the
     * native rebuild reports an error after the model was committed. */
    refresh_edit_controls(workstation);
    if (status == UMI_STATUS_OK) {
        workstation->revision += 1U;
    }
    return status;
}

/* Return the editor row which represents the panel's current placement. */
static guint panel_editor_region_index(const UmiUiWorkspaceWindow *window)
{
    const char *placement;
    guint index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return 0U;
    placement = window->floating
        ? "floating"
        : window->placement_id;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(placement, "auto-hide:", 10U) == 0) {
        placement += 10U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < G_N_ELEMENTS(PANEL_EDITOR_REGIONS); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(PANEL_EDITOR_REGIONS[index], placement) == 0) {
            return index;
        }
    }
    return 0U;
}

/* Return zero for no link or the one-based Framework context-group row. */
static guint panel_editor_context_index(
    const UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiWorkspaceWindow *window)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window == NULL ||
        window->context_group_id[0] == '\0') {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workstation->customisation.groups.count;
         ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                workstation->customisation.groups.items[index].group_id,
                window->context_group_id) == 0) {
            return (guint)(index + 1U);
        }
    }
    return 0U;
}

/* Refresh the embedded editor from the authoritative layout instead of
 * retaining widget state which may have been cancelled or switched away. */
static void refresh_panel_editor(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id)
{
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceWindow *window;
    char title[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY + 32U];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window_id == NULL) return;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy_text(
            workstation->panel_editor_window_id,
            sizeof(workstation->panel_editor_window_id),
            window_id) != UMI_STATUS_OK) {
        return;
    }
    (void)snprintf(title, sizeof(title), "Panel Settings · %s", window->title);
    gtk_label_set_text(GTK_LABEL(workstation->panel_editor_title), title);
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(workstation->panel_editor_region),
        panel_editor_region_index(window));
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(workstation->panel_editor_context),
        panel_editor_context_index(workstation, window));
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(workstation->panel_editor_auto_hide),
        umi_ui_workspace_customisation_window_is_auto_hidden(
            &workstation->customisation, window_id));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(workstation->panel_editor_bounds[0]), window->x * 100.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(workstation->panel_editor_bounds[1]), window->y * 100.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(workstation->panel_editor_bounds[2]), window->width * 100.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(workstation->panel_editor_bounds[3]), window->height * 100.0);
    gtk_widget_set_visible(workstation->panel_editor_geometry,
        !window->floating && strcmp(window->placement_id, "canvas") == 0);
    gtk_widget_set_sensitive(
        workstation->panel_editor_apply,
        workstation->customisation.edit_active);
    gtk_label_set_text(
        GTK_LABEL(workstation->panel_editor_status),
        workstation->customisation.edit_active
            ? "Choose a placement and context. Canvas bounds use percentages."
            : "Unlock the layout before changing panel settings.");
}

/* Reveal the shared editor for Move, Context and Settings header actions. */
static void show_panel_editor(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->panel_editor_revealer == NULL) {
        return;
    }
    refresh_panel_editor(workstation, window_id);
    gtk_revealer_set_reveal_child(
        GTK_REVEALER(workstation->panel_editor_revealer), TRUE);
}

/* Hide the editor without mutating the layout or its edit baseline. */
static void on_panel_editor_cancel(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation != NULL && workstation->panel_editor_revealer != NULL) {
        gtk_revealer_set_reveal_child(
            GTK_REVEALER(workstation->panel_editor_revealer), FALSE);
    }
}

/* Only docked panels have an edge for auto-hide. Internal canvas geometry is
 * edited in percentages; detached windows remain a separate placement. */
static void on_panel_editor_region_changed(
    GObject *object,
    GParamSpec *property,
    gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    guint region_index;
    gboolean docked;
    gboolean canvas;
    (void)object;
    (void)property;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    region_index = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(workstation->panel_editor_region));
    canvas = region_index < G_N_ELEMENTS(PANEL_EDITOR_REGIONS) &&
        strcmp(PANEL_EDITOR_REGIONS[region_index], "canvas") == 0;
    docked = region_index < 5U;
    gtk_widget_set_sensitive(
        workstation->panel_editor_auto_hide,
        docked);
    gtk_widget_set_visible(workstation->panel_editor_geometry, canvas);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!docked) {
        gtk_check_button_set_active(
            GTK_CHECK_BUTTON(workstation->panel_editor_auto_hide), FALSE);
    }
}

/* Translate the small GTK editor into one toolkit-neutral atomic request. */
static void on_panel_editor_apply(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    UmiUiWorkspacePanelSettings settings;
    UmiUiWorkspaceRect rectangle;
    UmiUiPlacement placement;
    guint region_index;
    guint context_index;
    UmiStatus status;
    (void)button;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL ||
        workstation->panel_editor_window_id[0] == '\0') {
        return;
    }
    region_index = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(workstation->panel_editor_region));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (region_index >= G_N_ELEMENTS(PANEL_EDITOR_REGIONS)) {
        region_index = 0U;
    }
    context_index = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(workstation->panel_editor_context));
    settings = umi_ui_workspace_panel_settings_default(
        workstation->panel_editor_window_id);
    settings.placement_id = PANEL_EDITOR_REGIONS[region_index];
    settings.stack_id = PANEL_EDITOR_REGIONS[region_index];
    settings.floating =
        strcmp(PANEL_EDITOR_REGIONS[region_index], "floating") == 0;
    settings.auto_hidden = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(workstation->panel_editor_auto_hide));
    /* Apply this branch only when its contract condition is satisfied. */
    if (context_index > 0U &&
        (size_t)(context_index - 1U) <
            workstation->customisation.groups.count) {
        settings.context_group_id = workstation->customisation.groups
            .items[context_index - 1U].group_id;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_ui_placement_parse(settings.placement_id, &placement) !=
        UMI_STATUS_OK) {
        placement = UMI_UI_PLACEMENT_CENTRE;
    }
    rectangle = umi_ui_workspace_region_rect(placement);
    settings.x = rectangle.x;
    settings.y = rectangle.y;
    settings.width = rectangle.width;
    settings.height = rectangle.height;
    /* The Framework validates all four values as one candidate, so a width
     * extending past the right edge cannot partially change the placement. */
    if (strcmp(settings.placement_id, "canvas") == 0) {
        settings.auto_hidden = false;
        settings.x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(workstation->panel_editor_bounds[0])) / 100.0;
        settings.y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(workstation->panel_editor_bounds[1])) / 100.0;
        settings.width = gtk_spin_button_get_value(GTK_SPIN_BUTTON(workstation->panel_editor_bounds[2])) / 100.0;
        settings.height = gtk_spin_button_get_value(GTK_SPIN_BUTTON(workstation->panel_editor_bounds[3])) / 100.0;
    }

    status = umi_application_suite_gtk4_workstation_apply_panel_settings(
        workstation, &settings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        gtk_revealer_set_reveal_child(
            GTK_REVEALER(workstation->panel_editor_revealer), FALSE);
        return;
    }
    gtk_label_set_text(
        GTK_LABEL(workstation->panel_editor_status),
        status == UMI_STATUS_INVALID_STATE ||
                status == UMI_STATUS_PERMISSION_DENIED
            ? "This panel or locked layout does not allow that change."
            : "Panel settings could not be applied; no partial change was kept.");
}

/* Build one compact editor shared by every suite product and panel type. */
static GtkWidget *build_panel_editor(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    static const char *REGION_LABELS[] = {
        "Centre", "Left", "Right", "Bottom", "Top", "Detached Window", "Canvas", NULL
    };
    GtkWidget *revealer = gtk_revealer_new();
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *options = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *cancel = gtk_button_new_with_label("Close");
    GtkStringList *contexts = gtk_string_list_new(NULL);
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || revealer == NULL || root == NULL ||
        cancel == NULL || contexts == NULL) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (contexts != NULL) g_object_unref(contexts);
        return revealer;
    }
    workstation->panel_editor_revealer = revealer;
    workstation->panel_editor_title = gtk_label_new("Panel Settings");
    workstation->panel_editor_region =
        gtk_drop_down_new_from_strings(REGION_LABELS);
    gtk_string_list_append(contexts, "No linked context");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workstation->customisation.groups.count;
         ++index) {
        gtk_string_list_append(
            contexts,
            workstation->customisation.groups.items[index].group_id);
    }
    workstation->panel_editor_context =
        umi_ui_gtk4_drop_down_new_take_string_list(contexts);
    workstation->panel_editor_auto_hide =
        gtk_check_button_new_with_label("Auto-hide");
    workstation->panel_editor_apply =
        gtk_button_new_with_label("Apply");
    workstation->panel_editor_status = gtk_label_new("");
    workstation->panel_editor_geometry = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    /* Short labelled number fields are also usable with Tab and arrow keys.
     * The horizontal viewport keeps a large font from widening the host. */
    for (index = 0U; index < 4U; ++index) {
        static const char *labels[] = {"X %", "Y %", "Width %", "Height %"};
        static const char *ids[] = {"umicom.panel.canvas.x", "umicom.panel.canvas.y",
                                    "umicom.panel.canvas.width", "umicom.panel.canvas.height"};
        GtkWidget *field = gtk_spin_button_new_with_range(index < 2U ? 0.0 : 0.1, 100.0, 1.0);
        workstation->panel_editor_bounds[index] = field;
        gtk_spin_button_set_digits(GTK_SPIN_BUTTON(field), 1U);
        gtk_widget_set_tooltip_text(field, labels[index]);
        gtk_accessible_update_property(GTK_ACCESSIBLE(field),
            GTK_ACCESSIBLE_PROPERTY_LABEL, labels[index], -1);
        (void)umi_gtk4_automation_tag_widget(field, ids[index]);
        gtk_box_append(GTK_BOX(workstation->panel_editor_geometry), gtk_label_new(labels[index]));
        gtk_box_append(GTK_BOX(workstation->panel_editor_geometry), field);
    }
    gtk_widget_set_visible(workstation->panel_editor_geometry, FALSE);

    gtk_widget_add_css_class(root, "umicom-panel-settings-editor");
    gtk_widget_add_css_class(workstation->panel_editor_title, "heading");
    gtk_widget_add_css_class(workstation->panel_editor_status, "dim-label");
    gtk_widget_set_hexpand(workstation->panel_editor_status, TRUE);
    gtk_label_set_xalign(GTK_LABEL(workstation->panel_editor_status), 0.0F);
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_title);
    gtk_box_append(GTK_BOX(options), gtk_label_new("Placement"));
    gtk_box_append(GTK_BOX(options), workstation->panel_editor_region);
    gtk_box_append(GTK_BOX(options), gtk_label_new("Context"));
    gtk_box_append(GTK_BOX(options), workstation->panel_editor_context);
    gtk_box_append(GTK_BOX(options), workstation->panel_editor_auto_hide);
    gtk_box_append(GTK_BOX(options), cancel);
    gtk_box_append(GTK_BOX(options), workstation->panel_editor_apply);
    gtk_box_append(GTK_BOX(root), options);
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_geometry);
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_status);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), root);
    gtk_revealer_set_child(GTK_REVEALER(revealer), scroll);
    gtk_revealer_set_reveal_child(GTK_REVEALER(revealer), FALSE);
    g_signal_connect(
        workstation->panel_editor_region,
        "notify::selected",
        G_CALLBACK(on_panel_editor_region_changed),
        workstation);
    g_signal_connect(
        cancel, "clicked", G_CALLBACK(on_panel_editor_cancel), workstation);
    g_signal_connect(
        workstation->panel_editor_apply,
        "clicked",
        G_CALLBACK(on_panel_editor_apply),
        workstation);
    return revealer;
}

/* Stage ordinary tool presentation before publishing the shared model. Native
 * allocation failures retain its old layout and attempt to restore the host. */
static UmiStatus set_normal_tool_presentation(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id, bool visible, bool auto_hidden)
{
    const UmiUiWorkspaceLayout *previous = active_layout(workstation);
    const UmiUiWorkspaceWindow *window = previous != NULL
        ? umi_ui_workspace_layout_find_window(previous, window_id) : NULL;
    const UmiExperiencePanelDefinition *panel;
    UmiUiWorkspaceCustomisation *candidate;
    const UmiUiWorkspaceLayout *candidate_layout;
    UmiStatus status;
    bool rebuild_attempted = false;
    if (workstation == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    if (workstation->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    panel = workstation->runtime.experience != NULL
        ? umi_application_experience_panel_find(workstation->runtime.experience, window->tool_id) : NULL;
    if (panel != NULL && (visible || auto_hidden)) {
        uint32_t required = window->floating ? UMI_EXPERIENCE_PANEL_FLOATABLE : UMI_EXPERIENCE_PANEL_DOCKABLE;
        if (auto_hidden) required |= UMI_EXPERIENCE_PANEL_AUTO_HIDE;
        if ((panel->flags & required) != required) return UMI_STATUS_PERMISSION_DENIED;
    }
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = workstation->customisation;
    status = umi_ui_workspace_customisation_set_tool_presentation(candidate,
        window_id, visible, auto_hidden);
    /* A rail reveal request is not a persistent edit or a reason to rebuild
     * all native tool widgets; the caller reveals the existing host flyout. */
    if (status == UMI_STATUS_OK && candidate->revision == workstation->customisation.revision) {
        free(candidate);
        return UMI_STATUS_OK;
    }
    candidate_layout = umi_ui_workspace_customisation_active_const(candidate);
    if (status == UMI_STATUS_OK) {
        rebuild_attempted = true;
        status = candidate_layout != NULL
            ? umi_gtk4_workspace_layout_host_rebuild(workstation->host, candidate_layout)
            : UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) {
        workstation->customisation = *candidate;
        workstation->revision += 1U;
        refresh_heading(workstation);
        refresh_edit_controls(workstation);
    } else if (rebuild_attempted && previous != NULL) {
        (void)umi_gtk4_workspace_layout_host_rebuild(workstation->host, previous);
    }
    free(candidate);
    return status;
}

/* Open/focus is a normal view operation, not an implicit panel mover. Copy the
 * caller's ID before an accepted model update can replace its backing bytes. */
UmiStatus umi_application_suite_gtk4_workstation_show_window(
    UmiApplicationSuiteGtk4Workstation *workstation, const char *window_id)
{
    char identity[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    const UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;
    UmiStatus status;
    if (workstation == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(identity, sizeof(identity), window_id);
    if (status != UMI_STATUS_OK) return status;
    layout = active_layout(workstation);
    window = layout != NULL ? umi_ui_workspace_layout_find_window(layout, identity) : NULL;
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    if (!can_show_window(workstation, window)) return UMI_STATUS_PERMISSION_DENIED;
    if (umi_ui_workspace_customisation_window_is_auto_hidden(
            &workstation->customisation, identity))
        return umi_gtk4_workspace_layout_host_reveal_tool_window(workstation->host, identity);
    if (!window->visible) {
        status = set_normal_tool_presentation(workstation, identity, true, false);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_gtk4_workspace_layout_host_focus_window(workstation->host, identity);
}

/* A catalogue row opens the first eligible existing instance. Creating another
 * instance, or moving a singleton, continues through explicit Edit Layout. */
static const UmiUiWorkspaceWindow *existing_catalogue_window(
    const UmiApplicationSuiteGtk4Workstation *workstation, const char *tool_id)
{
    const UmiUiWorkspaceLayout *layout = active_layout(workstation);
    const UmiUiWorkspaceWindow *hidden = NULL;
    size_t index;
    if (layout == NULL || tool_id == NULL) return NULL;
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        if (strcmp(window->tool_id, tool_id) != 0 || !can_show_window(workstation, window)) continue;
        if (window->visible) return window;
        if (hidden == NULL) hidden = window;
    }
    return hidden;
}

/* Placement controls explain their edit-only role, while ordinary Open/Focus
 * stays usable when locked. Unavailable definitions remain visible with a reason. */
static void refresh_window_catalogue_controls(UmiApplicationSuiteGtk4Workstation *workstation)
{
    GtkWidget *row;
    const int editing = workstation->customisation.edit_active ? 1 : 0;
    if (workstation->new_window_region != NULL)
        gtk_widget_set_sensitive(workstation->new_window_region, editing);
    if (workstation->new_window_floating != NULL)
        gtk_widget_set_sensitive(workstation->new_window_floating, editing);
    if (workstation->new_window_list == NULL) return;
    for (row = gtk_widget_get_first_child(workstation->new_window_list); row != NULL;
         row = gtk_widget_get_next_sibling(row)) {
        GtkWidget *button = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row));
        const UmiUiWindowDescriptor *descriptor = button != NULL
            ? g_object_get_data(G_OBJECT(button), "umicom-window-descriptor") : NULL;
        const UmiUiWorkspaceWindow *window = descriptor != NULL
            ? existing_catalogue_window(workstation, descriptor->tool_id) : NULL;
        if (button == NULL || descriptor == NULL) continue;
        gtk_widget_set_sensitive(button, editing || window != NULL);
        gtk_widget_set_tooltip_text(button, editing
            ? "Add an instance or move an existing singleton to the selected placement."
            : window != NULL ? "Focus or reopen an existing tool without changing its placement."
            : "No reopenable instance is in this layout. Use Edit Layout to add one or review protection.");
    }
}

/* Provide the on panel action operation used by this module and its client applications. */
static void on_panel_action(const char *window_id,
                            UmiWsPanelAction action,
                            void *user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceWindow *window;
    UmiStatus presentation_status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || window_id == NULL) return;
    layout = umi_ui_workspace_customisation_active(&workstation->customisation);
    window = umi_ui_workspace_layout_find_window_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return;
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
    case UMI_WS_PANEL_ACTION_PIN_TOGGLE:
        (void)umi_application_suite_gtk4_workstation_set_window_pinned(
            workstation, window_id, window->pinned ? 0 : 1);
        break;
    case UMI_WS_PANEL_ACTION_AUTO_HIDE_TOGGLE:
        presentation_status = set_normal_tool_presentation(workstation,
            window_id, true, !umi_ui_workspace_customisation_window_is_auto_hidden(
                &workstation->customisation, window_id));
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
        /* Normal docked tools can be reopened without unlocking or losing
         * their placement. Detached/canvas removal retains its edit policy. */
        if (!window->floating && strcmp(window->placement_id, "canvas") != 0)
            presentation_status = set_normal_tool_presentation(workstation, window_id, false, false);
        else
            (void)umi_application_suite_gtk4_workstation_close_window(workstation, window_id);
        break;
    case UMI_WS_PANEL_ACTION_MOVE:
    case UMI_WS_PANEL_ACTION_CONTEXT_GROUP:
    case UMI_WS_PANEL_ACTION_SETTINGS:
        /* These actions share one predictable editor so users do not have to
         * learn three different popovers for related panel settings. */
        show_panel_editor(workstation, window_id);
        break;
    default:
        /* Unknown future actions remain harmless until their semantic command
         * is understood by this frontend adapter. */
        break;
    }
    if (presentation_status != UMI_STATUS_OK && workstation->layout_status != NULL) {
        gtk_label_set_text(GTK_LABEL(workstation->layout_status), umi_status_text(presentation_status));
        gtk_widget_set_tooltip_text(workstation->layout_status,
            "The requested model update was not committed. Protected tools and unsupported modes remain unchanged.");
    }
}

/* Find on layout while leaving the underlying catalogue or model owned by this module. */
static void on_layout_selected(GObject *object,
                               GParamSpec *pspec,
                               gpointer data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)data;
    guint selected;
    (void)pspec;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->changing_selection) return;
    selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((size_t)selected < workstation->customisation.layout_count)
        (void)umi_application_suite_gtk4_workstation_select_layout(
            workstation, workstation->customisation.layouts[selected].layout_id);
}

/*
 * Provide the text contains ignore case operation used by this module and its client
 * applications.
 */
static int text_contains_ignore_case(const char *text, const char *query)
{
    char *text_casefold;
    char *query_casefold;
    int matches;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    text_casefold = g_utf8_casefold(text, -1);
    query_casefold = g_utf8_casefold(query, -1);
    matches = text_casefold != NULL && query_casefold != NULL &&
        strstr(text_casefold, query_casefold) != NULL;
    g_free(query_casefold);
    g_free(text_casefold);
    return matches;
}

/*
 * Provide the refresh new window filter operation used by this module and its client
 * applications.
 */
static void refresh_new_window_filter(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    const char *query;
    guint category_index;
    UmiUiWindowCategory category;
    GtkWidget *row;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->new_window_list == NULL) return;
    query = gtk_editable_get_text(
        GTK_EDITABLE(workstation->new_window_search));
    category_index = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(workstation->new_window_category));
    category = category_index < G_N_ELEMENTS(WINDOW_CATEGORIES)
        ? WINDOW_CATEGORIES[category_index]
        : 0;
    row = gtk_widget_get_first_child(workstation->new_window_list);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
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

/*
 * Provide the on new window search changed operation used by this module and its client
 * applications.
 */
static void on_new_window_search_changed(
    GtkSearchEntry *entry,
    gpointer user_data)
{
    (void)entry;
    refresh_new_window_filter(
        (UmiApplicationSuiteGtk4Workstation *)user_data);
}

/*
 * Provide the on new window category changed operation used by this module and its client
 * applications.
 */
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

/* Find on new window while leaving the underlying catalogue or model owned by this module. */
static void on_new_window_selected(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    const UmiUiWindowDescriptor *descriptor;
    guint region_index;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    int floating;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    descriptor = (const UmiUiWindowDescriptor *)g_object_get_data(
        G_OBJECT(button), "umicom-window-descriptor");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return;
    region_index = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(workstation->new_window_region));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (region_index >= G_N_ELEMENTS(WINDOW_REGIONS)) region_index = 0U;
    floating = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(workstation->new_window_floating));
    status = UMI_STATUS_NOT_FOUND;

    /* Normal mode never falls through to instance creation, even for a
     * multi-instance definition. The result identifies an existing instance. */
    if (!workstation->customisation.edit_active) {
        const UmiUiWorkspaceWindow *existing =
            existing_catalogue_window(workstation, descriptor->tool_id);
        status = existing != NULL
            ? umi_application_suite_gtk4_workstation_show_window(workstation, existing->window_id)
            : UMI_STATUS_NOT_FOUND;
        goto show_result;
    }

    /* Singleton entries double as a simple panel mover. This mirrors the
     * discoverability of a New Window catalogue without creating duplicate
     * instances of tools that are intentionally unique. */
    if (!descriptor->supports_multiple) {
        UmiUiWorkspaceLayout *layout =
            umi_ui_workspace_customisation_active(&workstation->customisation);
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; layout != NULL && index < layout->window_count; ++index) {
            UmiUiWorkspaceWindow *window = &layout->windows[index];
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(window->tool_id, descriptor->tool_id) == 0) {
                UmiUiPlacement placement;
                UmiApplicationSuiteLayoutRect region;
                /* Normal-mode Open was handled above. Only an explicit edit
                 * is allowed to replace the singleton's placement here. */
                /* Apply this branch only when its contract condition is satisfied. */
                if (floating) {
                    placement = UMI_UI_PLACEMENT_FLOATING;
                } else /* Apply this branch only when its contract condition is satisfied. */ if (umi_ui_placement_parse(
                               WINDOW_REGIONS[region_index], &placement) !=
                           UMI_STATUS_OK) {
                    placement = UMI_UI_PLACEMENT_CENTRE;
                }
                region = umi_application_suite_layout_region_rect(placement);
                /* One atomic move also changes the floating state, avoiding a
                 * visible intermediate arrangement and a second rebuild. */
                status = umi_application_suite_gtk4_workstation_move_window(
                    workstation,
                    window->window_id,
                    floating
                        ? "floating"
                        : WINDOW_REGIONS[region_index],
                    region.x,
                    region.y,
                    region.width,
                    region.height);
                break;
            }
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
show_result:
    /* Report the actual Open/Focus or edit result without silently unlocking. */
    if (status == UMI_STATUS_OK) {
        gtk_label_set_text(
            GTK_LABEL(workstation->new_window_status), "");
        gtk_popover_popdown(GTK_POPOVER(workstation->new_window_popover));
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (workstation->new_window_status != NULL) {
        gtk_label_set_text(
            GTK_LABEL(workstation->new_window_status),
            status == UMI_STATUS_PERMISSION_DENIED ||
                    status == UMI_STATUS_INVALID_STATE
                ? "This panel or mode does not allow that operation. Use Edit Layout to review placement and protection."
                : (status == UMI_STATUS_CAPACITY_EXCEEDED
                    ? "This layout has reached its window capacity."
                    : "No existing tool could be opened. Use Edit Layout to add an instance."));
    }
}

/*
 * Provide the build new window popover operation used by this module and its client
 * applications.
 */
static GtkWidget *build_new_window_popover(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    static const char *REGION_LABELS[] = {
        "Centre", "Left", "Right", "Bottom", "Top", "Canvas", NULL
    };
    static const char *CATEGORY_LABELS[] = {
        "All categories", "Development", "Navigation", "Operations",
        "Data", "AI", "Trading", "General", NULL
    };
    GtkWidget *popover = gtk_popover_new();
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *heading = gtk_label_new("Windows and tools");
    GtkWidget *help = gtk_label_new(
        "Open or focus an existing tool. Edit Layout enables destination "
        "controls for adding or moving panels.");
    GtkWidget *options = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *scroll = gtk_scrolled_window_new();
    UmiUiWindowCatalogueQuery catalogue_query =
        umi_ui_window_catalogue_query_default();
    UmiUiWindowCatalogueResult catalogue_result = {0};
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Shared IDs make the catalogue testable in every Umicom application. */
    (void)umi_gtk4_automation_tag_widget(
        workstation->new_window_search,
        "umicom.window-catalogue.search");
    (void)umi_gtk4_automation_tag_widget(
        workstation->new_window_category,
        "umicom.window-catalogue.category");
    (void)umi_gtk4_automation_tag_widget(
        workstation->new_window_region,
        "umicom.window-catalogue.region");
    (void)umi_gtk4_automation_tag_widget(
        workstation->new_window_floating,
        "umicom.window-catalogue.floating");

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
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue_result.count; ++index) {
        const UmiUiWindowDescriptor *descriptor =
            catalogue_result.items[index].descriptor;
        GtkWidget *button = gtk_button_new();
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        GtkWidget *title = gtk_label_new(descriptor->title);
        GtkWidget *detail = gtk_label_new(descriptor->description);
        char category[96];

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
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
        /* The catalogue tool identifier is the public, stable window type key.
         * Reusing it for automation keeps the adapter aligned with the current
         * descriptor contract when the catalogue is filtered or sorted. */
        (void)umi_gtk4_automation_tag_widget(
            button,
            descriptor->tool_id);
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

/*
 * Provide the on edit layout clicked operation used by this module and its client
 * applications.
 */
static void on_edit_layout_clicked(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    /* Apply this operation only while the related capability or state is available. */
    if (workstation->customisation.edit_active) {
        (void)umi_application_suite_gtk4_workstation_commit_layout_edit(
            workstation);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)umi_application_suite_gtk4_workstation_begin_layout_edit(
            workstation);
    }
}

/*
 * Provide the on cancel layout edit clicked operation used by this module and its client
 * applications.
 */
static void on_cancel_layout_edit_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)umi_application_suite_gtk4_workstation_cancel_layout_edit(
        (UmiApplicationSuiteGtk4Workstation *)user_data);
}

/* Save a committed checkpoint and provide plain-language feedback beside the
 * layout controls so a beginner can tell whether the action succeeded. */
static void on_save_layout_clicked(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    UmiStatus status;
    gint64 now_us;

    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) {
        return;
    }
    now_us = g_get_real_time();
    status = now_us > 0 && (uint64_t)now_us > UINT64_MAX / UINT64_C(1000)
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : umi_application_suite_gtk4_workstation_save_checkpoint(
            workstation,
            now_us > 0 ? (uint64_t)now_us * UINT64_C(1000) : UINT64_C(0));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    checkpoint_feedback(workstation, status,
        workstation->checkpoint_report.durable && workstation->checkpoint_server != NULL
            ? "Layout saved to disk."
            : "Layout saved for this session only.");
}

/* Restore the last checkpoint through the validated Framework importer. */
static void on_restore_layout_clicked(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    UmiStatus status;

    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) {
        return;
    }
    status = umi_application_suite_gtk4_workstation_restore_checkpoint(
        workstation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    checkpoint_feedback(workstation, status,
        workstation->checkpoint_report.recovered_last_good
            ? "Recovered last-known-good saved layout; review before saving."
            : workstation->checkpoint_report.durable && workstation->checkpoint_server != NULL
                ? "Saved layout restored from disk." : "Session layout restored.");
}

/* A random stable ID separates identity from the editable display name. No
 * existing preset, saved file or product document is removed by this action. */
static void on_create_blank_layout_clicked(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation = user_data;
    gchar *id;
    gchar *name;
    gchar *qualified_id;
    UmiStatus status;
    (void)button;
    if (workstation == NULL) return;
    name = g_strdup(gtk_editable_get_text(GTK_EDITABLE(workstation->new_layout_name)));
    g_strstrip(name);
    id = g_uuid_string_random();
    qualified_id = g_strdup_printf("%s.%s",
        workstation->runtime.experience->application_id, id);
    status = umi_application_suite_gtk4_workstation_create_blank_layout(workstation, qualified_id, name);
    g_free(qualified_id);
    g_free(id);
    g_free(name);
    gtk_label_set_text(GTK_LABEL(workstation->layout_status),
        status == UMI_STATUS_OK ? "Empty layout created. Choose Edit Layout to add panels."
            : "Layout could not be created. Enter a name and finish any active edit.");
    if (status == UMI_STATUS_OK)
        gtk_popover_popdown(GTK_POPOVER(workstation->new_layout_popover));
}

/* Clearing is part of Edit Layout, so Cancel restores removed view instances.
 * The result explains why protected panels may remain on the canvas. */
static void on_clear_canvas_clicked(GtkButton *button, gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation = user_data;
    UmiUiWorkspaceCanvasClearResult result = {0U, 0U};
    UmiStatus status;
    (void)button;
    if (workstation == NULL) return;
    status = umi_application_suite_gtk4_workstation_clear_canvas(workstation, &result);
    gtk_label_set_text(GTK_LABEL(workstation->layout_status),
        status != UMI_STATUS_OK ? "Canvas could not be cleared. Unlock the layout first."
            : result.retained != 0U ? "Removable panels cleared; protected panels remain. Cancel restores them."
            : "Panels cleared. Apply and Lock to keep this layout, or Cancel to restore them.");
}

/* Route one shared command-bar item to the same validated operations used by
 * the visible buttons. The command centre never changes layout state itself. */
static void on_command_bar_activated(
    const UmiWsCommandBarItem *item,
    void *user_data)
{
    static const char LAYOUT_PREFIX[] = "suite.choice.";
    static const char PANEL_PREFIX[] = "suite.panel.";
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || item == NULL || !item->enabled) return;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(item->command_id, "suite.layout.edit") == 0) {
        on_edit_layout_clicked(NULL, workstation);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(item->command_id, "suite.layout.save") == 0) {
        on_save_layout_clicked(NULL, workstation);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(item->command_id, "suite.layout.restore") == 0) {
        on_restore_layout_clicked(NULL, workstation);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(item->command_id, "suite.window.open") == 0) {
        /* The existing catalogue remains the single owner of panel filters,
         * placement choices and multi-instance policy. */
        if (workstation->new_window_button != NULL) {
            gtk_menu_button_popup(
                GTK_MENU_BUTTON(workstation->new_window_button));
        }
    } else if (strncmp(item->item_id, PANEL_PREFIX, sizeof(PANEL_PREFIX) - 1U) == 0) {
        UmiStatus status = umi_application_suite_gtk4_workstation_show_window(
            workstation, item->command_id);
        if (status != UMI_STATUS_OK && workstation->layout_status != NULL)
            gtk_label_set_text(GTK_LABEL(workstation->layout_status),
                "The selected panel is no longer available in this arrangement.");
    } else /* Resolve the copied saved-layout ID, never the result-row index. */ if (strncmp(
                   item->item_id,
                   LAYOUT_PREFIX,
                   sizeof(LAYOUT_PREFIX) - 1U) == 0) {
        (void)umi_application_suite_gtk4_workstation_select_layout(
            workstation,
            item->command_id);
    }
}

/* The scroll viewport reports the actual header space on the current monitor.
 * Give one quarter to search and let Framework choose the compact form. */
static void on_header_width_changed(
    GObject *object,
    GParamSpec *property,
    gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    int width;
    int command_width;
    (void)property;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->command_bar == NULL ||
        object == NULL) {
        return;
    }
    width = gtk_widget_get_width(GTK_WIDGET(object));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (width <= 0) return;
    command_width = width / 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (command_width > 420) command_width = 420;
    (void)umi_gtk4_ws_command_bar_set_available_width(
        workstation->command_bar, command_width);
}

/*
 * Initialise application suite gtk4 workstation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_suite_gtk4_workstation_create(
    const UmiApplicationSuiteGtk4WorkstationConfig *config,
    UmiApplicationSuiteGtk4Workstation **out_workstation)
{
    UmiApplicationSuiteGtk4Workstation *workstation;
    const UmiUiWorkspaceLayout *layout;
    GtkStringList *choices;
    GtkWidget *header;
    GtkWidget *header_scroll;
    GtkWidget *label;
    UmiGtk4AppearanceEditorConfig appearance_config;
    UmiGtk4WorkstationShellHeaderConfig identity_config;
    UmiGtk4WorkstationCommandBarConfig command_bar_config;
    size_t index;
    int written;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || out_workstation == NULL ||
        config->application_id == NULL || config->application_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_workstation = NULL;
    workstation = (UmiApplicationSuiteGtk4Workstation *)calloc(
        1U, sizeof(*workstation));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Every graphical application receives one Framework-owned canvas registry
     * before GTK widgets are created; future adapters can detach panels or open
     * another host without inventing a second layout owner. */
    umi_ui_workbench_canvas_init(&workstation->canvas);
    umi_application_suite_layout_runtime_init(&workstation->runtime);
    status = umi_application_suite_layout_runtime_load(
        &workstation->runtime, config->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    layout = umi_application_suite_layout_runtime_active(&workstation->runtime);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) { status = UMI_STATUS_INVALID_STATE; goto fail; }
    status = umi_application_suite_layout_selector_build(
        workstation->runtime.experience, layout->layout_id, &workstation->selector);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = build_command_model(workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;

    /* Load panels, layouts and linked-context groups through the single
     * Framework customisation path. Shared catalogue entries are merged after
     * product definitions so a richer product descriptor always wins. */
    status = umi_application_suite_customisation_load_experience(
        &workstation->customisation, workstation->runtime.experience);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_seed_window_catalogue(
            &workstation->customisation.windows);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    /* Derive a stable host identity from the canonical application identity so
     * multiple products can coexist without sharing routing state. */
    written = snprintf(
        workstation->canvas_host_id,
        sizeof(workstation->canvas_host_id),
        "%s.host",
        config->application_id);
    if (written < 0 || (size_t)written >= sizeof(workstation->canvas_host_id)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto fail;
    }
    /* Register the already-loaded customisation model without loading it a
     * second time; the GTK host and the portable canvas now share one layout. */
    status = umi_ui_workbench_canvas_add_host(
        &workstation->canvas,
        workstation->canvas_host_id,
        config->application_id,
        "primary",
        &workstation->customisation);
    if (status != UMI_STATUS_OK) goto fail;
    layout = active_layout(workstation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) { status = UMI_STATUS_INVALID_STATE; goto fail; }
    status = umi_gtk4_workspace_layout_host_create_with_options(
        layout,
        config->panel_factory,
        config->user_data,
        on_panel_action,
        workstation,
        &(const UmiGtk4WorkspaceLayoutHostOptions){ .tool_presentation_actions = 1 },
        &workstation->host);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;

    /* The renderer owns gesture previews; the existing customisation model
     * remains the only owner of accepted layout changes and Cancel history. */
    status = umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(
        workstation->host, on_canvas_geometry, workstation);
    if (status != UMI_STATUS_OK) goto fail;
    /* Resolve colour from the same group store that owns context routing;
     * group IDs such as development.blue are not themselves CSS colours. */
    status = umi_gtk4_workspace_layout_host_set_context_groups(
        workstation->host, &workstation->customisation.groups);
    if (status != UMI_STATUS_OK) goto fail;

    workstation->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->root == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto fail;
    }
    g_object_ref_sink(workstation->root);
    appearance_config = umi_gtk4_appearance_editor_config_default(
        config->application_id);
    status = umi_gtk4_appearance_editor_create(
        workstation->root, &appearance_config, &workstation->appearance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;

    /* Build product identity through the shared component. This keeps suite
     * applications thin and gives each one the same contrast-aware SVG mark. */
    identity_config = umi_gtk4_ws_shell_header_config_default(
        config->application_id,
        config->title != NULL ? config->title : layout->name);
    identity_config.subtitle = layout->name;
    identity_config.mode_badge = config->mode_badge;
    status = umi_gtk4_ws_shell_header_create_managed(
        &identity_config, &workstation->identity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_gtk4_appearance_editor_set_changed_handler(
        workstation->appearance,
        on_appearance_changed,
        workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;

    /* The suite header uses the same managed search renderer as Studio. A
     * compact initial width protects charts and editors on laptop screens. */
    command_bar_config = umi_gtk4_ws_command_bar_config_default();
    command_bar_config.placeholder = "Search layouts and windows";
    command_bar_config.compact_placeholder = "Quick action";
    command_bar_config.initial_available_width = 240;
    status = umi_gtk4_ws_command_bar_create_managed(
        &command_bar_config,
        &workstation->command_model,
        &workstation->command_bar);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_gtk4_ws_command_bar_set_activated_handler(
        workstation->command_bar,
        on_command_bar_activated,
        workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;

    header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    header_scroll = gtk_scrolled_window_new();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header == NULL || header_scroll == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto fail;
    }
    gtk_widget_add_css_class(header, "umicom-suite-layout-header");
    gtk_box_append(
        GTK_BOX(header),
        umi_gtk4_ws_shell_header_widget(workstation->identity));
    gtk_box_append(
        GTK_BOX(header),
        umi_gtk4_ws_command_bar_widget(workstation->command_bar));
    label = gtk_label_new("Layout");
    gtk_widget_add_css_class(label, "dim-label");
    gtk_box_append(GTK_BOX(header), label);

    choices = gtk_string_list_new(NULL);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workstation->selector.count; ++index)
        gtk_string_list_append(choices, workstation->selector.choices[index].title);
    workstation->layout_dropdown =
        umi_ui_gtk4_drop_down_new_take_string_list(choices);
    (void)umi_gtk4_automation_tag_widget(
        workstation->layout_dropdown,
        "umicom.layout.selector");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(workstation->layout_dropdown),
                               (guint)workstation->selector.selected_index);
    g_signal_connect(workstation->layout_dropdown, "notify::selected",
                     G_CALLBACK(on_layout_selected), workstation);
    gtk_box_append(GTK_BOX(header), workstation->layout_dropdown);
    refresh_layout_choices(workstation);

    /* All clients of this Framework workstation share one library component. */
    {
        GtkWidget *library_button = gtk_menu_button_new();
        gtk_menu_button_set_label(GTK_MENU_BUTTON(library_button), "Layout Library");
        (void)umi_gtk4_automation_tag_widget(library_button, "umicom.layout.library");
        gtk_box_append(GTK_BOX(header), library_button);
        status = umi_gtk4_ws_layout_library_create(suite_layout_library_read,
            suite_layout_library_apply, workstation, &workstation->layout_library);
        if (status != UMI_STATUS_OK) goto fail;
        gtk_menu_button_set_popover(GTK_MENU_BUTTON(library_button),
            umi_gtk4_ws_layout_library_popover(workstation->layout_library));
        status = umi_gtk4_ws_layout_library_set_storage_handlers(workstation->layout_library,
            suite_library_storage_read, suite_library_storage_operation, workstation);
        if (status != UMI_STATUS_OK) goto fail;
    }

    /* Keep layout creation reachable even on a completely empty canvas. */
    {
        GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *create = gtk_button_new_with_label("Create empty layout");
        workstation->new_layout_button = gtk_menu_button_new();
        workstation->new_layout_popover = gtk_popover_new();
        workstation->new_layout_name = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(workstation->new_layout_name), "Layout name");
        gtk_entry_set_max_length(GTK_ENTRY(workstation->new_layout_name),
                                 UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY - 1);
        gtk_widget_set_margin_start(content, 10);
        gtk_widget_set_margin_end(content, 10);
        gtk_widget_set_margin_top(content, 10);
        gtk_widget_set_margin_bottom(content, 10);
        gtk_box_append(GTK_BOX(content), gtk_label_new("Create your own arrangement"));
        gtk_box_append(GTK_BOX(content), workstation->new_layout_name);
        gtk_box_append(GTK_BOX(content), create);
        gtk_popover_set_child(GTK_POPOVER(workstation->new_layout_popover), content);
        gtk_menu_button_set_label(GTK_MENU_BUTTON(workstation->new_layout_button), "New Layout");
        gtk_menu_button_set_popover(GTK_MENU_BUTTON(workstation->new_layout_button),
                                    workstation->new_layout_popover);
        (void)umi_gtk4_automation_tag_widget(workstation->new_layout_button, "umicom.layout.new");
        (void)umi_gtk4_automation_tag_widget(workstation->new_layout_name, "umicom.layout.name");
        (void)umi_gtk4_automation_tag_widget(create, "umicom.layout.create-blank");
        g_signal_connect(create, "clicked", G_CALLBACK(on_create_blank_layout_clicked), workstation);
        gtk_box_append(GTK_BOX(header), workstation->new_layout_button);
    }

    /* Appearance belongs beside layout selection because both controls alter
     * presentation without changing any application data or business logic. */
    gtk_box_append(
        GTK_BOX(header),
        umi_gtk4_appearance_editor_widget(workstation->appearance));

    workstation->new_window_button = gtk_menu_button_new();
    (void)umi_gtk4_automation_tag_widget(
        workstation->new_window_button,
        "umicom.window-catalogue.menu");
    gtk_menu_button_set_label(
        GTK_MENU_BUTTON(workstation->new_window_button), "Windows");
    gtk_widget_set_tooltip_text(
        workstation->new_window_button,
        "Add a Framework panel to the unlocked layout");
    gtk_menu_button_set_popover(
        GTK_MENU_BUTTON(workstation->new_window_button),
        build_new_window_popover(workstation));
    gtk_box_append(GTK_BOX(header), workstation->new_window_button);

    /* Save and Restore are Framework controls shared by Studio, Trader and
     * future application shells; products do not implement their own codecs. */
    workstation->save_layout_button = gtk_button_new_with_label("Save");
    workstation->restore_layout_button = gtk_button_new_with_label("Restore");
    workstation->layout_status = gtk_label_new("");
    /* Long status messages must not increase the shell's minimum width. */
    gtk_label_set_ellipsize(GTK_LABEL(workstation->layout_status), PANGO_ELLIPSIZE_END);
    gtk_label_set_max_width_chars(GTK_LABEL(workstation->layout_status), 34);
    (void)umi_gtk4_automation_tag_widget(
        workstation->save_layout_button,
        "umicom.layout.save");
    (void)umi_gtk4_automation_tag_widget(
        workstation->restore_layout_button,
        "umicom.layout.restore");
    gtk_widget_add_css_class(workstation->save_layout_button, "flat");
    gtk_widget_add_css_class(workstation->restore_layout_button, "flat");
    gtk_widget_add_css_class(workstation->layout_status, "dim-label");
    gtk_widget_set_tooltip_text(
        workstation->save_layout_button,
        "Save the current locked panel layout as a recovery checkpoint");
    gtk_widget_set_tooltip_text(
        workstation->restore_layout_button,
        "Restore the last layout checkpoint saved in this session");
    g_signal_connect(
        workstation->save_layout_button,
        "clicked",
        G_CALLBACK(on_save_layout_clicked),
        workstation);
    g_signal_connect(
        workstation->restore_layout_button,
        "clicked",
        G_CALLBACK(on_restore_layout_clicked),
        workstation);
    gtk_box_append(GTK_BOX(header), workstation->save_layout_button);
    gtk_box_append(GTK_BOX(header), workstation->restore_layout_button);
    gtk_box_append(GTK_BOX(header), workstation->layout_status);

    workstation->edit_layout_button =
        gtk_button_new_with_label("Edit Layout");
    workstation->cancel_edit_button =
        gtk_button_new_with_label("Cancel");
    (void)umi_gtk4_automation_tag_widget(
        workstation->edit_layout_button,
        "umicom.layout.edit");
    (void)umi_gtk4_automation_tag_widget(
        workstation->cancel_edit_button,
        "umicom.layout.cancel-edit");
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
    workstation->clear_canvas_button = gtk_button_new_with_label("Clear Panels");
    gtk_widget_set_tooltip_text(workstation->clear_canvas_button,
        "Remove unpinned, closable panel instances from this edit; Cancel restores them. Product data is not deleted.");
    (void)umi_gtk4_automation_tag_widget(workstation->clear_canvas_button, "umicom.layout.clear-canvas");
    g_signal_connect(workstation->clear_canvas_button, "clicked", G_CALLBACK(on_clear_canvas_clicked), workstation);
    gtk_box_append(GTK_BOX(header), workstation->clear_canvas_button);
    /* A narrow laptop window may not fit every layout command. Horizontal
     * scrolling preserves each command and avoids forcing the application
     * wider than the monitor. */
    gtk_widget_add_css_class(
        header_scroll, "umicom-suite-layout-header-scroll");
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(header_scroll),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_NEVER);
    gtk_scrolled_window_set_propagate_natural_width(
        GTK_SCROLLED_WINDOW(header_scroll), FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(header_scroll), header);
    g_signal_connect(
        header_scroll,
        "notify::width",
        G_CALLBACK(on_header_width_changed),
        workstation);
    gtk_box_append(GTK_BOX(workstation->root), header_scroll);
    gtk_box_append(
        GTK_BOX(workstation->root), build_panel_editor(workstation));
    gtk_widget_set_hexpand(umi_gtk4_workspace_layout_host_widget(workstation->host),
                           TRUE);
    gtk_widget_set_vexpand(umi_gtk4_workspace_layout_host_widget(workstation->host),
                           TRUE);
    gtk_box_append(GTK_BOX(workstation->root),
                   umi_gtk4_workspace_layout_host_widget(workstation->host));
    refresh_heading(workstation);
    refresh_edit_controls(workstation);
    /* Retain one in-process driver after every shared control has been built. */
    status = umi_gtk4_automation_driver_create(
        workstation->root,
        &workstation->automation);
    if (status != UMI_STATUS_OK) goto fail;
    workstation->revision = 1U;
    *out_workstation = workstation;
    return UMI_STATUS_OK;

fail:
    umi_application_suite_gtk4_workstation_destroy(workstation);
    return status;
}

/* Invalidate the workstation's borrowed signal targets before releasing its
 * storage, even when a parent window or automation retains the old widgets. */
static void disconnect_workstation_signals(
    GtkWidget *root, UmiApplicationSuiteGtk4Workstation *workstation)
{
    GtkWidget *child;
    if (root == NULL) return;
    g_signal_handlers_disconnect_by_data(root, workstation);
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        disconnect_workstation_signals(child, workstation);
}

/*
 * Release or reset state held by application suite gtk4 workstation so the same storage
 * can be reused safely.
 */
void umi_application_suite_gtk4_workstation_destroy(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    disconnect_workstation_signals(workstation->root, workstation);
    /* Unregister the borrowed customisation before the workstation storage is released.
     * This keeps the shared canvas from retaining a pointer to an object that is about
     * to disappear and also repairs the active-host selection for other windows. */
    if (workstation->canvas_host_id[0] != '\0') {
        (void)umi_ui_workbench_canvas_remove_host(
            &workstation->canvas, workstation->canvas_host_id);
        workstation->canvas_host_id[0] = '\0';
    }
    /* Release the driver's retained root before dismantling child services. */
    umi_gtk4_automation_driver_destroy(workstation->automation);
    workstation->automation = NULL;
    /* Deferred library requests borrow this workstation and must stop first. */
    umi_gtk4_ws_layout_library_destroy(workstation->layout_library);
    workstation->layout_library = NULL;
    umi_gtk4_workspace_layout_host_destroy(workstation->host);
    workstation->host = NULL;
    /* Disconnect the borrowed callback before releasing its target. */
    (void)umi_gtk4_appearance_editor_set_changed_handler(
        workstation->appearance, NULL, NULL);
    (void)umi_gtk4_ws_command_bar_set_activated_handler(
        workstation->command_bar, NULL, NULL);
    umi_gtk4_ws_command_bar_destroy(workstation->command_bar);
    workstation->command_bar = NULL;
    if (workstation->bound_window != NULL)
        g_object_remove_weak_pointer(G_OBJECT(workstation->bound_window),
            (gpointer *)&workstation->bound_window);
    workstation->bound_window = NULL;
    /* Binding transfers identity ownership. Never destroy the borrowed alias
     * twice; unbound embedded clients retain the original header ownership. */
    if (workstation->titlebar != NULL)
        umi_gtk4_ws_window_titlebar_destroy(workstation->titlebar);
    else
        umi_gtk4_ws_shell_header_destroy(workstation->identity);
    workstation->titlebar = NULL;
    workstation->identity = NULL;
    umi_gtk4_appearance_editor_destroy(workstation->appearance);
    workstation->appearance = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->root != NULL) {
        GtkWidget *child;
        /* A caller may keep the stable root parented. Leave it empty instead
         * of leaving visible controls that refer to released controllers. */
        while ((child = gtk_widget_get_first_child(workstation->root)) != NULL)
            gtk_box_remove(GTK_BOX(workstation->root), child);
        g_object_unref(workstation->root);
    }
    workstation->root = NULL;
    /* The checkpoint owns its encoded buffer independently of GTK widgets. */
    free(workstation->saved_layout_text);
    workstation->saved_layout_text = NULL;
    umi_data_server_destroy(workstation->owned_checkpoint_server);
    workstation->owned_checkpoint_server = NULL;
    workstation->checkpoint_server = NULL;
    free(workstation);
}

/*
 * Provide the application suite gtk4 workstation widget operation used by this module and
 * its client applications.
 */
GtkWidget *umi_application_suite_gtk4_workstation_widget(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    return workstation != NULL ? workstation->root : NULL;
}

/* Copy the shared workstation's borrowed UAT interface for a suite application. */
UmiStatus umi_application_suite_gtk4_workstation_automation(
    UmiApplicationSuiteGtk4Workstation *workstation,
    UmiUiAutomationDriver *out_driver)
{
    if (workstation == NULL || workstation->automation == NULL ||
        out_driver == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_driver = umi_gtk4_automation_driver_interface(
        workstation->automation);
    return UMI_STATUS_OK;
}

/*
 * Provide the application suite gtk4 workstation refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_suite_gtk4_workstation_refresh(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Rebuilding asks the borrowed factory for fresh views while preserving
     * the active layout, docking choices and appearance profile. */
    status = rebuild_active_layout(workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/*
 * Provide the application suite gtk4 workstation snapshot operation used by this module
 * and its client applications.
 */
UmiApplicationSuiteGtk4WorkstationSnapshot
umi_application_suite_gtk4_workstation_snapshot(
    const UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiApplicationSuiteGtk4WorkstationSnapshot snapshot;
    UmiGtk4WorkspaceLayoutHostSnapshot host_snapshot;
    const UmiUiWorkspaceLayout *layout;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return snapshot;
    layout = active_layout(workstation);
    host_snapshot = umi_gtk4_workspace_layout_host_snapshot(workstation->host);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->runtime.experience != NULL)
        (void)copy_text(snapshot.application_id, sizeof(snapshot.application_id),
                        workstation->runtime.experience->application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout != NULL) {
        (void)copy_text(snapshot.active_layout_id,
                        sizeof(snapshot.active_layout_id), layout->layout_id);
        (void)copy_text(snapshot.active_layout_name,
                        sizeof(snapshot.active_layout_name), layout->name);
        /* Keep the established short ID for canonical snapshots. Custom
         * canvases have only their qualified stable ID and expose that. */
        for (size_t index = 0U; index < workstation->selector.count; ++index) {
            char qualified[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
            const char *short_id = workstation->selector.choices[index].layout_id;
            if (qualify_layout_id(qualified, sizeof(qualified),
                    workstation->runtime.experience->application_id, short_id) == UMI_STATUS_OK &&
                strcmp(layout->layout_id, qualified) == 0) {
                (void)copy_text(snapshot.active_layout_id,
                    sizeof(snapshot.active_layout_id), short_id);
                break;
            }
        }
    }
    snapshot.layout_count = workstation->customisation.layout_count;
    snapshot.canvas_panel_count = host_snapshot.canvas_count;
    snapshot.source_layout_revision = host_snapshot.source_layout_revision;
    snapshot.rendered_panel_count = host_snapshot.panel_count;
    snapshot.placeholder_count = host_snapshot.placeholder_count;
    snapshot.available_window_count = workstation->customisation.windows.count;
    snapshot.recent_window_count = workstation->customisation.windows.recent_count;
    snapshot.context_group_count = workstation->customisation.groups.count;
    snapshot.identity = umi_gtk4_ws_shell_header_snapshot(
        workstation->identity);
    snapshot.appearance = umi_gtk4_appearance_editor_snapshot(
        workstation->appearance);
    snapshot.command_bar = umi_gtk4_ws_command_bar_snapshot(
        workstation->command_bar);
    snapshot.layout_locked = active_layout(workstation) != NULL &&
        active_layout(workstation)->locked;
    snapshot.editing_layout = workstation->customisation.edit_active;
    snapshot.has_saved_layout = workstation->saved_layout_text != NULL;
    snapshot.saved_layout_at_ns = workstation->saved_layout_at_ns;
    snapshot.checkpoint_storage_bound = workstation->checkpoint_server != NULL;
    snapshot.checkpoint_storage_durable = workstation->checkpoint_server != NULL &&
        workstation->checkpoint_report.durable;
    snapshot.checkpoint_storage_status = workstation->checkpoint_storage_status;
    snapshot.checkpoint_storage_revision = workstation->checkpoint_report.storage_revision;
    snapshot.revision = workstation->revision + host_snapshot.revision +
        snapshot.identity.revision + snapshot.appearance.revision +
        snapshot.command_bar.revision;
    return snapshot;
}
