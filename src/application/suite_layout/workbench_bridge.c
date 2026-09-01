/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/workbench_bridge.c
 *
 * PURPOSE:
 *   Bridge canonical suite layouts into the authoritative workbench profile
 *   model without creating another UI shell or application-owned layout engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/workbench_bridge.h"

#include "umicom/application/suite_layout/profile_projection.h"
#include "umicom/ui/internal/workspace_profile_owner.h"
#include <stdio.h>

/* Project and install every canonical layout through one workbench owner. */
UmiStatus umi_application_suite_layout_register_workbench_profiles_resolved(
    const UmiApplicationExperienceDefinition *experience,
    UmiUiWorkbench *workbench,
    int activate_default,
    UmiApplicationSuiteLayoutPaneResolver resolver,
    void *user_data)
{
    UmiUiWorkspaceProfileModel *profiles;
    size_t index;
    UmiStatus status;

    /* Both canonical source metadata and the target workbench are required. */
    if (experience == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_experience_validate(experience);
    /* Do not install profiles from an invalid panel or layout graph. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    profiles = umi_ui_workbench_workspace_profiles(workbench);
    /* A workbench without its authoritative profile model is not ready. */
    if (profiles == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Every named layout becomes a built-in profile in catalogue order. */
    for (index = 0U; index < experience->layout_count; ++index) {
        UmiUiWorkspaceProfileSnapshot profile;

        status = umi_application_suite_layout_profile_project_resolved(
            experience,
            &experience->layouts[index],
            (int32_t)(100U + index),
            resolver,
            user_data,
            &profile);
        /* One failed projection stops registration before publishing a
         * misleading replacement profile. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        /* Suite layouts are Framework-owned built-ins. Recomposition must be
         * able to refresh their pane IDs without weakening generic upsert
         * protection for locked or user-owned profiles. */
        status = umi_ui_workspace_profile_model_install_built_in(
            profiles, &profile);
        /* Preserve the exact profile-store error for the application shell. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    /* Activation is optional because callers may register several products
     * before deciding which workspace should become visible. */
    if (activate_default) {
        char profile_id[UMI_UI_ID_CAPACITY];
        int written = snprintf(
            profile_id,
            sizeof(profile_id),
            "%s.%s",
            experience->application_id,
            experience->default_layout_id);

        /* The profile lookup key must fit without truncation. */
        if (written < 0 || (size_t)written >= sizeof(profile_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        return umi_ui_workbench_activate_workspace_profile(
            workbench, profile_id);
    }
    return UMI_STATUS_OK;
}

/* Preserve the simple workbench API by using canonical pane identifiers. */
UmiStatus umi_application_suite_layout_register_workbench_profiles(
    const UmiApplicationExperienceDefinition *experience,
    UmiUiWorkbench *workbench,
    int activate_default)
{
    return umi_application_suite_layout_register_workbench_profiles_resolved(
        experience, workbench, activate_default, NULL, NULL);
}
