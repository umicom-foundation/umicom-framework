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
#include "umicom/application/suite_layout/customisation.h"
#include "umicom/desktop/ui_bridge.h"
#include "umicom/ui/workspace_customisation.h"
#include "umicom/ui/workspace_geometry.h"

struct UmiApplicationSuiteGtk4Workstation {
    UmiApplicationSuiteLayoutRuntime runtime;
    UmiApplicationSuiteLayoutSelectorModel selector;
    UmiUiWorkspaceCustomisation customisation;
    UmiGtk4WorkspaceLayoutHost *host;
    UmiGtk4AppearanceEditor *appearance;
    UmiGtk4WorkstationShellHeader *identity;
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
    char panel_editor_window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char *saved_layout_text;
    uint64_t saved_layout_at_ns;
    int changing_selection;
    uint64_t revision;
};

static const char *WINDOW_REGIONS[] = {
    "centre", "left", "right", "bottom", "top"
};

/* The panel editor also offers Floating as an explicit placement rather than
 * hiding detachment behind an icon whose result is difficult to predict. */
static const char *PANEL_EDITOR_REGIONS[] = {
    "centre", "left", "right", "bottom", "top", "floating"
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

/* Build the shared suite action catalogue from real layout metadata. Product
 * applications inherit these actions and do not duplicate header commands. */
static UmiStatus build_command_model(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ws_command_bar_model_init(&workstation->command_model);

    status = umi_ws_command_bar_model_add(
        &workstation->command_model,
        "suite.layout.edit",
        "Edit or apply layout",
        "Unlock panel movement, or apply and lock the current arrangement.",
        "suite.layout.edit",
        "layout lock unlock customise arrange",
        UMI_WS_COMMAND_SCOPE_COMMAND,
        100U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ws_command_bar_model_add(
            &workstation->command_model,
            "suite.window.open",
            "Add or move a window",
            "Open the shared window catalogue for the unlocked layout.",
            "suite.window.open",
            "panel tool dock float new window",
            UMI_WS_COMMAND_SCOPE_PANEL,
            95U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ws_command_bar_model_add(
            &workstation->command_model,
            "suite.layout.save",
            "Save layout checkpoint",
            "Keep a recovery copy of the current locked arrangement.",
            "suite.layout.save",
            "layout checkpoint recovery",
            UMI_WS_COMMAND_SCOPE_COMMAND,
            90U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ws_command_bar_model_add(
            &workstation->command_model,
            "suite.layout.restore",
            "Restore layout checkpoint",
            "Return to the most recently saved arrangement.",
            "suite.layout.restore",
            "layout checkpoint recover reset",
            UMI_WS_COMMAND_SCOPE_COMMAND,
            85U);
    }

    /* Every canonical layout becomes discoverable through the same command
     * centre. Its identifier is copied into a bounded namespaced command. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < workstation->selector.count;
         ++index) {
        const UmiApplicationSuiteLayoutChoice *choice =
            &workstation->selector.choices[index];
        char command_id[UMI_UI_ID_CAPACITY];
        char description[UMI_UI_TEXT_CAPACITY];
        int written;

        written = snprintf(
            command_id, sizeof(command_id), "suite.layout.%s",
            choice->layout_id);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(command_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        written = snprintf(
            description,
            sizeof(description),
            "Switch this application to the %s workspace.",
            choice->title);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(description)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_ws_command_bar_model_add(
            &workstation->command_model,
            command_id,
            choice->title,
            description,
            command_id,
            choice->layout_id,
            UMI_WS_COMMAND_SCOPE_COMMAND,
            50U);
    }
    return status;
}

/* Synchronise availability after edit or checkpoint state changes. A disabled
 * action remains documented in search results but cannot be dispatched. */
static void refresh_command_model(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    int editing;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    editing = workstation->customisation.edit_active ? 1 : 0;
    (void)umi_ws_command_bar_model_set_enabled(
        &workstation->command_model, "suite.window.open", editing != 0);
    (void)umi_ws_command_bar_model_set_enabled(
        &workstation->command_model, "suite.layout.save", editing == 0);
    (void)umi_ws_command_bar_model_set_enabled(
        &workstation->command_model,
        "suite.layout.restore",
        editing == 0 && workstation->saved_layout_text != NULL);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->command_bar != NULL) {
        (void)umi_gtk4_ws_command_bar_set_model(
            workstation->command_bar, &workstation->command_model);
    }
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

/* Provide the refresh heading operation used by this module and its client applications. */
static void refresh_heading(UmiApplicationSuiteGtk4Workstation *workstation)
{
    const UmiApplicationSuiteLayoutChoice *choice;
    UmiGtk4WorkstationShellHeaderSnapshot identity;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->identity == NULL) return;
    choice = umi_application_suite_layout_selector_current(
        &workstation->selector);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (choice == NULL) return;

    /* The product name remains stable while the smaller second line explains
     * which workspace layout is currently active. */
    identity = umi_gtk4_ws_shell_header_snapshot(workstation->identity);
    (void)umi_gtk4_ws_shell_header_set_text(
        workstation->identity,
        identity.title,
        choice->title,
        identity.mode_badge);
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || layout == NULL) return UMI_STATUS_INVALID_STATE;
    return umi_gtk4_workspace_layout_host_rebuild(workstation->host, layout);
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
        gtk_widget_set_sensitive(workstation->new_window_button, editing);
    }
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
        gtk_widget_set_sensitive(workstation->save_layout_button, !editing);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->restore_layout_button != NULL) {
        gtk_widget_set_sensitive(
            workstation->restore_layout_button,
            !editing && workstation->saved_layout_text != NULL);
    }
    refresh_command_model(workstation);
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (workstation->customisation.edit_active) return UMI_STATUS_BUSY;
    status = umi_application_suite_layout_runtime_select(
        &workstation->runtime, layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_suite_layout_selector_select(
        &workstation->selector, layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_customisation_activate(
        &workstation->customisation,
        workstation->runtime.active_layout.layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = rebuild_active_layout(workstation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    workstation->changing_selection = 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workstation->selector.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Import an existing canonical layout and synchronise model, selector and GTK. */
UmiStatus umi_application_suite_gtk4_workstation_import_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *text,
    int activate,
    UmiUiWorkspaceImportReport *out_report)
{
    UmiUiWorkspaceImportOptions options =
        umi_ui_workspace_import_options_default();
    UmiUiWorkspaceImportReport local_report;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* A suite selector contains application-defined canonical layouts. Saved
     * text may replace one of them but cannot inject an unknown selector item. */
    options.conflict_policy = UMI_UI_WORKSPACE_IMPORT_REPLACE_CONFLICT;
    options.activate_imported_layout = activate != 0;
    options.allow_new_layout = false;
    status = umi_ui_workspace_customisation_import(
        &workstation->customisation, text, &options, &local_report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (activate != 0) {
        /* Selection updates runtime metadata, the dropdown and native panels
         * while retaining the imported customisation layout by identifier. */
        status = umi_application_suite_gtk4_workstation_select_layout(
            workstation, local_report.layout_id);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(workstation->customisation.active_layout_id,
                      local_report.layout_id) == 0) {
        /* A non-activating replacement still needs redrawing when it replaced
         * the layout which was already visible. */
        status = rebuild_active_layout(workstation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        workstation->revision += 1U;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_report != NULL) {
            *out_report = local_report;
        }
    }
    return status;
}

/* Save an atomic in-memory checkpoint used by the workstation header buttons. */
UmiStatus umi_application_suite_gtk4_workstation_save_checkpoint(
    UmiApplicationSuiteGtk4Workstation *workstation,
    uint64_t saved_at_ns)
{
    char *candidate;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    candidate = (char *)calloc(UMI_UI_LAYOUT_ENCODED_CAPACITY, sizeof(char));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (candidate == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = umi_application_suite_gtk4_workstation_export_layout(
        workstation,
        saved_at_ns,
        candidate,
        UMI_UI_LAYOUT_ENCODED_CAPACITY);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(candidate);
        return status;
    }
    /* Publish the complete new text only after encoding succeeds, preserving
     * the previous usable checkpoint if allocation or encoding fails. */
    free(workstation->saved_layout_text);
    workstation->saved_layout_text = candidate;
    workstation->saved_layout_at_ns = saved_at_ns;
    refresh_edit_controls(workstation);
    workstation->revision += 1U;
    return UMI_STATUS_OK;
}

/* Restore the last in-memory checkpoint through the same validated importer. */
UmiStatus umi_application_suite_gtk4_workstation_restore_checkpoint(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->saved_layout_text == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_application_suite_gtk4_workstation_import_layout(
        workstation, workstation->saved_layout_text, 1, NULL);
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
    gtk_widget_set_sensitive(
        workstation->panel_editor_apply,
        workstation->customisation.edit_active);
    gtk_label_set_text(
        GTK_LABEL(workstation->panel_editor_status),
        workstation->customisation.edit_active
            ? "Choose a region and linked context, then apply."
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

/* Disable auto-hide whenever Floating is selected because a detached panel
 * has no dock edge on which an auto-hide strip could be rendered. */
static void on_panel_editor_region_changed(
    GObject *object,
    GParamSpec *property,
    gpointer user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    guint region_index;
    (void)object;
    (void)property;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    region_index = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(workstation->panel_editor_region));
    gtk_widget_set_sensitive(
        workstation->panel_editor_auto_hide,
        region_index + 1U < G_N_ELEMENTS(PANEL_EDITOR_REGIONS));
    /* Apply this branch only when its contract condition is satisfied. */
    if (region_index + 1U == G_N_ELEMENTS(PANEL_EDITOR_REGIONS)) {
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
        "Centre", "Left", "Right", "Bottom", "Top", "Floating", NULL
    };
    GtkWidget *revealer = gtk_revealer_new();
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
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

    gtk_widget_add_css_class(root, "umicom-panel-settings-editor");
    gtk_widget_add_css_class(workstation->panel_editor_title, "heading");
    gtk_widget_add_css_class(workstation->panel_editor_status, "dim-label");
    gtk_widget_set_hexpand(workstation->panel_editor_status, TRUE);
    gtk_label_set_xalign(GTK_LABEL(workstation->panel_editor_status), 0.0F);
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_title);
    gtk_box_append(GTK_BOX(root), gtk_label_new("Dock"));
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_region);
    gtk_box_append(GTK_BOX(root), gtk_label_new("Context"));
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_context);
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_auto_hide);
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_status);
    gtk_box_append(GTK_BOX(root), cancel);
    gtk_box_append(GTK_BOX(root), workstation->panel_editor_apply);
    gtk_revealer_set_child(GTK_REVEALER(revealer), root);
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

/* Provide the on panel action operation used by this module and its client applications. */
static void on_panel_action(const char *window_id,
                            UmiWsPanelAction action,
                            void *user_data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)user_data;
    UmiUiWorkspaceLayout *layout;
    UmiUiWorkspaceWindow *window;
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
}

/* Find on layout while leaving the underlying catalogue or model owned by this module. */
static void on_layout_selected(GObject *object,
                               GParamSpec *pspec,
                               gpointer data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)data;
    guint selected;
    const UmiApplicationSuiteLayoutChoice *choice;
    (void)pspec;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->changing_selection) return;
    selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    choice = umi_application_suite_layout_selector_at(
        &workstation->selector, (size_t)selected);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (choice != NULL)
        (void)umi_application_suite_gtk4_workstation_select_layout(
            workstation, choice->layout_id);
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        gtk_label_set_text(
            GTK_LABEL(workstation->new_window_status), "");
        gtk_popover_popdown(GTK_POPOVER(workstation->new_window_popover));
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (workstation->new_window_status != NULL) {
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

/*
 * Provide the build new window popover operation used by this module and its client
 * applications.
 */
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
    status = umi_application_suite_gtk4_workstation_save_checkpoint(
        workstation,
        now_us > 0 ? (uint64_t)now_us * UINT64_C(1000) : UINT64_C(0));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->layout_status != NULL) {
        gtk_label_set_text(
            GTK_LABEL(workstation->layout_status),
            status == UMI_STATUS_OK
                ? "Layout saved"
                : "Layout could not be saved");
    }
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
    if (workstation->layout_status != NULL) {
        gtk_label_set_text(
            GTK_LABEL(workstation->layout_status),
            status == UMI_STATUS_OK
                ? "Saved layout restored"
                : "No saved layout is available");
    }
}

