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
    UmiUiWorkspaceLayout edit_baseline;
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
UmiStatus umi_ui_workspace_customisation_set_theme(UmiUiWorkspaceCustomisation *customisation,const UmiUiThemeProfile *theme);
void umi_ui_workspace_customisation_snapshot(const UmiUiWorkspaceCustomisation *customisation,UmiUiWorkspaceCustomisationSnapshot *out_snapshot);
#endif
