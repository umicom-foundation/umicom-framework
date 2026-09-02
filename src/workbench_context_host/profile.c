/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/profile.c
 *
 * PURPOSE:
 *   Implement bounded application-profile construction and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/profile.h"

#include <string.h>

/*
 * Initialise workbench context host profile from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_profile_init(
    UmiWorkbenchContextHostProfile *profile,
    const char *profile_id,
    const char *application_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) return;
    memset(profile, 0, sizeof(*profile));
    profile->structure_size = (uint32_t)sizeof(*profile);
    profile->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            profile->profile_id, sizeof(profile->profile_id), profile_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            profile->application_id, sizeof(profile->application_id), application_id);
    }
}

/*
 * Provide the workbench context host profile set title operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_profile_set_title(
    UmiWorkbenchContextHostProfile *profile,
    const char *title)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || title == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_copy_text(
        profile->title, sizeof(profile->title), title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++profile->revision;
    return status;
}

/*
 * Provide the workbench context host profile add group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_profile_add_group(
    UmiWorkbenchContextHostProfile *profile,
    const UmiWorkbenchContextHostGroupDefinition *definition)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || definition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_group_definition_validate(definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->group_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(profile->groups[index].group_id, definition->group_id) == 0) {
            profile->groups[index] = *definition;
            ++profile->revision;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (profile->group_count >= UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_GROUPS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    profile->groups[profile->group_count++] = *definition;
    ++profile->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host profile add endpoint operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_profile_add_endpoint(
    UmiWorkbenchContextHostProfile *profile,
    const UmiWorkbenchContextHostEndpoint *endpoint)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || endpoint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_endpoint_validate(endpoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->endpoint_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(profile->endpoints[index].endpoint_id, endpoint->endpoint_id) == 0) {
            profile->endpoints[index] = *endpoint;
            ++profile->revision;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (profile->endpoint_count >= UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_ENDPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    profile->endpoints[profile->endpoint_count++] = *endpoint;
    ++profile->revision;
    return UMI_STATUS_OK;
}

/*
 * Check that workbench context host profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_host_profile_validate(
    const UmiWorkbenchContextHostProfile *profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || profile->structure_size != sizeof(*profile)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_host_text_is_valid(
            profile->profile_id, sizeof(profile->profile_id)) ||
        profile->profile_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            profile->application_id, sizeof(profile->application_id)) ||
        profile->application_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            profile->title, sizeof(profile->title))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->group_count > UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_GROUPS ||
        profile->endpoint_count > UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_ENDPOINTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->group_count; ++index) {
        /* Apply this operation only while the related capability or state is available. */
        if (umi_workbench_context_host_group_definition_validate(
                &profile->groups[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->endpoint_count; ++index) {
        /* Apply this operation only while the related capability or state is available. */
        if (umi_workbench_context_host_endpoint_validate(
                &profile->endpoints[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host profile default group operation used by this module
 * and its client applications.
 */
const char *umi_workbench_context_host_profile_default_group(
    const UmiWorkbenchContextHostProfile *profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->group_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (profile->groups[index].default_active) {
            return profile->groups[index].group_id;
        }
    }
    return profile->group_count > 0U ? profile->groups[0].group_id : NULL;
}
