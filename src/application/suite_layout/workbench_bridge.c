/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/workbench_bridge.c
 *
 * PURPOSE:
 *   Bridge canonical suite layouts into the authoritative workbench profile
 *   model without creating another UI shell or application-owned layout engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/workbench_bridge.h"

#include <stdio.h>
#include "umicom/application/suite_layout/profile_projection.h"

UmiStatus umi_application_suite_layout_register_workbench_profiles(
    const UmiApplicationExperienceDefinition *experience,
    UmiUiWorkbench *workbench,
    int activate_default,
    UmiApplicationSuiteLayoutPaneResolver resolver,
    void *user_data)
{
    UmiUiWorkspaceProfileModel *profiles;
    size_t index;
    UmiStatus status;
    if (experience == NULL || workbench == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    if (status != UMI_STATUS_OK) return status;
    profiles = umi_ui_workbench_workspace_profiles(workbench);
    if (profiles == NULL) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < experience->layout_count; ++index) {
        UmiUiWorkspaceProfileSnapshot profile;
        status = umi_application_suite_layout_profile_project(
            experience, &experience->layouts[index],
            (int32_t)(100U + index), resolver, user_data, &profile);
        if (status != UMI_STATUS_OK) return status;
        status = umi_ui_workspace_profile_model_upsert(profiles, &profile);
        if (status != UMI_STATUS_OK) return status;
    }
    if (activate_default) {
        char profile_id[UMI_UI_ID_CAPACITY];
        int written = snprintf(profile_id, sizeof(profile_id), "%s.%s",
                               experience->application_id,
                               experience->default_layout_id);
        if (written < 0 || (size_t)written >= sizeof(profile_id))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        return umi_ui_workbench_activate_workspace_profile(workbench, profile_id);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_suite_layout_register_workbench_profiles(
    const UmiApplicationExperienceDefinition *experience,
    UmiUiWorkbench *workbench,
    int activate_default)
{
    return umi_application_suite_layout_register_workbench_profiles(
        experience, workbench, activate_default, NULL, NULL);
}
