/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace_refresh.c
 * PURPOSE: Compare discovery with current membership without deleting state.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace_refresh.h"

#include "umicom/platform/path.h"
#include "umicom/project/workspace_model.h"

#include <string.h>

UmiStatus umi_project_workspace_model_plan_refresh(
    const UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceDiscoveryOptions *options,
    UmiProjectWorkspaceRefreshSnapshot *out_refresh)
{
    UmiProjectWorkspaceModelSnapshot model_snapshot;
    UmiStatus status;
    size_t discovered;
    size_t member_index;
    if (model == NULL || out_refresh == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_refresh, 0, sizeof(*out_refresh));
    out_refresh->struct_size = (uint32_t)sizeof(*out_refresh);
    out_refresh->api_version = UMI_PROJECT_WORKSPACE_REFRESH_API_VERSION;
    if (umi_project_workspace_model_snapshot(model, &model_snapshot) != UMI_STATUS_OK)
        return UMI_STATUS_INTERNAL_ERROR;
    out_refresh->source_revision = model_snapshot.revision;
    out_refresh->existing_project_count = model_snapshot.member_count;
    status = umi_project_workspace_model_discover(
        model, options, &out_refresh->discovery);
    if (status != UMI_STATUS_OK) return status;
    for (discovered = 0U; discovered < out_refresh->discovery.project_count;
         ++discovered) {
        int found = 0;
        for (member_index = 0U;
             member_index < umi_project_workspace_model_member_count(model);
             ++member_index) {
            UmiProjectWorkspaceMemberSnapshot member;
            UmiProjectDescriptorSnapshot project;
            if (umi_project_workspace_model_member_at(
                    model, member_index, &member) != UMI_STATUS_OK ||
                umi_project_descriptor_registry_find(
                    umi_project_workspace_descriptor(
                        umi_project_workspace_model_projects(model)),
                    member.project_id, &project) != UMI_STATUS_OK) continue;
            if (umi_path_equal(project.root_uri,
                    out_refresh->discovery.projects[discovered].project_directory)) {
                found = 1;
                break;
            }
        }
        if (found) out_refresh->unchanged_project_count += 1U;
        else out_refresh->import_candidate_count += 1U;
    }
    if (out_refresh->existing_project_count > out_refresh->unchanged_project_count)
        out_refresh->missing_project_count = out_refresh->existing_project_count -
                                             out_refresh->unchanged_project_count;
    out_refresh->requires_confirmation =
        out_refresh->import_candidate_count != 0U ||
        out_refresh->missing_project_count != 0U;
    return UMI_STATUS_OK;
}
