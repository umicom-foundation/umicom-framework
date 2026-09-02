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

/*
 * Return the number of records represented by application presentation workspace runtime
 * policy catalogue without changing their state.
 */
size_t umi_application_presentation_workspace_runtime_policy_catalogue_count(void)
{
    return sizeof(POLICIES) / sizeof(POLICIES[0]);
}

/*
 * Find application presentation workspace runtime policy catalogue while leaving the
 * underlying catalogue or model owned by this module.
 */
const UmiApplicationPresentationWorkspaceRuntimePolicy *
umi_application_presentation_workspace_runtime_policy_catalogue_at(size_t index)
{
    return index <
                   umi_application_presentation_workspace_runtime_policy_catalogue_count()
        ? &POLICIES[index]
        : NULL;
}

/*
 * Find application presentation workspace runtime policy catalogue while leaving the
 * underlying catalogue or model owned by this module.
 */
const UmiApplicationPresentationWorkspaceRuntimePolicy *
umi_application_presentation_workspace_runtime_policy_catalogue_find(
    const char *recipe_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (recipe_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index <
             umi_application_presentation_workspace_runtime_policy_catalogue_count();
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(POLICIES[index].recipe_id, recipe_id) == 0)
            return &POLICIES[index];
    }
    return NULL;
}

/*
 * Return the number of records represented by application presentation workspace runtime
 * policy catalogue checkpoint without changing their state.
 */
size_t umi_application_presentation_workspace_runtime_policy_catalogue_checkpoint_count(
    UmiApplicationPresentationCheckpointPolicy policy)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index <
             umi_application_presentation_workspace_runtime_policy_catalogue_count();
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (POLICIES[index].checkpoint_policy == policy) count += 1U;
    }
    return count;
}

/*
 * Check that application presentation workspace runtime policy catalogue satisfies its
 * contract before another service relies on it.
 */
UmiStatus
umi_application_presentation_workspace_runtime_policy_catalogue_validate(void)
{
    size_t index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_presentation_workspace_runtime_policy_catalogue_count() !=
        umi_application_component_recipe_catalogue_count()) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index <
             umi_application_presentation_workspace_runtime_policy_catalogue_count();
         ++index) {
        size_t previous;
        UmiStatus status =
            umi_application_presentation_workspace_runtime_policy_validate(
                &POLICIES[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Visit each bounded item once so every record receives the same rule. */
        for (previous = 0U; previous < index; ++previous) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(POLICIES[previous].recipe_id,
                       POLICIES[index].recipe_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}
