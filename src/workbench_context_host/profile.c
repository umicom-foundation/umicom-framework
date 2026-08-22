/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/profile.c
 *
 * PURPOSE:
 *   Implement bounded application-profile construction and validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/profile.h"

#include <string.h>

void umi_workbench_context_host_profile_init(
    UmiWorkbenchContextHostProfile *profile,
    const char *profile_id,
    const char *application_id)
{
    if (profile == NULL) return;
    memset(profile, 0, sizeof(*profile));
    profile->structure_size = (uint32_t)sizeof(*profile);
    profile->revision = 1U;
    if (profile_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            profile->profile_id, sizeof(profile->profile_id), profile_id);
    }
    if (application_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            profile->application_id, sizeof(profile->application_id), application_id);
    }
}

UmiStatus umi_workbench_context_host_profile_set_title(
    UmiWorkbenchContextHostProfile *profile,
    const char *title)
{
    UmiStatus status;
    if (profile == NULL || title == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_copy_text(
        profile->title, sizeof(profile->title), title);
    if (status == UMI_STATUS_OK) ++profile->revision;
    return status;
}

UmiStatus umi_workbench_context_host_profile_add_group(
    UmiWorkbenchContextHostProfile *profile,
    const UmiWorkbenchContextHostGroupDefinition *definition)
{
    size_t index;
    UmiStatus status;
    if (profile == NULL || definition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_group_definition_validate(definition);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < profile->group_count; ++index) {
        if (strcmp(profile->groups[index].group_id, definition->group_id) == 0) {
            profile->groups[index] = *definition;
            ++profile->revision;
            return UMI_STATUS_OK;
        }
    }
    if (profile->group_count >= UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_GROUPS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    profile->groups[profile->group_count++] = *definition;
    ++profile->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_profile_add_endpoint(
    UmiWorkbenchContextHostProfile *profile,
    const UmiWorkbenchContextHostEndpoint *endpoint)
{
    size_t index;
    UmiStatus status;
    if (profile == NULL || endpoint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_endpoint_validate(endpoint);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < profile->endpoint_count; ++index) {
        if (strcmp(profile->endpoints[index].endpoint_id, endpoint->endpoint_id) == 0) {
            profile->endpoints[index] = *endpoint;
            ++profile->revision;
            return UMI_STATUS_OK;
        }
    }
    if (profile->endpoint_count >= UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_ENDPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    profile->endpoints[profile->endpoint_count++] = *endpoint;
    ++profile->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_profile_validate(
    const UmiWorkbenchContextHostProfile *profile)
{
    size_t index;
    if (profile == NULL || profile->structure_size != sizeof(*profile)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
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
    if (profile->group_count > UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_GROUPS ||
        profile->endpoint_count > UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_ENDPOINTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < profile->group_count; ++index) {
        if (umi_workbench_context_host_group_definition_validate(
                &profile->groups[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    for (index = 0U; index < profile->endpoint_count; ++index) {
        if (umi_workbench_context_host_endpoint_validate(
                &profile->endpoints[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

const char *umi_workbench_context_host_profile_default_group(
    const UmiWorkbenchContextHostProfile *profile)
{
    size_t index;
    if (profile == NULL) return NULL;
    for (index = 0U; index < profile->group_count; ++index) {
        if (profile->groups[index].default_active) {
            return profile->groups[index].group_id;
        }
    }
    return profile->group_count > 0U ? profile->groups[0].group_id : NULL;
}
