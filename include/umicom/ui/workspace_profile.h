/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_profile.h
 *
 * PURPOSE:
 *   Define reusable named workbench chrome profiles. A profile describes the
 *   visibility and preferred size of the primary, auxiliary and bottom tool
 *   regions without depending on GTK, Qt, Wt or any other frontend toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A workspace profile is similar to a saved IDE layout tab. Applications add
 * profiles such as Develop, Focus or Debug; frontend adapters render a picker
 * and the Framework remains the authoritative owner of the selected profile.
 */

#ifndef UMICOM_UI_WORKSPACE_PROFILE_H
#define UMICOM_UI_WORKSPACE_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ui/types.h"
#include "umicom/ui/workbench_state.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_WORKSPACE_PROFILE_MAX 64U
#define UMI_UI_WORKSPACE_PROFILE_MAX_PANES 24U

/*
 * A saved layout records pane placement independently of GTK. This is what
 * allows a tab dragged from the left tool area to the bottom tool area to be
 * restored by another frontend, or after Studio is restarted.
 */
typedef struct UmiUiWorkspacePanePlacement {
    char pane_id[UMI_UI_ID_CAPACITY];
    UmiUiPlacement placement;
    int32_t order;
    int visible;
} UmiUiWorkspacePanePlacement;

/**
 * Represent the ui workspace profile snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiWorkspaceProfileSnapshot {
    char profile_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    int sidebar_visible;
    int auxiliary_sidebar_visible;
    int bottom_panel_visible;
    int32_t sidebar_size;
    int32_t auxiliary_sidebar_size;
    int32_t bottom_panel_size;
    int32_t order;
    int active;
    int built_in;
    int locked;
    size_t pane_count;
    UmiUiWorkspacePanePlacement panes[UMI_UI_WORKSPACE_PROFILE_MAX_PANES];
    UmiUiEditorSplitMode editor_split_mode;
    int32_t editor_split_ratio;
} UmiUiWorkspaceProfileSnapshot;

/**
 * Represent the ui workspace profile model data shared with callers of this public
 * contract.
 */
typedef struct UmiUiWorkspaceProfileModel UmiUiWorkspaceProfileModel;

/**
 * Initialise ui workspace profile model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_workspace_profile_model_create(
    UmiUiWorkspaceProfileModel **out_model);
/**
 * Release or reset state held by ui workspace profile model so the same storage can be
 * reused safely.
 */
void umi_ui_workspace_profile_model_destroy(UmiUiWorkspaceProfileModel *model);
/**
 * Provide the ui workspace profile model upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_profile_model_upsert(
    UmiUiWorkspaceProfileModel *model,
    const UmiUiWorkspaceProfileSnapshot *profile);
/**
 * Remove ui workspace profile model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_workspace_profile_model_remove(
    UmiUiWorkspaceProfileModel *model,
    const char *profile_id);
/**
 * Find ui workspace profile model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_workspace_profile_model_find(
    const UmiUiWorkspaceProfileModel *model,
    const char *profile_id,
    UmiUiWorkspaceProfileSnapshot *out_profile);
/**
 * Find ui workspace profile model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_workspace_profile_model_at(
    const UmiUiWorkspaceProfileModel *model,
    size_t index,
    UmiUiWorkspaceProfileSnapshot *out_profile);
/**
 * Provide the ui workspace profile model set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_profile_model_set_active(
    UmiUiWorkspaceProfileModel *model,
    const char *profile_id);
/**
 * Provide the ui workspace profile model rename operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_profile_model_rename(
    UmiUiWorkspaceProfileModel *model,
    const char *profile_id,
    const char *label,
    const char *description);
/**
 * Provide the ui workspace profile model set locked operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_profile_model_set_locked(
    UmiUiWorkspaceProfileModel *model,
    const char *profile_id,
    int locked);
/**
 * Return the number of records represented by ui workspace profile model without changing
 * their state.
 */
size_t umi_ui_workspace_profile_model_count(
    const UmiUiWorkspaceProfileModel *model);
/**
 * Provide the ui workspace profile model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_workspace_profile_model_revision(
    const UmiUiWorkspaceProfileModel *model);

#ifdef __cplusplus
}
#endif

#endif
