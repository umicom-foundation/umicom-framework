/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace_model_internal.h
 *
 * PURPOSE:
 *   Declare the workspace model internal contract shared by Framework services
 *   and thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework - private Batch 34 workspace-model storage.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_MODEL_INTERNAL_H
#define UMICOM_PROJECT_WORKSPACE_MODEL_INTERNAL_H

#include "umicom/project/workspace_model.h"
#include "umicom/project/workspace_group.h"
#include "umicom/project/workspace_setting.h"
#include "umicom/project/workspace_exclusion.h"

struct UmiProjectWorkspaceModel {
    UmiProjectWorkspace *projects; /* Borrowed; the caller remains the owner. */
    UmiProjectWorkspaceRootSnapshot roots[UMI_PROJECT_WORKSPACE_ROOT_CAPACITY];
    UmiProjectWorkspaceMemberSnapshot members[UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY];
    UmiProjectWorkspaceGroupSnapshot groups[UMI_PROJECT_WORKSPACE_GROUP_CAPACITY];
    UmiProjectWorkspaceSettingSnapshot settings[UMI_PROJECT_WORKSPACE_SETTING_CAPACITY];
    UmiProjectWorkspaceExclusionSnapshot exclusions[UMI_PROJECT_WORKSPACE_EXCLUSION_CAPACITY];
    size_t root_count;
    size_t member_count;
    size_t group_count;
    size_t setting_count;
    size_t exclusion_count;
    char active_project_id[128];
    uint64_t revision;
};

/**
 * Provide the project workspace model copy text operation used by this module and its
 * client applications.
 */
void umi_project_workspace_model_copy_text(
    char *destination, size_t capacity, const char *source);
/**
 * Provide the project workspace model find root index operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_model_find_root_index(
    const UmiProjectWorkspaceModel *model, const char *root_id);
/**
 * Provide the project workspace model find member index operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_model_find_member_index(
    const UmiProjectWorkspaceModel *model, const char *member_id);
/**
 * Provide the project workspace model find group index operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_model_find_group_index(
    const UmiProjectWorkspaceModel *model, const char *group_id);
/**
 * Provide the project workspace model member is selectable operation used by this module
 * and its client applications.
 */
int umi_project_workspace_model_member_is_selectable(
    const UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceMemberSnapshot *member);

#endif
