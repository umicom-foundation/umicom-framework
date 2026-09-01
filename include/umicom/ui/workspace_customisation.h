/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_customisation.h
 *
 * PURPOSE:
 *   Publish the public workspace customisation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_CUSTOMISATION_H
#define UMICOM_UI_WORKSPACE_CUSTOMISATION_H
#include "umicom/ui/window_catalogue.h"
#include "umicom/ui/window_group.h"
#include "umicom/ui/layout_library.h"
#include "umicom/ui/theme_profile.h"
#define UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS 16U
typedef struct UmiUiWorkspaceCustomisation {
    UmiUiWorkspaceLayout layouts[UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS];
    size_t layout_count;
    char active_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiUiWindowCatalogue windows;
    UmiUiWindowGroupStore groups;
    UmiUiLayoutLibrary library;
    UmiUiThemeProfile theme;
    /* Keep the complete layout and context routing graph so Cancel can undo
     * every visible and behavioural change made during one edit session. */
    UmiUiWorkspaceLayout edit_baseline;
    UmiUiWindowGroupStore edit_groups_baseline;
    bool edit_active;
    uint64_t edit_started_revision;
    uint64_t revision;
} UmiUiWorkspaceCustomisation;
typedef struct UmiUiWorkspaceCustomisationSnapshot {
    size_t layouts;
    size_t available_windows;
    size_t recent_windows;
    size_t groups;
    size_t presets;
    char active_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char theme_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    bool active_layout_locked;
    bool editing;
    uint64_t revision;
} UmiUiWorkspaceCustomisationSnapshot;

/* Describe one complete panel-placement edit. Text pointers are borrowed only
 * for the duration of the apply call, so callers may safely use local buffers. */
typedef struct UmiUiWorkspacePanelSettings {
    const char *window_id;         /* Existing layout window to edit. */
    const char *placement_id;      /* Semantic dock region or floating. */
    const char *stack_id;          /* Tab stack used when the panel is docked. */
    const char *context_group_id;  /* Empty text selects no linked context. */
    UmiUiWindowGroupRole context_role; /* Source/destination routing role. */
    double x;                      /* Normalised floating left position. */
    double y;                      /* Normalised floating top position. */
    double width;                  /* Normalised floating width. */
    double height;                 /* Normalised floating height. */
    bool floating;                 /* True detaches; false docks. */
    bool auto_hidden;              /* True collapses a supported docked panel. */
} UmiUiWorkspacePanelSettings;

void umi_ui_workspace_customisation_init(UmiUiWorkspaceCustomisation *customisation);
UmiStatus umi_ui_workspace_customisation_add_layout(UmiUiWorkspaceCustomisation *customisation,const UmiUiWorkspaceLayout *layout);
UmiStatus umi_ui_workspace_customisation_activate(UmiUiWorkspaceCustomisation *customisation,const char *layout_id);
UmiUiWorkspaceLayout *umi_ui_workspace_customisation_active(UmiUiWorkspaceCustomisation *customisation);
const UmiUiWorkspaceLayout *umi_ui_workspace_customisation_active_const(
    const UmiUiWorkspaceCustomisation *customisation);
UmiStatus umi_ui_workspace_customisation_clone_layout(
    UmiUiWorkspaceCustomisation *customisation,
    const char *source_layout_id,
    const char *layout_id,
    const char *name);
UmiStatus umi_ui_workspace_customisation_remove_layout(
    UmiUiWorkspaceCustomisation *customisation,
    const char *layout_id);
UmiStatus umi_ui_workspace_customisation_rename_active(
    UmiUiWorkspaceCustomisation *customisation,
    const char *name);
UmiStatus umi_ui_workspace_customisation_begin_edit(
    UmiUiWorkspaceCustomisation *customisation);
UmiStatus umi_ui_workspace_customisation_commit_edit(
    UmiUiWorkspaceCustomisation *customisation);
UmiStatus umi_ui_workspace_customisation_cancel_edit(
    UmiUiWorkspaceCustomisation *customisation);
UmiStatus umi_ui_workspace_customisation_open_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *tool_id,
    const char *group_id,
    bool floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity);
/* Dock a window into a named region and tab stack during an edit session. */
UmiStatus umi_ui_workspace_customisation_dock_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *placement_id,
    const char *stack_id);
/* Undock a window into bounded floating geometry during an edit session. */
UmiStatus umi_ui_workspace_customisation_float_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height);
/* Collapse or restore a docked panel without removing it from the layout. */
UmiStatus umi_ui_workspace_customisation_set_auto_hidden(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    bool auto_hidden);
/* Report whether a window is currently represented by an auto-hide strip. */
bool umi_ui_workspace_customisation_window_is_auto_hidden(
    const UmiUiWorkspaceCustomisation *customisation,
    const char *window_id);
/* Return safe centre-panel defaults which a caller can adjust before apply. */
UmiUiWorkspacePanelSettings umi_ui_workspace_panel_settings_default(
    const char *window_id);
/* Apply placement, floating, auto-hide and context as one rollback-safe edit. */
UmiStatus umi_ui_workspace_customisation_apply_panel_settings(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspacePanelSettings *settings);
/* Close a non-critical window while preserving the catalogue definition. */
UmiStatus umi_ui_workspace_customisation_close_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id);
UmiStatus umi_ui_workspace_customisation_assign_context_group(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *context_group_id,
    UmiUiWindowGroupRole role);
UmiStatus umi_ui_workspace_customisation_clear_context_group(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id);
UmiStatus umi_ui_workspace_customisation_set_theme(UmiUiWorkspaceCustomisation *customisation,const UmiUiThemeProfile *theme);
void umi_ui_workspace_customisation_snapshot(const UmiUiWorkspaceCustomisation *customisation,UmiUiWorkspaceCustomisationSnapshot *out_snapshot);
#endif
