/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/workspace_runtime_policy_catalogue.c
 *
 * PURPOSE:
 *   Build, query and validate one live workspace policy for every portable
 *   application recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/workspace_runtime_policy_catalogue.h"

#include <string.h>

#include "umicom/application/component/recipe_catalogue.h"

#define WORKSPACE_RUNTIME_POLICY(recipe, startup, checkpoint, interval, focus, background, restore_focus_value, share_context_value, background_commands) \
    {sizeof(UmiApplicationPresentationWorkspaceRuntimePolicy),                           \
     UMI_APPLICATION_PRESENTATION_WORKSPACE_POLICY_API_VERSION, (recipe), (startup),      \
     (checkpoint), (interval), (focus), (background), (restore_focus_value),              \
     (share_context_value), (background_commands)}

static const UmiApplicationPresentationWorkspaceRuntimePolicy POLICIES[] = {
#include "workspace_policies/workspace_policy_records.inc"
};

#undef WORKSPACE_RUNTIME_POLICY

size_t umi_application_presentation_workspace_runtime_policy_catalogue_count(void)
{
    return sizeof(POLICIES) / sizeof(POLICIES[0]);
}

const UmiApplicationPresentationWorkspaceRuntimePolicy *
umi_application_presentation_workspace_runtime_policy_catalogue_at(size_t index)
{
    return index <
                   umi_application_presentation_workspace_runtime_policy_catalogue_count()
        ? &POLICIES[index]
        : NULL;
}

const UmiApplicationPresentationWorkspaceRuntimePolicy *
umi_application_presentation_workspace_runtime_policy_catalogue_find(
    const char *recipe_id)
{
    size_t index;
    if (recipe_id == NULL) return NULL;
    for (index = 0U;
         index <
             umi_application_presentation_workspace_runtime_policy_catalogue_count();
         ++index) {
        if (strcmp(POLICIES[index].recipe_id, recipe_id) == 0)
            return &POLICIES[index];
    }
    return NULL;
}

size_t umi_application_presentation_workspace_runtime_policy_catalogue_checkpoint_count(
    UmiApplicationPresentationCheckpointPolicy policy)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U;
         index <
             umi_application_presentation_workspace_runtime_policy_catalogue_count();
         ++index) {
        if (POLICIES[index].checkpoint_policy == policy) count += 1U;
    }
    return count;
}

UmiStatus
umi_application_presentation_workspace_runtime_policy_catalogue_validate(void)
{
    size_t index;
    if (umi_application_presentation_workspace_runtime_policy_catalogue_count() !=
        umi_application_component_recipe_catalogue_count()) {
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U;
         index <
             umi_application_presentation_workspace_runtime_policy_catalogue_count();
         ++index) {
        size_t previous;
        UmiStatus status =
            umi_application_presentation_workspace_runtime_policy_validate(
                &POLICIES[index]);
        if (status != UMI_STATUS_OK) return status;
        for (previous = 0U; previous < index; ++previous) {
            if (strcmp(POLICIES[previous].recipe_id,
                       POLICIES[index].recipe_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}