/* Route one shared command-bar item to the same validated operations used by
 * the visible buttons. The command centre never changes layout state itself. */
static void on_command_bar_activated(
    const UmiWsCommandBarItem *item,
    void *user_data)
{
    static const char LAYOUT_PREFIX[] = "suite.layout.";
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
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(
                   item->command_id,
                   LAYOUT_PREFIX,
                   sizeof(LAYOUT_PREFIX) - 1U) == 0) {
        (void)umi_application_suite_gtk4_workstation_select_layout(
            workstation,
            item->command_id + sizeof(LAYOUT_PREFIX) - 1U);
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
    layout = active_layout(workstation);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) { status = UMI_STATUS_INVALID_STATE; goto fail; }
    status = umi_gtk4_workspace_layout_host_create_interactive(
        layout,
        config->panel_factory,
        config->user_data,
        on_panel_action,
        workstation,
        &workstation->host);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
        GTK_MENU_BUTTON(workstation->new_window_button), "New Window");
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
    /* Release the driver's retained root before dismantling child services. */
    umi_gtk4_automation_driver_destroy(workstation->automation);
    workstation->automation = NULL;
    umi_gtk4_workspace_layout_host_destroy(workstation->host);
    workstation->host = NULL;
    /* Disconnect the borrowed callback before releasing its target. */
    (void)umi_gtk4_appearance_editor_set_changed_handler(
        workstation->appearance, NULL, NULL);
    (void)umi_gtk4_ws_command_bar_set_activated_handler(
        workstation->command_bar, NULL, NULL);
    umi_gtk4_ws_command_bar_destroy(workstation->command_bar);
    workstation->command_bar = NULL;
    umi_gtk4_ws_shell_header_destroy(workstation->identity);
    workstation->identity = NULL;
    umi_gtk4_appearance_editor_destroy(workstation->appearance);
    workstation->appearance = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->root != NULL) g_object_unref(workstation->root);
    workstation->root = NULL;
    /* The checkpoint owns its encoded buffer independently of GTK widgets. */
    free(workstation->saved_layout_text);
    workstation->saved_layout_text = NULL;
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
    const UmiApplicationSuiteLayoutChoice *choice;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return snapshot;
    choice = umi_application_suite_layout_selector_current(&workstation->selector);
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
    snapshot.revision = workstation->revision + host_snapshot.revision +
        snapshot.identity.revision + snapshot.appearance.revision +
        snapshot.command_bar.revision;
    return snapshot;
}
